/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename trapapi.h
*
* @purpose Trap Manager API Functions, Constants and Data Structures
*
* @component trapmgr
*
* @comments none
*
* @create 08/31/2000
*
* @author bmutz
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef INCLUDE_TRAPAPI_H
#define INCLUDE_TRAPAPI_H



#include "trapmgr_exports.h"
#include "trapmgr_exports.h"
/*********************************************************************
* @purpose  Returns Trap Mangaer's trap authentication state
*
* @param    void
*
* @returns  state   trap authentication state (L7_ENABLE or L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapAuth(void);

/*********************************************************************
* @purpose  Sets Trap Mangaer's trap authentication state
*
* @param    state   trap authentication state (L7_ENABLE or L7_DISABLE)
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrSetTrapAuth(L7_uint32 state);

/*********************************************************************
* @purpose  Returns Trap Mangaer's trap link state
*
* @param    void
*
* @returns  state   trap link state (L7_ENABLE or L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapLink(void);

/*********************************************************************
* @purpose  Sets Trap Mangaer's trap link state
*
* @param    state   trap link state (L7_ENABLE or L7_DISABLE)
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrSetTrapLink(L7_uint32 state);

/*********************************************************************
* @purpose  Returns Trap Mangaer's trap multiUsers state
*
* @param    void
*
* @returns  state   trap multiUsers state (L7_ENABLE or L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapMultiUsers(void);

/*********************************************************************
* @purpose  Sets Trap Mangaer's trap multiUsers state
*
* @param    state   trap multiUsers state (L7_ENABLE or L7_DISABLE)
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrSetTrapMultiUsers(L7_uint32 state);

/*********************************************************************
* @purpose  Sets Trap Manager's trap global wireless state
*
* @param    state   trap global wireless state (L7_ENABLE or L7_DISABLE)
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrSetTrapGlobalWirelessTree(L7_uint32 state);


/*********************************************************************
* @purpose  Returns Trap Mangaer's trap spanning tree state
*
* @param    void
*
* @returns  state   trap spanning tree state (L7_ENABLE or L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapSpanningTree(void);

/*********************************************************************
* @purpose  Returns Trap Manager's trap global wireless state
*
* @param    void
*
* @returns  state   trap global wireless state (L7_ENABLE or L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapGlobalWireless(void);

/*********************************************************************
* @purpose  Sets Trap Mangaer's trap spanning tree state
*
* @param    state   trap spanning tree state (L7_ENABLE or L7_DISABLE)
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrSetTrapMultiSpanningTree(L7_uint32 state);

/*********************************************************************
* @purpose  Returns Trap Manager's trap POE state
*
* @param    void
*
* @returns  state   trap POE state (L7_ENABLE or L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapPoe(void);

/*********************************************************************
* @purpose  Sets Trap Manager's POE state
*
* @param    state   trap POE state (L7_ENABLE or L7_DISABLE)
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrSetTrapPoe(L7_uint32 state);

/*********************************************************************
* @purpose  Returns Trap Manager's trap MAC Lock Violation state
*
* @param    void
*
* @returns  state   trap MAC Lock Violation state (L7_ENABLE or L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapMacLockViolation(void);

/*********************************************************************
* @purpose  Sets Trap Manager's MAC Lock Violation state
*
* @param    state   trap MAC Lock Violation state (L7_ENABLE or L7_DISABLE)
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrSetTrapMacLockViolation(L7_uint32 state);

/*********************************************************************
* @purpose  Returns Trap Mangaer's amount of undisplayed trap messages
*
* @param    void
*
* @returns  amount  Amount of undisplayed trap messages
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetAmountOfUnDisplayedTraps(void);

/*********************************************************************
* @purpose  Returns Trap Mangaer's total amount of traps since last reset
*
* @param    void
*
* @returns  amount  Total amount of traps since last reset
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTotalAmountOfTraps(void);

/*********************************************************************
* @purpose  Returns Trap Mangaer's complete trap log
*
* @param    user  type of user (TRAPMGR_USER_DISPLAY or TRAPMGR_USER_TFTP)
* @param    log   pointer to buf with size
*                (trapMgrTrapLogEntry_t * TRAPMGR_TABLE_SIZE)
*
* @returns  amount  Total number of traps returned in log
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapLog(L7_uint32 user, trapMgrTrapLogEntry_t trapLog[]);

/*********************************************************************
* @purpose  Returns Trap Manager's last received trap log entry index
*
* @returns  index of last received trap log entry

* @notes    none
*
* @end
*********************************************************************/
L7_uint32 trapMgrGetTrapLogEntryLastReceived(void);

/*********************************************************************
* @purpose  Returns Trap Manager's trap log entry
*
* @param    user  type of user (TRAPMGR_USER_DISPLAY or TRAPMGR_USER_TFTP)
* @param    log   pointer to trapMgrTrapLogEntry_t structure
*
* @returns  L7_SUCCESS if the entry exists
*           L7_FAILURE if the entry does not exist
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrGetTrapLogEntry(L7_uint32 user, L7_uint32 index, trapMgrTrapLogEntry_t *trapLogEntry);

/*********************************************************************
* @purpose  Returns Trap Mangaer's complete trap log
*
* @param    user  type of user (TRAPMGR_USER_DISPLAY or TRAPMGR_USER_TFTP)
* @param    log   pointer to buf with size
*                (trapMgrTrapLogEntry_t * TRAPMGR_TABLE_SIZE)
*
* @returns  amount  Total number of traps returned in log
*
* @notes    none
*
* @end
*********************************************************************/
void trapMgrClearTrapLog(void);

/*********************************************************************
*
* @purpose  Issue link down trap
*
* @param    intIfNum    L7_uint32 internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLinkDownLogTrap(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Issue link up trap
*
* @param    intIfNum    L7_uint32 internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLinkUpLogTrap(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Issue Link Failure trap
*
*
* @param    intIfNum    L7_uint32 internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLinkFailureLogTrap(L7_uint32 ifIndex);

/*********************************************************************
*
* @purpose  Issue coldstart trap
*
* @param    void
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrColdStartLogTrap();

/*********************************************************************
*
* @purpose  Issue warmstart trap
*
* @param    void
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrWarmStartLogTrap();

/*********************************************************************
*
* @purpose  Issue Authen Failure trap
*
* @param    intIfNum    L7_uint32 internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrAuthenFailureLogTrap(L7_uint32 ipAddress);

/*********************************************************************
*
* @purpose  Issue Multiple users trap
*
* @param    intIfNum    L7_uint32 internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrMultipleUsersLogTrap(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Problem issuing a request for a VLAN
*
* @param    vlanID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    for some reason, sendVlanCfgRequest() failed
*
* @end
*********************************************************************/
L7_RC_t trapMgrVlanRequestFailureLogTrap(L7_uint32 vlanID);

/*********************************************************************
*
* @purpose  Deleting either the last remaining or the default VLAN
*
* @param    vlanID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLastVlanDeleteLastLogTrap(L7_uint32 vlanID);

/*********************************************************************
*
* @purpose  Default VLAN configuration failed
*
* @param    vlanID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    vlanID always 1 since its the default
*
* @end
*********************************************************************/
L7_RC_t trapMgrDefaultVlanCfgFailureLogTrap(L7_uint32 vlanID);

/*********************************************************************
*
* @purpose  Problem restoring prior VLAN configuration
*
* @param    vlanID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    vlanPriorEntryRestore() failed
*
* @end
*********************************************************************/
L7_RC_t trapMgrVlanRestoreFailureLogTrap(L7_uint32 vlanID);


/*********************************************************************
*
* @purpose  Issue a new root trap
*
* @param
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrNewRootTrap();


/*********************************************************************
*
* @purpose  Issue a topology trap change
*
* @param
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrTopologyChange();

/*********************************************************************
*
* @purpose  Announce a Fan Failure
*
* @param    pStr L7_char8 ptr to string describing fan status
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrFanFailureLogTrap(L7_char8 *pStr);

/*********************************************************************
*
* @purpose  Announce a MAC Lock violation trap
*
* @param    L7_enetMacAddr_t macAddr
* @param    L7_ushort16      vlanId
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrMacLockViolationLogTrap(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose  Issue a new root trap for an STP instance
*
* @param    instanceId    STP instance id
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStpInstanceNewRootTrap(L7_uint32 instanceId);

/*********************************************************************
*
* @purpose  Issue a LoopInconsistentStart trap
*
* @param    instanceId    MSTP instance id
* @param    intIfNum       ifIndex of link which go into inconsistent state.
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStpInstanceLoopInconsistentStartTrap(L7_uint32 instanceId, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Issue a LoopInconsistentEnd trap
*
* @param    instanceId    MSTP instance id
* @param    intIfNum       ifIndex of link which return from inconsistent state.
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStpInstanceLoopInconsistentEndTrap(L7_uint32 instanceId, L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Issue a topology trap change for an STP instance
*
* @param    instanceId    STP instance id
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStpInstanceTopologyChange(L7_uint32 instanceId);


/*********************************************************************
*
* @purpose  Issue an RMON Rising Alarm trap
*
* @param    alarmIdex   L7_uint32
*
* @returns  L7_SUCCESS, if success
*           L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrRisingAlarmLogTrap(L7_uint32 alarmIndex);


/*********************************************************************
*
* @purpose  Issue an RMON Falling Alarm trap
*
* @param    alarmIndex      L7_uint32
*
* @returns  L7_SUCCESS, if success
*           L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrFallingAlarmLogTrap(L7_uint32 alarmIndex);

/*********************************************************************
*
* @purpose  Announce a failed user login attempt
*
* @param    pStr L7_char8 ptr to string describing login status
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrFailedUserLoginTrap(L7_char8 *pStr);

/*********************************************************************
*
* @purpose  Issue DHCP Snooping interface error disabled trap
*
* @param    intIfNum    L7_uint32 internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDsIntfErrorDisabledTrap(L7_uint32 intIfNum);


/*********************************************************************
*
* @purpose  Announce a locked user
*
* @param    pStr L7_char8 ptr to string describing login status
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLockedUserLoginTrap(L7_char8 *pStr);

/*********************************************************************
*
* @purpose  Issue dai interface error disabled trap
*
* @param    intIfNum    L7_uint32 internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDaiIntfErrorDisabledTrap(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Announce an 802.1AB remote table change
*
* @param    L7_uint32 inserts @b((input)) remote table inserts
* @param    L7_uint32 deletes @b((input)) remote table deletes
* @param    L7_uint32 drops   @b((input)) remote table drops
* @param    L7_uint32 ageouts @b((input)) remote table ageouts
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLldpRemTablesChangeTrap(L7_uint32 inserts,
                                       L7_uint32 deletes,
                                       L7_uint32 drops,
                                       L7_uint32 ageouts);

/*********************************************************************
*
* @purpose  Announce an 802.1AB-MED remote table topology change
*
* @param    L7_uint32 	remChassisIdSubtype @b((input)) subtype
* @param    L7_uchar8* 	remChassisId    	@b((input)) chassis id string
* @param	L7_uint32	length  			@b((input)) chassis id string length
* @param    L7_uint32 	remDeviceClass 		@b((input)) device Class
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLldpXMedTopologyChangeDetectedTrap(L7_uint32 remChassisIdSubtype,
												  L7_uchar8* remChassisId,
												  L7_uint32	length,
												  L7_uint32 remDeviceClass);
/*********************************************************************
*
* @purpose  This function process the alarm raise trap.
*
* @param    alarmIndex   Alarm Id.
*
* @returns  L7_SUCCESS, if success
*           L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrAlarmRaise(L7_uint32 alarmIndex);

/*********************************************************************
*
* @purpose  This function process the alarm clear trap.
*
* @param    alarmIndex   Alarm Id.
*
* @returns  L7_SUCCESS, if success
*           L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrAlarmClear(L7_uint32 alarmIndex);

/*********************************************************************
*
* @purpose  Announce an 802.1X unauthorized host access attempt
*
* @param    L7_uint32        intIfNum
* @param    L7_enetMacAddr_t macAddr
* @param    L7_ushort16      vlanId
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDot1xUnauthorizedHostLogTrap(L7_uint32        intIfNum,
                                            L7_enetMacAddr_t macAddr,
                                            L7_ushort16      vlanId);

/*********************************************************************
*
* @purpose  Announce an IP ACL Deny Rule match.
*
* @param    L7_uint32 index @b((input)) IP ACL index
* @param    L7_uint32 rule @b((input)) rule number
* @param    L7_ulong64 count @b((input)) Number of matches to rule
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrIpAclRuleDenyTrap(L7_uint32 index,
                                 L7_uint32 rule,
                                 L7_ulong64 count);

/*********************************************************************
*
* @purpose  Announce an MAC ACL Deny Rule match.
*
* @param    L7_uchar8 name @b((input)) MAC ACL name
* @param    L7_uint32 rule @b((input)) rule number
* @param    L7_ulong64 count @b((input)) number of matches to rule
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrMacAclRuleDenyTrap(L7_uchar8 *name,
                                  L7_uint32 rule,
                                  L7_ulong64 count);

/*********************************************************************
*
* @purpose  Issue Config Chaged trap
*
* @param    trapSource L7_char8 ptr to string containing the source of the
*                      change such as the unit, slot, port, VLAN, LAG, etc
* @param    trapInfo   L7_char8 ptr to string containing the additional
*                      information regarding the change
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrConfigChangedLogTrap(L7_char8 *trapSource, L7_char8 *trapInfo);

/*********************************************************************
*
* @purpose  Issue TFTP End trap
*
* @param    exitCode L7_int32 exitcode of the TFTP
* @param    fName L7_char8 ptr to string containing the name of the file
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrTFTPEndLogTrap(L7_int32 exitCode, L7_char8 *fName);

/*********************************************************************
*
* @purpose  Issue TFTP Abort trap
*
* @param    exitCode L7_int32 exitcode of the TFTP
* @param    fName L7_char8 ptr to string containing the name of the file
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrTFTPAbortLogTrap(L7_int32 exitCode, L7_char8 *fName);

/*********************************************************************
*
* @purpose  Issue TFTP Start trap
*
* @param    fName L7_char8 ptr to string containing the name of the file
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrTFTPStartLogTrap(L7_char8 *fName);

/*********************************************************************
*
* @purpose  Issue VlanDynPortAdded trap
*
* @param    dot1qVlanIndex L7_int32 VLan index
* @param    port L7_int32 Port number that was added dynamically to the VLan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrVlanDynPortAddedLogTrap(L7_int32 dot1qVlanIndex, L7_int32 port);

/*********************************************************************
*
* @purpose  Issue VlanDynPortRemoved trap
*
* @param    dot1qVlanIndex L7_int32 VLan index
* @param    port L7_int32 Port number that was removed dynamically from the VLan*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrVlanDynPortRemovedLogTrap(L7_int32 dot1qVlanIndex,
                                         L7_int32 port);

/*********************************************************************
*
* @purpose  Issue StackMasterFailed trap
*
* @param    oldUnitNumber L7_int32 Old Stack master unit number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackMasterFailedLogTrap(L7_int32 oldUnitNumber);

/*********************************************************************
*
* @purpose  Issue StackNewMasterElected trap
*
* @param    oldUnitNumber L7_int32 Old Stack master unit number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackNewMasterElectedLogTrap(L7_int32 oldUnitNumber);

/*********************************************************************
*
* @purpose  Issue StackMemberUnitFailed trap
*
* @param    failedUnitNumber L7_int32 Failed member unit number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackMemberUnitFailedLogTrap(L7_int32 failedUnitNumber);

/*********************************************************************
*
* @purpose  Issue StackNewMemberUnitAdded trap
*
* @param    newUnitNumber L7_int32 New member unit number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackNewMemberUnitAddedLogTrap(L7_int32 newUnitNumber);

/*********************************************************************
*
* @purpose  Issue StackMemberUnitRemoved trap
*
* @param    removedUnitNumber L7_int32 Removed member unit number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackMemberUnitRemovedLogTrap(L7_int32 removedUnitNumber);

/*********************************************************************
*
* @purpose  Issue StackSplitMasterReport trap
*
* @param    trapInfo L7_uchar8 ptr to a string that contains the list of unit
*                    numbers of the units that are split from the main stack
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackSplitMasterReportLogTrap(L7_uchar8 *trapInfo);

/*********************************************************************
*
* @purpose  Issue StackSplitNewMasterReport trap
*
* @param    oldMasterUnitNumber L7_int32 Old Stack master unit number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackSplitNewMasterReportLogTrap(L7_int32 oldMasterUnitNumber);

/*********************************************************************
*
* @purpose  Issue StackRejoined trap
*
* @param    None
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackRejoinedLogTrap();

/*********************************************************************
*
* @purpose  Issue StackLinkFailed trap
*
* @param    identifiedUnit L7_int32 The unit that identified the link failure
* @param    info L7_uchar8 ptr to a string that contains the additional
*                information about the stack state after the failure
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackLinkFailedLogTrap(L7_int32 identifiedUnit, L7_uchar8 *info);

/*********************************************************************
*
* @purpose  Issue Dot1dStpPortStateForwarding trap
*
* @param    port L7_int32 Port number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDot1dStpPortStateForwardingLogTrap(L7_int32 port, L7_uint32 instanceIdx);

/*********************************************************************
*
* @purpose  Issue Dot1dStpPortStateNotForwarding trap
*
* @param    port L7_int32 Port number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDot1dStpPortStateNotForwardingLogTrap(L7_int32 port, L7_uint32 instanceIdx);

/*********************************************************************
*
* @purpose  Issue TrunkPortAdded trap
*
* @param    trunkIfIndex If Index of the trunk
* @param    port L7_int32 Port number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrTrunkPortAddedLogTrap(L7_int32 trunkIfIndex, L7_int32 port);

/*********************************************************************
*
* @purpose  Issue TrunkPortRemoved trap
*
* @param    trunkIfIndex If Index of the trunk
* @param    port L7_int32 Port number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrTrunkPortRemovedLogTrap(L7_int32 trunkIfIndex, L7_int32 port);

/*********************************************************************
*
* @purpose  Issue LockPort trap
*
* @param    port L7_int32 Port number
* @param    macAddr L7_enetMacAddr_t New MAC address received
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLockPortLogTrap(L7_int32 port, L7_enetMacAddr_t macAddr);

/*********************************************************************
*
* @purpose  Issue VlanDynVlanAdded trap
*
* @param    dot1qVlanIndex L7_int32 Index of the VLan being added
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrVlanDynVlanAddedLogTrap(L7_int32 dot1qVlanIndex);

/*********************************************************************
*
* @purpose  Issue VlanDynVlanRemoved trap
*
* @param    dot1qVlanIndex L7_int32 Index of the VLan being removed
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrVlanDynVlanRemovedLogTrap(L7_int32 dot1qVlanIndex);

/*********************************************************************
*
* @purpose  Issue EnvMonFanStateChange trap
*
* @param    fanIndex L7_int32 Fan Index
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrEnvMonFanStateChangeLogTrap(L7_uint32 unit, L7_int32 fanIndex, L7_BOOL status);

/*********************************************************************
*
* @purpose  Issue EnvMonPowerSupplyStateChange trap
*
* @param    envMonSupplyIndex L7_int32 Power supply Index
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrEnvMonPowerSupplyStateChangeLogTrap(L7_uint32 trap_unit, L7_int32 envMonSupplyIndex, L7_BOOL status);

/*********************************************************************
*
* @purpose  Issue EnvMonTemperatureRisingAlarm trap
*
* @param    None
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrEnvMonTemperatureRisingAlarmLogTrap(L7_uint32 unit);

/*********************************************************************
*
* @purpose  Issue CopyFinished trap
*
* @param    None
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrCopyFinishedLogTrap();

/*********************************************************************
*
* @purpose  Issue CopyFailed trap
*
* @param    None
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrCopyFailedLogTrap();

/*********************************************************************
*
* @purpose  Issue Dot1xPortStatusAuthorized trap
*
* @param    port L7_int32 Port ifIndex
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDot1xPortStatusAuthorizedLogTrap(L7_int32 port);

/*********************************************************************
*
* @purpose  Issue Dot1xPortStatusUnauthorized trap
*
* @param    port L7_int32 Port ifIndex
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDot1xPortStatusUnauthorizedLogTrap(L7_int32 port);

/*********************************************************************
*
* @purpose  Issue StpElectedAsRoot trap
*
* @param    None
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStpElectedAsRootLogTrap();

/*********************************************************************
*
* @purpose  Issue StpNewRootElected trap
*
* @param    instanceID L7_int32 Instance ID
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStpNewRootElectedLogTrap(L7_int32 instanceID, L7_uchar8 *rootId);

/*********************************************************************
*
* @purpose  Issue InvalidUserLoginAttempted trap
*
* @param    uiMode L7_uchar8 ptr to string with value of either "Web" or "CLI"
*                  the mode used to logged in
* @param    fromIpAddress L7_uchar8 ptr to string containing the IP address
*                  from where the attempt was made
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrInvalidUserLoginAttemptedLogTrap(L7_uchar8 *uiMode,
                                                L7_uchar8 *fromIpAddress);

/*********************************************************************
*
* @purpose  Issue ManagementACLViolation trap
*
* @param    uiMode L7_uchar8 ptr to string with value of "SNMP" or "Web" or
*                  "CLI" the mode used to logged in
* @param    fromIpAddress L7_uchar8 ptr to string containing the IP address
*                  from where the attempt was made
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrManagementACLViolationLogTrap(L7_uchar8 *uiMode,
                                             L7_uchar8 *fromIpAddress);
/*********************************************************************
*
* @purpose Issue a SFP insertion/removal trap.
*
* @param L7_int32 unitNumber @b((input)) Unit where the SFP was inserted/removed
*
* @param L7_uint32 intIfNum   @b((input))  The internal interface number
*
* @param L7_BOOL status   @b((input))  Indicates whether the SFP was inserted
*                                      or removed
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t trapMgrSFPInsertionRemovalTrap(L7_int32 unitNumber,
                                       L7_int32 intIfNum,
                                       L7_BOOL status);

/*********************************************************************
*
* @purpose  Announce an 802.1AB-MED remote table topology change
*
* @param    L7_uint32   remChassisIdSubtype @b((input)) subtype
* @param    L7_uchar8*  remChassisId        @b((input)) chassis id string
* @param    L7_uint32   length              @b((input)) chassis id string length
* @param    L7_uint32   remDeviceClass      @b((input)) device Class
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLldpXMedTopologyChangeDetectedTrap(L7_uint32 remChassisIdSubtype,
                                                  L7_uchar8 *remChassisId,
                                                  L7_uint32 length,
                                                  L7_uint32 remDeviceClass);

/*********************************************************************
*
* @purpose Issue a XFP insertion/removal trap.
*
* @param L7_int32 unitNumber @b((input)) Unit where the XFP was inserted/removed
*
* @param L7_uint32 intIfNum   @b((input))  The internal interface number
*
* @param L7_BOOL status   @b((input))  Indicates whether the XFP was inserted
*                                      or removed
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if failure
*
* @notes None
*
* @end
*
*********************************************************************/
L7_RC_t trapMgrXFPInsertionRemovalTrap(L7_int32 unitNumber, L7_int32 intIfNum, L7_BOOL status);


/*********************************************************************
*
*
* @purpose  Announce a Poe Port power change
*
* @param    value L7_uint32 status, up or down
* @param    detection L7_uint32 status of PD detection for the PSE port
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrPoePortChangeLogTrap( L7_uint32 intIfNum, L7_uint32 value ,
                                     L7_uint32 detection);


/*********************************************************************
*
* @purpose  Announce a Poe System Power Threshold Crossing
*
* @param    value L7_uint32 status, up or down
* @param    allocPower L7_uint32 Power delivering by the PSE
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrPoeThresholdCrossingTrap(L7_uint32 Unit, L7_uint32 value,
                                        L7_uint32 allocPower );

/*********************************************************************
*
* @purpose  This function process the temperature event trap
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    itemNum  @b((input)) temperature sensor index
* @param    tempEventType @b((input)) type of temperature event
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t trapMgrTemperatureChange(L7_uint32 UnitIndex, L7_uint32 itemNum, L7_uint32 tempEventType);


/*********************************************************************
*
* @purpose  This function process the fan state change event trap
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    itemNum  @b((input)) fan index
* @param    fanEventType @b((input)) type of fan event
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t trapMgrFanStateChange(L7_uint32 UnitIndex, L7_uint32 itemNum, L7_uint32 fanEventType);


/*********************************************************************
*
* @purpose  This function process the power supply state change event trap
*
* @param    UnitIndex           L7_uint32 the unit for this operation
* @param    itemNum  @b((input)) power supply  index
* @param    powSupplyEventType @b((input)) type of power supply event
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t trapMgrPowSupplyStateChange(L7_uint32 UnitIndex, L7_uint32 itemNum, L7_uint32 powSupplyEventType);

#ifdef FEAT_METRO_CPE_V1_0
/*********************************************************************
*
* @purpose	Issue No Startup Config trap
*
* @param	none
*
* @returns	L7_SUCCESS, if success
* @returns	L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrNoStartupConfigLogTrap(void);
#endif

/*********************************************************************
*
* @purpose  Issue EntConfigChange trap
*
* @param    None
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrEntConfigChangeLogTrap(void);


#ifdef L7_DOT1AG_PACKAGE
/*********************************************************************
*
* @purpose  Issue Dot1ag CFM Defect Notification Trap
*
* @param    mdIndex   @b{(input)} MD whose FNGSM is reporting a defect
* @param    maIndex   @b{(input)} MA whose FNGSM is reporting a defect
* @param    mepId     @b{(input)} MEP whose FNGSM is reporting a defect
* @param    fngDefect   @b{(input)} Defect to be reported
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDot1agCfmFaultNotification(L7_uint32 mdIndex, L7_uint32 maIndex, 
                                          L7_uint32 mepId, L7_uint32 highestPriDefect);
#endif /* L7_DOT1AG_PACKAGE */
#endif /* INCLUDE_TRAPAPI_H */


