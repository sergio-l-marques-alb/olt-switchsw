/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename traputil.c
*
* @purpose Trap Manager Utilities File
*
* @component trapmgr
*
* @comments none
*
* @create 08/30/2000
*
* @author bmutz
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"
#include "trap.h"
#include "trapapi.h"
#include "trapmgr_exports.h"
#include "trapstr.h"
#include "usmdb_snmp_api.h"
#include "usmdb_snmp_trap_api.h"
#include "usmdb_snmp_trap_api_base.h"
#include "trap_cnfgr_api.h"
#include "dot1ag_exports.h"

extern trapMgrTrapData_t trapMgrTrapData;
extern trapMgrCfgData_t trapMgrCfgData;

extern L7_uint32 warmStartSetTrapNeeded;
extern L7_uint32 coldStartSetTrapNeeded;

extern void *trapMsgQueue;
extern L7_uint32 trapMsgQueueLostMsgs;

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
L7_RC_t trapMgrLinkDownLogTrap(L7_uint32 intIfNum)
{
  L7_TRAP_t trap;
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 portTrapState;

  if (trapMgrCfgData.trapLink == L7_ENABLE &&
      nimGetIntfLinkTrap(intIfNum, &portTrapState) == L7_SUCCESS &&
      portTrapState == L7_ENABLE)
  {
    /* make sure you have message Queue created before sending and trap message */
    if (trapMgrCnfgrPhaseReady() == L7_FALSE)
    {
      LOG_PT_ERR(LOG_CTX_MISC,"intfIfNum=%u: Not ready to be processed",intIfNum);
      return L7_FAILURE;
    }

    trap.trapId = TRAP_LINK_DOWN;

    trap.gen = TRAPMGR_GENERIC_LINKDOWN;
    trap.spec = TRAPMGR_SPECIFIC_NONE;

    trap.u.linkStatus.intIfNum = intIfNum;

    rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc != L7_SUCCESS)
    {
      LOG_PT_ERR(LOG_CTX_MISC,"intfIfNum=%u: Error sending message",intIfNum);
      trapMsgQueueLostMsgs++;
    }
  }
  return rc;
}

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
L7_RC_t trapMgrLinkUpLogTrap(L7_uint32 intIfNum)
{
  L7_TRAP_t trap;
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 portTrapState;

  if (trapMgrCfgData.trapLink == L7_ENABLE &&
      nimGetIntfLinkTrap(intIfNum, &portTrapState) == L7_SUCCESS &&
      portTrapState == L7_ENABLE)
  {
    /* make sure you have message Queue created before sending and trap message */
    if (trapMgrCnfgrPhaseReady() == L7_FALSE)
    {
      return L7_FAILURE;
    }

    trap.trapId = TRAP_LINK_UP;

    trap.gen = TRAPMGR_GENERIC_LINKDOWN;
    trap.spec = TRAPMGR_SPECIFIC_NONE;

    trap.u.linkStatus.intIfNum = intIfNum;

    rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc != L7_SUCCESS)
    {
      trapMsgQueueLostMsgs++;
    }
  }
  return rc;
}

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
L7_RC_t trapMgrColdStartLogTrap()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), coldstart_str, 0);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_COLDSTART;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

  coldStartSetTrapNeeded = L7_TRUE;

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    rc = usmDbSnmpColdStartTrapSend(unit);

  return rc;
}

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
L7_RC_t trapMgrWarmStartLogTrap()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), warmstart_str, 0);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_WARMSTART;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

  warmStartSetTrapNeeded = L7_TRUE;

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    rc = usmDbSnmpWarmStartTrapSend(unit);
  return rc;
}

/*********************************************************************
*
* @purpose  Issue Authen Failure trap
*
* @param    void
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrAuthenFailureLogTrap(L7_uint32 ipAddress)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 ipAddressBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (trapMgrGetTrapAuth() == L7_ENABLE)
  {
    usmDbInetNtoa(ipAddress, ipAddressBuf);
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), authfail_str, TRAPMGR_UNIT_INDEX, ipAddressBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_AUTFAILURE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpAuthenFailureTrapSend(unit);
  }
  return rc;
}

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
L7_RC_t trapMgrMultipleUsersLogTrap(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (trapMgrCfgData.trapMultiUsers == L7_ENABLE)
  {
    rc = nimGetUnitSlotPort(intIfNum, &usp);
    if (rc == L7_SUCCESS)
    {
      osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), multiuser_str, usp.unit, usp.slot, usp.port);
      trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
      trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_MULTIPLEUSERS;
      trapMgrLogTrapToLocalLog(trapStringBuf);

      if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
        rc = usmDbSnmpMultipleUsersTrapSend(unit);
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Issue a topology trap change
*
* @param
*
* @returns  none
*
* @notes Will only log trap if Spanning Tree Traps are enabled.
*
* @end
*********************************************************************/
L7_RC_t trapMgrReceivedTopologyChange(L7_uint32 instanceId, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;
  L7_int32 u;
  L7_int32 s;
  L7_int32 p;

  usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);

  usmDbUnitMgrNumberGet(&unit);

/* @p1501 start */
  if (trapMgrCfgData.trapSpanningTree == L7_ENABLE)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), topochange_rx_str, 
                  instanceId, u, s, p);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_TOPOLOGYCHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc =  usmDbSnmpTopologyChangeTrapSend(unit);
  }

  return rc;
}

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
L7_RC_t trapMgrStpInstanceTopologyChange(L7_uint32 instanceId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  usmDbUnitMgrNumberGet(&unit);

  if (trapMgrCfgData.trapSpanningTree == L7_ENABLE)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), topochange_str, instanceId, TRAPMGR_UNIT_INDEX);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_TOPOLOGYCHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc =  usmDbSnmpStpInstanceTopologyChangeTrapSend(unit, instanceId);

  }
/* @p1501 end */

  return rc;
}

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
L7_RC_t trapMgrRisingAlarmLogTrap(L7_uint32 alarmIndex)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  usmDbUnitMgrNumberGet(&unit);

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), rmon_rising_alarm_str, TRAPMGR_UNIT_INDEX, alarmIndex);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
  /* trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_RMONRISINGALARM; */

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    rc =  usmDbSnmpRisingAlarmTrapSend(unit, alarmIndex);

  return rc;
}

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
L7_RC_t trapMgrFallingAlarmLogTrap(L7_uint32 alarmIndex)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  usmDbUnitMgrNumberGet(&unit);

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), rmon_falling_alarm_str, TRAPMGR_UNIT_INDEX, alarmIndex);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
  /* trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_RMONRISINGALARM; */

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    rc =  usmDbSnmpFallingAlarmTrapSend(unit, alarmIndex);

  return rc;
}

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
                                                  L7_uchar8* remChassisId,
                                                  L7_uint32 length,
                                                  L7_uint32 remDeviceClass)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), lldp_med_topo_change_str, remChassisIdSubtype, remChassisId, remDeviceClass);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_LLDP_MED_TOPO_CHANGE;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
  {
    rc = usmDbSnmpLldpXMedTopologyChangeDetectedTrapSend(remChassisIdSubtype,
                                                         remChassisId,
                                                         length,
                                                         remDeviceClass);
  }

  return rc;
}

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
                                            L7_ushort16      vlanId)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_char8  trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t  usp;
  L7_uint32 unit;

  usmDbUnitMgrNumberGet(&unit);

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return(L7_FAILURE);
  }

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf),
          dot1x_unauth_host_str,
          usp.unit,
          usp.slot,
          usp.port,
          macAddr.addr[0],
          macAddr.addr[1],
          macAddr.addr[2],
          macAddr.addr[3],
          macAddr.addr[4],
          macAddr.addr[5],
          vlanId);

  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_DOT1X_UNATH_HOST;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;
  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (L7_ENABLE == usmDbSnmpStatusGet(unit))
  {
    /* call into SNMP */
/* TBD - dflint
   usmDbSnmpDot1xUnauthorizedHostTrapSend(intIfNum);
*/
  }

  return(rc);
}

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
                                 L7_ulong64 count)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;
  L7_char8 buf[64];

  usmDbUnitMgrNumberGet(&unit);

  memset(buf, L7_NULL, sizeof(buf));
  (void)usmDb64BitsToString(count, buf);

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), acl_ip_rule_deny_str, index, rule, buf);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_IP_ACL_DENY_RULE_MATCH;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
  {
#if 0
    rc = usmDbSnmpIpAclRuleDenyTrapNotificationSend(unit, index, rule, count);
#endif
  }

  return rc;
}

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
                                  L7_ulong64 count)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;
  L7_char8 buf[64];

  usmDbUnitMgrNumberGet(&unit);

  memset(buf, L7_NULL, sizeof(buf));
  (void)usmDb64BitsToString(count, buf);

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), acl_mac_rule_deny_str, name, rule, buf);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_MAC_ACL_DENY_RULE_MATCH;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
  {
#if 0
    rc = usmDbSnmpMacAclRuleDenyTrapNotificationSend(unit, name, rule, count);
#endif
  }

  return rc;
}


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
L7_RC_t trapMgrConfigChangedLogTrap(L7_char8 *trapSource, L7_char8 *trapInfo)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_CONFIG_CHANGED;
  osapiStrncpySafe(trap.u.configChanged.trapSource,
                   trapSource,
                   sizeof(trap.u.configChanged.trapSource));
  osapiStrncpySafe(trap.u.configChanged.trapInfo,
                   trapInfo,
                   sizeof(trap.u.configChanged.trapInfo));

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_CONFIG_CHANGED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrTFTPEndLogTrap(L7_int32 exitCode, L7_char8 *fName)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_END_TFTP;
  trap.u.endTftp.exitCode = exitCode;
  osapiStrncpySafe(trap.u.endTftp.fName, fName, sizeof(trap.u.endTftp.fName));

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_TFTP_END;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrTFTPAbortLogTrap(L7_int32 exitCode, L7_char8 *fName)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_ABORT_TFTP;
  trap.u.abortTftp.exitCode = exitCode;
  osapiStrncpySafe(trap.u.abortTftp.fName, fName, sizeof(trap.u.abortTftp.fName));

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_TFTP_ABORT;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrTFTPStartLogTrap(L7_char8 *fName)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_START_TFTP;
  osapiStrncpySafe(trap.u.startTftp.fName, fName, sizeof(trap.u.startTftp.fName));

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_TFTP_START;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Issue Link Failure trap
*
* @param    ifIndex L7_int32 The if index of the failed link
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLinkFailureLogTrap(L7_uint32 ifIndex)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  if (trapMgrCfgData.trapLink == L7_ENABLE)
  {
    /* make sure you have message Queue created before sending and trap message */
    if (trapMgrCnfgrPhaseReady() == L7_FALSE)
    {
      return L7_FAILURE;
    }
  
    trap.trapId = TRAP_LINK_FAILURE;
    trap.u.linkStatus.intIfNum = ifIndex;
  
    trap.gen = TRAPMGR_GENERIC_NONE;
    trap.spec = TRAPMGR_SPECIFIC_LINK_FAILURE;
  
    rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc != L7_SUCCESS)
    {
      trapMsgQueueLostMsgs++;
    }
  }

  return rc;
}


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
L7_RC_t trapMgrVlanDynPortAddedLogTrap(L7_int32 dot1qVlanIndex, L7_int32 port)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_VLAN_DYN_PORT_ADDED;
  trap.u.vlanDynPortAdded.dot1qVlanIndex = dot1qVlanIndex;
  trap.u.vlanDynPortAdded.port = port;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_VLANDYNPORTADDED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Issue VlanDynPortRemoved trap
*
* @param    dot1qVlanIndex L7_int32 VLan index
* @param    port L7_int32 Port number that was removed dynamically from the VLan
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrVlanDynPortRemovedLogTrap(L7_int32 dot1qVlanIndex, L7_int32 port)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_VLAN_DYN_PORT_REMOVED;
  trap.u.vlanDynPortRemoved.dot1qVlanIndex = dot1qVlanIndex;
  trap.u.vlanDynPortRemoved.port = port;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_VLANDYNPORTREMOVED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrStackMasterFailedLogTrap(L7_int32 oldUnitNumber)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STACK_MASTER_FAILED;
  trap.u.stackMasterFailed.oldUnitNumber = oldUnitNumber;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STACKMASTERFAILED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrStackNewMasterElectedLogTrap(L7_int32 oldUnitNumber)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STACK_NEW_MASTER_ELECTED;
  trap.u.stackNewMasterElected.oldUnitNumber = oldUnitNumber;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STACKNEWMASTERELECTED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrStackMemberUnitFailedLogTrap(L7_int32 failedUnitNumber)
{
  return L7_FAILURE;
}

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
L7_RC_t trapMgrStackNewMemberUnitAddedLogTrap(L7_int32 newUnitNumber)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STACK_NEW_MEMBER_UNIT_ADDED;
  trap.u.stackNewMemberUnitAdded.newUnitNumber = newUnitNumber;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STACKMEMBERUNITADDED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrStackSplitMasterReportLogTrap(L7_uchar8 *trapInfo)
{
  /* unsupported */
  return L7_NOT_SUPPORTED;
}

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
L7_RC_t trapMgrStackMemberUnitRemovedLogTrap(L7_int32 removedUnitNumber)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STACK_MEMBER_UNIT_REMOVED;
  trap.u.stackMemberUnitRemoved.removedUnitNumber = removedUnitNumber;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STACKMEMBERUNITREMOVED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrStackSplitNewMasterReportLogTrap(L7_int32 oldMasterUnitNumber)
{
  /* unsupported */
  return L7_NOT_SUPPORTED;
}

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
L7_RC_t trapMgrStackRejoinedLogTrap()
{
  /* unsupported */
  return L7_NOT_SUPPORTED;
}

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
L7_RC_t trapMgrStackLinkFailedLogTrap(L7_int32 identifiedUnit, L7_uchar8 *info)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STACK_LINK_FAILED;
  trap.u.stackLinkFailed.identifiedUnit = identifiedUnit;
  osapiStrncpySafe(trap.u.stackLinkFailed.info, info, sizeof(trap.u.stackLinkFailed.info));

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STACKLINKFAILED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Issue StackRestartComplete trap
*
* @param    unitId L7_int32 The unit that completed the restart
* @param    reason L7_LAST_STARTUP_REASON_t reason  The restart reason
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStackRestartCompleteLogTrap(L7_int32 unitId, 
                                           L7_LAST_STARTUP_REASON_t reason)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STACK_RESTART_COMPLETE;
  trap.u.stackRestartComplete.unitId = unitId;
  trap.u.stackRestartComplete.reason = reason;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STACKRESTARTCOMPLETE;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrDot1dStpPortStateForwardingLogTrap(L7_int32 port, L7_uint32 instanceIdx)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  if (trapMgrCfgData.trapSpanningTree == L7_ENABLE)
  {
    /* make sure you have message Queue created before sending and trap message */
    if (trapMgrCnfgrPhaseReady() == L7_FALSE)
    {
      return L7_FAILURE;
    }
  
    trap.trapId = TRAP_DOT1D_STP_PORTSTATE_FWD;
    trap.u.dot1dStpPortStateForwarding.port = port;
    trap.u.dot1dStpPortStateForwarding.instanceIdx = instanceIdx;
  
    trap.gen = TRAPMGR_GENERIC_NONE;
    trap.spec = TRAPMGR_SPECIFIC_DOT1DSTPPORTSTATEFORWARDING;
  
    rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc != L7_SUCCESS)
    {
      trapMsgQueueLostMsgs++;
    }
  }

  return rc;
}

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
L7_RC_t trapMgrDot1dStpPortStateNotForwardingLogTrap(L7_int32 port, L7_uint32 instanceIdx)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  if (trapMgrCfgData.trapSpanningTree == L7_ENABLE)
  {
    /* make sure you have message Queue created before sending and trap message */
    if (trapMgrCnfgrPhaseReady() == L7_FALSE)
    {
      return L7_FAILURE;
    }
  
    trap.trapId = TRAP_DOT1D_STP_PORTSTATE_NOT_FWD;
    trap.u.dot1dStpPortStateNotForwarding.port = port;
    trap.u.dot1dStpPortStateNotForwarding.instanceIdx = instanceIdx;
  
    trap.gen = TRAPMGR_GENERIC_NONE;
    trap.spec = TRAPMGR_SPECIFIC_DOT1DSTPPORTSTATENOTFORWARDING;
  
    rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
    if (rc != L7_SUCCESS)
    {
      trapMsgQueueLostMsgs++;
    }
  }

  return rc;
}

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
L7_RC_t trapMgrTrunkPortAddedLogTrap(L7_int32 trunkIfIndex, L7_int32 port)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_TRUNK_PORT_ADDED;
  trap.u.trunkPortAddedTrap.trunkIfIndex = trunkIfIndex;
  trap.u.trunkPortAddedTrap.port = port;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_TRUNKPORTADDED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrTrunkPortRemovedLogTrap(L7_int32 trunkIfIndex, L7_int32 port)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_TRUNK_PORT_REMOVED;
  trap.u.trunkPortRemovedTrap.trunkIfIndex = trunkIfIndex;
  trap.u.trunkPortRemovedTrap.port = port;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_TRUNKPORTREMOVED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrLockPortLogTrap(L7_int32 port, L7_enetMacAddr_t macAddr)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_LOCK_PORT;
  trap.u.lockPortTrap.port = port;
  trap.u.lockPortTrap.macAddr = macAddr;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_LOCKPORT;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrVlanDynVlanAddedLogTrap(L7_int32 dot1qVlanIndex)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_VLAN_DYN_VLAN_ADDED;
  trap.u.vlanDynVlanAdded.dot1qVlanIndex = dot1qVlanIndex;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_VLANDYNVLANADDED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrVlanDynVlanRemovedLogTrap(L7_int32 dot1qVlanIndex)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_VLAN_DYN_VLAN_REMOVED;
  trap.u.vlanDynVlanRemoved.dot1qVlanIndex = dot1qVlanIndex;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_VLANDYNVLANREMOVED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}


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
L7_RC_t trapMgrEnvMonFanStateChangeLogTrap(L7_uint32 trap_unit, L7_int32 fanIndex, L7_BOOL status)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_ENV_MON_FAN_STATE_CHANGE;
  trap.u.envMonFanStateChange.trap_unit = trap_unit;
  trap.u.envMonFanStateChange.fanIndex = fanIndex;
  trap.u.envMonFanStateChange.status = status;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_ENVMONFANSTATECHANGE;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrEnvMonPowerSupplyStateChangeLogTrap(L7_uint32 trap_unit, L7_int32 envMonSupplyIndex, L7_BOOL status)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_ENV_MON_POWER_SUPPLY_STATE_CHANGE;
  trap.u.envMonPowerSupplyStateChange.trap_unit = trap_unit;
  trap.u.envMonPowerSupplyStateChange.envMonSupplyIndex = envMonSupplyIndex;
  trap.u.envMonPowerSupplyStateChange.status = status;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_ENVMONPOWERSUPPLYSTATECHANGE;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrEnvMonTemperatureRisingAlarmLogTrap(L7_uint32 unit)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_ENV_MON_TEMP_RISING;
  trap.u.envMonTemperatureRisingAlarm.unit = unit;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_ENVMONTEMPERATURERISINGALARM;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrCopyFinishedLogTrap()
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_COPY_FINISHED;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_COPYFINISHED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrCopyFailedLogTrap()
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_COPY_FAILED;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_COPYFAILED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrDot1xPortStatusAuthorizedLogTrap(L7_int32 port)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_DOT1X_PORT_STATUS_AUTHORIZED;
  trap.u.dot1xPortStatusAuthorizedTrap.port = port;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_DOT1XPORTSTATUSAUTHORIZED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrDot1xPortStatusUnauthorizedLogTrap(L7_int32 port)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_DOT1X_PORT_STATUS_UNAUTHORIZED;
  trap.u.dot1xPortStatusUnauthorizedTrap.port = port;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_DOT1XPORTSTATUSUNAUTHORIZED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrStpElectedAsRootLogTrap()
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STP_ELECTED_AS_ROOT;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STPELECTEDASROOT;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrStpNewRootElectedLogTrap(L7_int32 instanceID, L7_uchar8 *rootId)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STP_NEW_ROOT_ELECTED;
  trap.u.stpNewRootElected.instanceID = instanceID;
  memcpy(trap.u.stpNewRootElected.rootId, rootId, 8);

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STPNEWROOTELECTED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
                                                L7_uchar8 *fromIpAddress)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_INVALID_USER_LOGIN_ATTEMPTED;
  osapiStrncpySafe(trap.u.invalidUserLoginAttempted.uiMode, uiMode, sizeof(trap.u.invalidUserLoginAttempted.uiMode));
  osapiStrncpySafe(trap.u.invalidUserLoginAttempted.fromIpAddress, fromIpAddress, sizeof(trap.u.invalidUserLoginAttempted.fromIpAddress));

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_INVALIDUSERLOGINATTEMPTED;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
                                             L7_uchar8 *fromIpAddress)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_MGMT_ACL_VIOLATION;
  osapiStrncpySafe(trap.u.managementACLViolation.uiMode, uiMode, sizeof(trap.u.managementACLViolation.uiMode));
  osapiStrncpySafe(trap.u.managementACLViolation.fromIpAddress, fromIpAddress, sizeof(trap.u.managementACLViolation.fromIpAddress));

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_MANAGEMENTACLVIOLATION;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
                                       L7_BOOL  status)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_SFP_INSERTION_REMOVAL;
  trap.u.sfpInsertionRemoval.Unit = unitNumber;
  trap.u.sfpInsertionRemoval.intIfNum = intIfNum;
  trap.u.sfpInsertionRemoval.status = status;

  trap.gen = TRAPMGR_GENERIC_NONE;
  if (status == L7_TRUE) {
    trap.spec = TRAPMGR_SPECIFIC_SFP_REMOVED;
  }
  else {
    trap.spec = TRAPMGR_SPECIFIC_SFP_INSERTED;
  }

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrXFPInsertionRemovalTrap(L7_int32 unitNumber,
                                       L7_int32 intIfNum,
                                       L7_BOOL status)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_XFP_INSERTION_REMOVAL;
  trap.u.xfpInsertionRemoval.Unit = unitNumber;
  trap.u.xfpInsertionRemoval.intIfNum = intIfNum;
  trap.u.xfpInsertionRemoval.status = status;

  trap.gen = TRAPMGR_GENERIC_NONE;
  if (status == L7_TRUE) {
    trap.spec = TRAPMGR_SPECIFIC_XFP_REMOVED;
  }
  else {
    trap.spec = TRAPMGR_SPECIFIC_XFP_INSERTED;
  }

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrStpInstanceNewRootTrap(L7_uint32 instanceId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (trapMgrCfgData.trapSpanningTree == L7_ENABLE)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), newroot_str, instanceId, TRAPMGR_UNIT_INDEX);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NEWROOT;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpStpInstanceNewRootTrapSend(unit, instanceId);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Issue a LoopInconsistentStart trap
*
* @param    instanceId    MSTP instance id
* @param    intIfNum      intIfNum of link which go into inconsistent state.
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStpInstanceLoopInconsistentStartTrap(L7_uint32 instanceId, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;
  nimUSP_t usp;

  if (trapMgrCfgData.trapSpanningTree == L7_ENABLE)
  {
    unit = usmDbThisUnitGet();
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), loop_inconsistent_start_str, instanceId, usp.unit, usp.slot, usp.port);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_LOOP_INCONSISTENT_START;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpStpInstanceLoopInconsistentStartTrapSend(unit, instanceId, intIfNum);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Issue a LoopInconsistentEnd trap
*
* @param    instanceId    MSTP instance id
* @param    intIfNum      intIfNum of link which return from inconsistent state.
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrStpInstanceLoopInconsistentEndTrap(L7_uint32 instanceId, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;
  nimUSP_t usp;

  if (trapMgrCfgData.trapSpanningTree == L7_ENABLE)
  {
    unit = usmDbThisUnitGet();
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), loop_inconsistent_end_str, instanceId, usp.unit, usp.slot, usp.port);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_LOOP_INCONSISTENT_END;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpStpInstanceLoopInconsistentEndTrapSend(unit, instanceId, intIfNum);
  }

  return rc;
}

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
L7_RC_t trapMgrFailedUserLoginTrap(L7_char8 *pStr)
{
        L7_RC_t rc = L7_SUCCESS;
        L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
        L7_uint32 unit;

        unit = usmDbThisUnitGet();

        osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), failed_login_str, TRAPMGR_UNIT_INDEX, pStr);
        trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_FANSTATUS;
        trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

        trapMgrLogTrapToLocalLog(trapStringBuf);

        if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
          rc = usmDbSnmpFailedUserLoginTrapSend();

    return rc;
}

/*********************************************************************
*
* @purpose  Announce a locked local user login
*
* @param    pStr L7_char8 ptr to string describing login status
*
* @returns  none
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrLockedUserLoginTrap(L7_char8 *pStr)
{
        L7_RC_t rc = L7_SUCCESS;
        L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
        L7_uint32 unit;

        unit = usmDbThisUnitGet();

        osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), user_login_lockout_str, TRAPMGR_UNIT_INDEX, pStr);
        trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_FANSTATUS;
        trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

        trapMgrLogTrapToLocalLog(trapStringBuf);

        if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
          rc = usmDbSnmpLockedUserLoginTrapSend();

    return rc;
}

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
L7_RC_t trapMgrDaiIntfErrorDisabledTrap(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  rc = nimGetUnitSlotPort(intIfNum, &usp);
  if (rc == L7_SUCCESS)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), dai_intf_error_disabled_str, usp.unit, usp.slot, usp.port);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_LINKDOWN;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_DAI_ERROR_DISABLE;
    trapMgrLogTrapToLocalLog(trapStringBuf);
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpDaiIntfErrorDisabledTrapSend(intIfNum);
  }

  return rc;
}

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
L7_RC_t trapMgrDsIntfErrorDisabledTrap(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  rc = nimGetUnitSlotPort(intIfNum, &usp);
  if (rc == L7_SUCCESS)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ds_intf_error_disabled_str, usp.unit, usp.slot, usp.port);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_LINKDOWN;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_DHCP_SNOOPING_ERROR_DISABLE;
    trapMgrLogTrapToLocalLog(trapStringBuf);
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpDsIntfErrorDisabledTrapSend(intIfNum);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Issue IP address conflict detected trap
*
* @param    ipAddr    Conflicting IP address
* @param    macAddr   Conflicting host's MAC address
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrSwitchIpAddrConflictTrap(L7_uint32 ipAddr, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 ipAddressBuf[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  if(usmDbInetNtoa(ipAddr, ipAddressBuf) == L7_SUCCESS)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ip_address_conflict_str,
                  ipAddressBuf, macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_IP_ADDRESS_CONFLICT;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;
    trapMgrLogTrapToLocalLog(trapStringBuf);
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      rc = usmDbSnmpIpAddrConflictTrapSend(ipAddr, macAddr);
    }
  }

  return rc;
}

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
                                       L7_uint32 ageouts)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), lldp_rem_change_str, TRAPMGR_UNIT_INDEX, inserts, deletes, drops, ageouts);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_LLDP_REM_CHANGE;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
  {
    /* TBD - SNMP work needs to be completed...
    rc = usmDbSnmpLldpRemTablesChangeNotificationSend();
    */
  }

  return rc;
}

/*Functions "trapMgrAlarmRaise" and  "trapMgrAlarmClear"
 *are called only from alarm controller.
 */

#ifdef L7_CHASSIS
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
L7_RC_t trapMgrAlarmRaise(L7_uint32 alarmIndex)
{
   L7_RC_t rc = L7_SUCCESS;
   L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
   L7_uint32 unit;

   unit = usmDbThisUnitGet();

   osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), chassis_rising_alarm_str, alarmIndex);
   trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
   trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

   trapMgrLogTrapToLocalLog(trapStringBuf);

   if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
     rc = usmDbSnmpChassisRisingAlarmTrapSend(alarmIndex);
   return rc;
}

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
L7_RC_t trapMgrAlarmClear(L7_uint32 alarmIndex)
{
   L7_RC_t rc = L7_SUCCESS;
   L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
   L7_uint32 unit;

   unit = usmDbThisUnitGet();

   osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), chassis_falling_alarm_str, alarmIndex);
   trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
   trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

   trapMgrLogTrapToLocalLog(trapStringBuf);

   if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpChassisFallingAlarmTrapSend(alarmIndex);
   return rc;
}
#endif


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
L7_RC_t trapMgrLastVlanDeleteLastLogTrap(L7_uint32 vlanID)
{

  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (trapMgrCfgData.trapDot1q == L7_ENABLE)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), vlan_delete_last_str, vlanID);

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_LAST_VLAN_DELETE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpVlanDeleteLastTrapSend(unit, vlanID);
  }
  return rc;
}


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
L7_RC_t trapMgrDefaultVlanCfgFailureLogTrap(L7_uint32 vlanID)
{

  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (trapMgrCfgData.trapDot1q == L7_ENABLE)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), vlan_default_cfg_fail_str, vlanID);

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_DEFAULT_VLAN_DELETE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpVlanDefaultCfgFailureTrapSend(unit, vlanID);
  }
  return rc;
}

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
L7_RC_t trapMgrVlanRestoreFailureLogTrap(L7_uint32 vlanID)
{

  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (trapMgrCfgData.trapDot1q == L7_ENABLE)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), vlan_restore_fail_str, vlanID);

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VLAN_RESTORE_FAIL;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpVlanRestoreFailureTrapSend(unit, vlanID);
  }
  return rc;
}

void trapMgrLinkChangeCallBack()
{

  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (L7_FORWARDING)
  {
    if (coldStartSetTrapNeeded == L7_TRUE)
    {
      usmDbSnmpColdStartTrapSend(unit);
      coldStartSetTrapNeeded = L7_FALSE;
    }
    if (warmStartSetTrapNeeded == L7_TRUE)
    {
      usmDbSnmpWarmStartTrapSend(unit);
      warmStartSetTrapNeeded = L7_FALSE;
    }
  }

}


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
L7_RC_t trapMgrFanFailureLogTrap( L7_char8 *pStr )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), fan_status_str, pStr);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_FANSTATUS;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    rc = usmDbSnmpFanFailureTrapSend(unit);

  return rc;
}

/*********************************************************************
*
* @purpose  Announce a MAC Lock violation trap
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
L7_RC_t trapMgrMacLockViolationLogTrap( L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  L7_RC_t  rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_uint32 unit;

  if (trapMgrCfgData.trapMacLockViolation == L7_ENABLE)
  {
    unit = usmDbThisUnitGet();

    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), mac_lock_violation_str,
            usp.unit, usp.slot, usp.port,
            macAddr.addr[0],
            macAddr.addr[1],
            macAddr.addr[2],
            macAddr.addr[3],
            macAddr.addr[4],
            macAddr.addr[5],
            vlanId);

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_MACLOCK_VIOLATION;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NONE;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      /* call into SNMP */
      usmDbSnmpMacLockViolationTrapSend(intIfNum);
    }
  }

  return rc;
}

#ifdef L7_POE_PACKAGE
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
                                     L7_uint32 detection)
{
  L7_RC_t  rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (trapMgrCfgData.trapPoe == L7_ENABLE)
  {
    if (value == L7_POE_PORT_ON)
    {
      osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), poe_port_change_str, usp.unit, usp.slot, usp.port, "power up" );
    }
    else
    {
      osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), poe_port_change_str, usp.unit, usp.slot, usp.port, "power down" );
    }

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_POE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_POEPORTCHANGE;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      rc = usmDbSnmpPethPsePortOnOffNotificationTrapSend(usp.unit, intIfNum, detection);
    }
  }

  return rc;
}


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
                                        L7_uint32 allocPower )
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_char8  trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (trapMgrCfgData.trapPoe == L7_ENABLE)
  {

    if ( value == L7_POE_THRESHOLD_ABOVE)
    {
      osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), "PoE Above Threshold Crossing");
    }
    else
    {
      osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), "PoE Below Threshold Crossing");
    }

    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen  = TRAPMGR_GENERIC_POE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_SYSTEMTHRESHOLD;
    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
    {
      if ( value == L7_POE_THRESHOLD_ABOVE)
      {
        rc = usmDbSnmpPethMainPowerUsageOnNotificationTrapSend(Unit, allocPower);
      }
      else
      {
        rc = usmDbSnmpPethMainPowerUsageOffNotificationTrapSend(Unit, allocPower);
      }
    }
  }

  return rc;
}
#endif  /* L7_POE_PACKAGE */


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
L7_RC_t trapMgrEntConfigChangeLogTrap(void)
{
  L7_TRAP_t trap;
  L7_RC_t     rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_EDB_CONFIG_CHANGE;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_ENTCONFIGCHANGE;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

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
L7_RC_t trapMgrDot1agCfmFaultNotification(L7_uint32 mdIndex, L7_uint32 maIndex, L7_uint32 mepId, L7_uint32 highestPriDefect)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 defectStr[20]={0};
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
 
  switch(highestPriDefect)
  {
    case DOT1AG_MEP_DEFECT_BIT_bDefRDICCM: 
         osapiSnprintf(defectStr, sizeof(defectStr), "%s", "RDICCM");
         break;
    case DOT1AG_MEP_DEFECT_BIT_bDefMACstatus:
         osapiSnprintf(defectStr, sizeof(defectStr), "%s", "MACStatus");
         break;
    case DOT1AG_MEP_DEFECT_BIT_bDefRemoteCCM:
         osapiSnprintf(defectStr, sizeof(defectStr), "%s", "RemoteCCM");
         break;
    case DOT1AG_MEP_DEFECT_BIT_bDefErrorCCM:
         osapiSnprintf(defectStr, sizeof(defectStr), "%s", "ErrorCCM");
         break;
    case DOT1AG_MEP_DEFECT_BIT_bDefXconCCM:
         osapiSnprintf(defectStr, sizeof(defectStr), "%s", "XConnectCCM");
         break;
    default:
         osapiSnprintf(defectStr, sizeof(defectStr), "%s", "Unknown");
         break;
  }
  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), dot1ag_cfm_defect_notify_str, mdIndex, maIndex, mepId, defectStr);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_DOT1AG_CFM_DEFECT_NOTIFICATION;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_DOT1AG_CFM_DEFECT_NOTIFICATION;

  trapMgrLogTrapToLocalLog(trapStringBuf);

  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
  {
    rc = usmDbSnmpDot1agCfmFaultAlarmTrapSend(mdIndex, maIndex, mepId, highestPriDefect);
  }

  return rc;
}
#endif /* L7_DOT1AG_PACKAGE */

/*********************************************************************
*
* @purpose  Issue SFS start trap
*
* @param    unitId L7_int32 The unit id of stack member on which SFS started
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrUnitMgrSfsStartTrapSend(L7_int32 unitId)
{
  L7_TRAP_t trap;
  L7_RC_t rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STACK_FIRMWARE_SYNC_START;
  trap.u.stackFirmwareSync.stackMember = unitId;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STACKFIRMWARESYNCSTART;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Issue SFS Fail trap
*
* @param    unitId L7_int32 The unitId on which SFS failed
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrUnitMgrSfsFailTrapSend(L7_int32 unitId)
{
  L7_TRAP_t trap;
  L7_RC_t rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STACK_FIRMWARE_SYNC_FAIL;
  trap.u.stackFirmwareSync.stackMember = unitId;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STACKFIRMWARESYNCFAIL;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Issue SFS Finish trap
*
* @param    unitId L7_int32 The unitId on which SFS completed successfully
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrUnitMgrSfsFinishTrapSend(L7_int32 unitId)
{
  L7_TRAP_t trap;
  L7_RC_t rc = L7_SUCCESS;

  /* make sure you have message Queue created before sending and trap message */
  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_STACK_FIRMWARE_SYNC_FINISH;
  trap.u.stackFirmwareSync.stackMember = unitId;

  trap.gen = TRAPMGR_GENERIC_NONE;
  trap.spec = TRAPMGR_SPECIFIC_STACKFIRMWARESYNCFINISH;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

/*********************************************************************
* @purpose  Send a CPU Rising utilization threshold trap
*
* @param    risingThreshold  @b{(input)} configured CPU rising threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrSwitchCpuRisingThresholdTrap(L7_uint32 risingThreshold, L7_char8 *buf)
{
  L7_TRAP_t trap;
  L7_RC_t rc = L7_SUCCESS;

  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_CPU_RISING_THRESHOLD;
  trap.gen = TRAPMGR_GENERIC_CPU_UTILIZATION;
  trap.spec = TRAPMGR_SPECIFIC_CPU_RISING_THRESHOLD;
  trap.u.cpuUtil.threshold = risingThreshold;
  osapiStrncpySafe(trap.u.cpuUtil.buf, buf, sizeof(trap.u.cpuUtil.buf));

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

/*********************************************************************
* @purpose  Send a CPU Falling utilization threshold trap
*
* @param    fallingThreshold  @b{(input)} configured CPU falling threshold
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t trapMgrSwitchCpuFallingThresholdTrap(L7_uint32 fallingThreshold)
{
  L7_TRAP_t trap;
  L7_RC_t rc = L7_SUCCESS;

  if (trapMgrCnfgrPhaseReady() == L7_FALSE)
  {
    return L7_FAILURE;
  }

  trap.trapId = TRAP_CPU_FALLING_THRESHOLD;
  trap.gen = TRAPMGR_GENERIC_CPU_UTILIZATION;
  trap.spec = TRAPMGR_SPECIFIC_CPU_FALLING_THRESHOLD;
  trap.u.cpuUtil.threshold = fallingThreshold;

  rc = osapiMessageSend(trapMsgQueue, (void *)&trap, sizeof(L7_TRAP_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc != L7_SUCCESS)
  {
    trapMsgQueueLostMsgs++;
  }

  return rc;
}

/*********************************************************************
* @purpose  Send a CPU Free Memory below threshold trap
*
* @param    freeMemThreshold  @b{(input)} configured CPU free memory threshold
* @param    totalAllocated    @b{(input)} Total memory allocated
* @param    totalFree         @b{(input)} Total free memory
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrSwitchCpuFreeMemBelowThresholdTrap(L7_uint32 freeMemThreshold,
                                                  L7_uint32 totalAllocated,
                                                  L7_uint32 totalFree)
{
  L7_uint32 unit;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_RC_t  rc = L7_SUCCESS;

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), cpuFreeMemBelowThreshold_str, 
                freeMemThreshold, totalAllocated, totalFree);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_CPU_UTILIZATION;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CPU_FREE_MEM_BELOW_THRESHOLD;
  trapMgrLogTrapToLocalLog(trapStringBuf);

  unit = usmDbThisUnitGet();
  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
  {
    rc = usmDbSnmpSwitchCpuFreeMemBelowThresholdTrapSend(freeMemThreshold);
  }

  return rc;
}

/*********************************************************************
* @purpose  Send a CPU Free Memory above threshold trap
*
* @param    freeMemThreshold  @b{(input)} configured CPU free memory threshold
* @param    totalAllocated    @b{(input)} Total memory allocated
* @param    totalFree         @b{(input)} Total free memory
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t trapMgrSwitchCpuFreeMemAboveThresholdTrap(L7_uint32 freeMemThreshold,
                                                  L7_uint32 totalAllocated,
                                                  L7_uint32 totalFree)
{
  L7_uint32 unit;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_RC_t  rc = L7_SUCCESS;

  osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), CpuFreeMemAboveThreshold_str, 
                freeMemThreshold, totalAllocated, totalFree);
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_CPU_UTILIZATION;
  trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_CPU_FREE_MEM_ABOVE_THRESHOLD;
  trapMgrLogTrapToLocalLog(trapStringBuf);

  unit = usmDbThisUnitGet();
  if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
  {
    rc = usmDbSnmpSwitchCpuFreeMemAboveThresholdTrapSend(freeMemThreshold);
  }

  return rc;
}

