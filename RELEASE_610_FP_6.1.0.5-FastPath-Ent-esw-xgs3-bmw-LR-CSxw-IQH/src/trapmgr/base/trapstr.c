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

#include <l7_common.h>
#include <trapstr.h>

const L7_uchar8 *const coldstart_str = "Cold Start: Unit: %d";
const L7_uchar8 *const warmstart_str = "Warm Start: Unit: %d";
const L7_uchar8 *const mylinkdown_str = "My Link Down: Unit: %d Slot: %d Port: %d";
const L7_uchar8 *const authfail_str = "Authent. Failure: Unit: %d IP Address: %s";
const L7_uchar8 *const linkup_str = "Link Up: %s";
const L7_uchar8 *const linkdown_str = "Link Down: %s";
const L7_uchar8 *const multiuser_str = "Multiple Users: Unit: %d Slot: %d Port: %d";
const L7_uchar8 *const linkstatus_str = "Diagnostics Link Failure: IfIndex: %d";
const L7_uchar8 *const securityint_str = "Security Intruder Detected: Unit: %d Slot: %d Port: %d Address: %s";
const L7_uchar8 *const newroot_str = "New Spanning Tree Root: %u, Unit: %d";
const L7_uchar8 *const loop_inconsistent_start_str = "Transitioning Into Loop Inconsistent State: MSTID: %d Unit: %d Slot: %d Port: %d";
const L7_uchar8 *const loop_inconsistent_end_str = "Transitioning Out Of Loop Inconsistent State: MSTID: %d Unit: %d Slot: %d Port: %d";
const L7_uchar8 *const topochange_str = "Spanning Tree Topology Change: %u, Unit: %d";
const L7_uchar8 *const rmonevent_str = "RMON Event: %s";
const L7_uchar8 *const bcaststormexc_str = "Broadcast Storm Threshold Exceeded: Unit: %d Slot: %d Port: %d";
const L7_uchar8 *const framesdiscardexc_str = "Frames Discarded Threshold Exceeded: Unit: %d Slot: %d Port: %d";
const L7_uchar8 *const bcaststorm_start="Broadcast Storm Recovery Started: Unit: %d Slot: %d Port: %d";
const L7_uchar8 *const bcaststorm_end="Broadcast Storm Recovery Ended: Unit: %d Slot: %d Port: %d";
const L7_uchar8 *const general_system = "General System: Unit: %d";
const L7_uchar8 *const vlan_request_fail = "VLAN issue request failed: VLAN: %d";
const L7_uchar8 *const vlan_delete_last_str = "Last or default VLAN deleted: VLAN: %d";
const L7_uchar8 *const vlan_default_cfg_fail_str = "Default VLAN configuration failed: VLAN: %d";
const L7_uchar8 *const vlan_restore_fail_str = "Problem restoring previous VLAN: VLAN: %d";
const L7_uchar8 *const fan_status_str = "Fan Status: %s";
const L7_uchar8 *const power_supply_status_str = "Power Supply Status: %s";

const L7_uchar8 *const dot1q_str = "dot1q: Unit: %d";

const L7_uchar8 *const rmon_rising_alarm_str = "RMON Rising Alarm: Unit: %d  Index: %d";
const L7_uchar8 *const rmon_falling_alarm_str = "RMON Falling Alarm: Unit: %d  Index: %d";
const L7_uchar8 *const failed_login_str = "Failed User Login: Unit: %d User ID: %s";
const L7_uchar8 *const user_login_lockout_str = "Local User Locked out: Unit: %d User ID: %s";
const L7_uchar8 *const mac_lock_violation_str = "MAC Lock Violation: Unit: %d Slot: %d Port: %d, %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x, vlan %d";
const L7_uchar8 *const poe_port_change_str = "PoE: Unit: %d Slot: %d Port: %d %s";
const L7_uchar8 *const lldp_rem_change_str = "Unit- %d LLDP Remote Table Change: Inserts: %i, Deletes: %i, Drops: %i, Ageouts: %i";

const L7_uchar8 *const dot1x_unauth_host_str = "802.1X Unauthorized Access Attempt: %d Slot: %d Port: %d, %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x, vlan %d";
const L7_uchar8 *const acl_ip_rule_deny_str = "IP ACL ID %d, Rule %d, Deny Match - %s time(s)";
const L7_uchar8 *const acl_mac_rule_deny_str = "MAC ACL %s, Rule %d, Deny Match - %s time(s)";
const L7_uchar8 *const configChanged_str = "'startup-config' has changed.";
const L7_uchar8 *const tftpEnd_str = "TFTP of %s is completed with exit code = %d";
const L7_uchar8 *const tftpAbort_str = "TFTP of %s is aborted with exit code = %d";
const L7_uchar8 *const tftpStart_str = "TFTP of %s is started";
const L7_uchar8 *const linkFailure_str = "Link on %s is failed";
const L7_uchar8 *const vlanDynPortAdded_str = "%s is added to the VLAN %d dynamically";
const L7_uchar8 *const vlanDynPortRemoved_str = "%s is removed from the VLAN %d dynamically";
const L7_uchar8 *const stackMasterFailed_str = "Stack Master unit %d is failed";
const L7_uchar8 *const stackNewMasterElected_str = "Unit %d is the new stack master, Old stack master unit is %d";
const L7_uchar8 *const stackMemberUnitFailed_str = "Stack member unit %d is failed";
const L7_uchar8 *const stackMemberUnitAdded_str = "Unit %d is added to the stack";
const L7_uchar8 *const stackMemberUnitRemoved_str = "Unit %d is removed from the stack";
const L7_uchar8 *const stackSplitMasterReport_str = "Stack split. The %s units are lost";
const L7_uchar8 *const stackSplitNewMasterReport_str = "Splitted stack elected unit %d as master. Old master is unit %d";
const L7_uchar8 *const stackRejoined_str = "Stack got rejoined. Current units in the stack are %s";
const L7_uchar8 *const stackLinkFailed_str = "Unit %d identified a link failure in the stack. %s";
const L7_uchar8 *const dot1dStpPortStateForwarding_str = "%s is transitioned from the Learning state to the Forwarding state in instance %d";
const L7_uchar8 *const dot1dStpPortStateNotForwarding_str = "%s is transitioned from the Forwarding state to the Blocking state in instance %d";
const L7_uchar8 *const trunkPortAdded_str = "%s is added to the trunk %s";
const L7_uchar8 *const trunkPortRemoved_str = "%s is removed from the trunk %s";
const L7_uchar8 *const lockPort_str = "%s that is blocked from Mac learning has received a request with new source Mac address %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x";
const L7_uchar8 *const vlanDynVlanAdded_str = "A dynamic VLAN %d is added via GVRP";
const L7_uchar8 *const vlanDynVlanRemoved_str = "Dynamic VLAN %d has been removed via GVRP";
const L7_uchar8 *const envMonFanStateChange_str = "Fan %d has changed the state on unit %d to %d";
const L7_uchar8 *const envMonPowerSupplyStateChange_str = "Powersupply %d has changed the state on unit %d to %d";
const L7_uchar8 *const envMonTemperatureRisingAlarm_str = "Temperature of unit %d has exceeded the device specific limit";
const L7_uchar8 *const copyFinished_str = "Unit %d has finished the copy operation successfully";
const L7_uchar8 *const copyFailed_str = "Copy operation has failed in unit %d";
const L7_uchar8 *const dot1xPortStatusAuthorized_str = "%s status is authorized";
const L7_uchar8 *const dot1xPortStatusUnauthorized_str = "%s status is Unauthorized";
const L7_uchar8 *const stpElectedAsRoot_str = "Unit %d elected as the new STP root";
const L7_uchar8 *const stpNewRootElected_str = "Instance %d has elected a new STP root: %4.4x:%4.4x:%4.4x:%4.4x";
const L7_uchar8 *const invalidUserLoginAttempted_str = "An invalid user tried to login through %s from %s";
const L7_uchar8 *const managementACLViolation_str = "Management ACL violation on %s from %s";
const L7_uchar8 *const sfpInserted_str = "SFP Inserted in Unit %d, Interface %d";
const L7_uchar8 *const sfpRemoved_str = "SFP Removed from Unit %d, Interface %d";
const L7_uchar8 *const xfpInserted_str = "10G Optical Transceiver Inserted in Unit %d, Interface %d";
const L7_uchar8 *const xfpRemoved_str = "10G Optical Transceiver Removed on Unit %d, Interface %d";
const L7_uchar8 *const lldp_med_topo_change_str = "LLDP-MED Topology Change Detected: ChassisIDSubtype: %d, ChassisID: %s, DeviceClass: %d";
const L7_uchar8 *const dai_intf_error_disabled_str = "DAI error disabled interface: Unit: %d Slot: %d Port: %d";
const L7_uchar8 *const ds_intf_error_disabled_str = "DHCP Snooping error disabled interface: Unit: %d Slot: %d Port: %d";

const L7_uchar8 *const temperature_change_alarm_str = "Temperature change alarm: Sensor ID: %d Event: %d";
const L7_uchar8 *const fan_state_change_alarm_str = "Fan state change alarm: Fan ID: %d Event: %d";
const L7_uchar8 *const powsupply_state_change_alarm_str = "Power supply state change alarm: Power supply ID: %d Event: %d";

const L7_uchar8 *const entConfigChanged_str = "Entity Database: Configuration Changed";

#ifdef FEAT_METRO_CPE_V1_0
L7_char8 no_startup_config_str[] = "No startup-config file found";
#endif

#ifdef L7_DOT1AG_PACKAGE
const L7_uchar8 *const dot1ag_cfm_defect_notify_str = "DOT1AG CFM Highest Priority Defect - MdIndex:[%d] MaIndex:[%d] MepId:[%d] Defect:[%s]";
#endif /* L7_DOT1AG_PACKAGE */
