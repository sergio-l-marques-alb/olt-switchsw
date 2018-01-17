/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename trapmgr_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 04/09/2008
*
* @author cpverne    
* @end
*
**********************************************************************/

#ifndef __TRAPMGR_EXPORTS_H_
#define __TRAPMGR_EXPORTS_H_


/* TRAPMGR Component Feature List */
typedef enum
{
  L7_TRAPMGR_FEATURE_ID = 0,                  /* general support statement */
  L7_TRAPMGR_FEATURE_ID_TOTAL                 /* total number of enum values */
} L7_TRAPMGR_FEATURE_IDS_t;

/*--------------------------------------
|
|  Trap Manager API Constants
|
--------------------------------------*/

#define TRAPMGR_USER_DISPLAY  1
#define TRAPMGR_USER_TFTP     2

#define TRAPMGR_MSG_SIZE      275
#define TRAPMGR_TIME_STR_SIZE  25
/* #define TRAPMGR_TABLE_SIZE    256   !!! This is defined in defaultconfig.h !!! */

#define TRAPMGR_CPU_UTIL_BUF_SIZE   256

#define TRAPMGR_GENERIC_NONE              0
#define TRAPMGR_GENERIC_COLDSTART         1
#define TRAPMGR_GENERIC_WARMSTART         2
#define TRAPMGR_GENERIC_LINKDOWN          3
#define TRAPMGR_GENERIC_LINKUP            4
#define TRAPMGR_GENERIC_AUTFAILURE        5
#define TRAPMGR_GENERIC_EQPNEIGHBORLOSS   6
#define TRAPMGR_GENERIC_ENTSPECIFIC       7
#define TRAPMGR_GENERIC_FANSTATUS         8
#define TRAPMGR_GENERIC_POWERSTATUS       9
#define TRAPMGR_GENERIC_POE               10
#define TRAPMGR_GENERIC_MACLOCK_VIOLATION 11
#define TRAPMGR_GENERIC_LLDP_REM_CHANGE   12
#define TRAPMGR_GENERIC_WIRELESS          13
#define TRAPMGR_GENERIC_LLDP_MED_TOPO_CHANGE 14
#define TRAPMGR_GENERIC_CAPTIVE_PORTAL    15 
#define TRAPMGR_GENERIC_WIRELESS          13
#define TRAPMGR_GENERIC_DOT1X_UNATH_HOST  14
#define TRAPMGR_GENERIC_MAC_ACL_DENY_RULE_MATCH  15
#define TRAPMGR_GENERIC_IP_ACL_DENY_RULE_MATCH   16
#define TRAPMGR_GENERIC_DOT1AG_CFM_DEFECT_NOTIFICATION 17
#define TRAPMGR_GENERIC_IP_ADDRESS_CONFLICT 18
#define TRAPMGR_GENERIC_CPU_UTILIZATION   19

#define TRAPMGR_SPECIFIC_NONE             0
#define TRAPMGR_SPECIFIC_MULTIPLEUSERS    1
#define TRAPMGR_SPECIFIC_LINKFAILURE      2
#define TRAPMGR_SPECIFIC_BCASTSTORM_START 3
#define TRAPMGR_SPECIFIC_BCASTSTORM_END   4
#define TRAPMGR_SPECIFIC_VLAN_REQUEST_FAILURE   5
#define TRAPMGR_SPECIFIC_LAST_VLAN_DELETE       6
#define TRAPMGR_SPECIFIC_DEFAULT_VLAN_DELETE    7
#define TRAPMGR_SPECIFIC_VLAN_RESTORE_FAIL      8
#define TRAPMGR_SPECIFIC_DAI_ERROR_DISABLE 9
#define TRAPMGR_SPECIFIC_DHCP_SNOOPING_ERROR_DISABLE 10

#define TRAPMGR_SPECIFIC_NEWROOT          1
#define TRAPMGR_SPECIFIC_TOPOLOGYCHANGE   2
#define TRAPMGR_SPECIFIC_LOOP_INCONSISTENT_START 1 
#define TRAPMGR_SPECIFIC_LOOP_INCONSISTENT_END   2
#define TRAPMGR_SPECIFIC_RISINGALARM      1
#define TRAPMGR_SPECIFIC_FALLINGALARM     2
#define TRAPMGR_SPECIFIC_PACKETMATCH      3
#define TRAPMGR_SPECIFIC_REGISTER         4
#define TRAPMGR_SPECIFIC_NEUROTHRESHOLD   5

#define TRAPMGR_SPECIFIC_POEPORTCHANGE    1
#define TRAPMGR_SPECIFIC_SYSTEMTHRESHOLD  2

#define TRAPMGR_SPECIFIC_CONFIG_CHANGED 1
#define TRAPMGR_SPECIFIC_TFTP_END 2
#define TRAPMGR_SPECIFIC_TFTP_ABORT 3
#define TRAPMGR_SPECIFIC_TFTP_START 4
#define TRAPMGR_SPECIFIC_LINK_FAILURE 5
#define TRAPMGR_SPECIFIC_VLANDYNPORTADDED 6
#define TRAPMGR_SPECIFIC_VLANDYNPORTREMOVED 7
#define TRAPMGR_SPECIFIC_STACKMASTERFAILED 8
#define TRAPMGR_SPECIFIC_STACKNEWMASTERELECTED 9
#define TRAPMGR_SPECIFIC_STACKMEMBERUNITFAILED 10
#define TRAPMGR_SPECIFIC_STACKMEMBERUNITADDED 11
#define TRAPMGR_SPECIFIC_STACKMEMBERUNITREMOVED 12
#define TRAPMGR_SPECIFIC_STACKSPLITMASTERREPORT 13
#define TRAPMGR_SPECIFIC_STACKSPLITNEWMASTERREPORT 14
#define TRAPMGR_SPECIFIC_STACKREJOINED 15
#define TRAPMGR_SPECIFIC_STACKLINKFAILED 16
#define TRAPMGR_SPECIFIC_DOT1DSTPPORTSTATEFORWARDING 17
#define TRAPMGR_SPECIFIC_DOT1DSTPPORTSTATENOTFORWARDING 18
#define TRAPMGR_SPECIFIC_TRUNKPORTADDED 19
#define TRAPMGR_SPECIFIC_TRUNKPORTREMOVED 20
#define TRAPMGR_SPECIFIC_LOCKPORT 21
#define TRAPMGR_SPECIFIC_VLANDYNVLANADDED 22
#define TRAPMGR_SPECIFIC_VLANDYNVLANREMOVED 23
#define TRAPMGR_SPECIFIC_ENVMONFANSTATECHANGE 24
#define TRAPMGR_SPECIFIC_ENVMONPOWERSUPPLYSTATECHANGE 25
#define TRAPMGR_SPECIFIC_ENVMONTEMPERATURERISINGALARM 26
#define TRAPMGR_SPECIFIC_COPYFINISHED 27
#define TRAPMGR_SPECIFIC_COPYFAILED 28
#define TRAPMGR_SPECIFIC_DOT1XPORTSTATUSAUTHORIZED 29
#define TRAPMGR_SPECIFIC_DOT1XPORTSTATUSUNAUTHORIZED 30
#define TRAPMGR_SPECIFIC_STPELECTEDASROOT 31
#define TRAPMGR_SPECIFIC_STPNEWROOTELECTED 32
#define TRAPMGR_SPECIFIC_INVALIDUSERLOGINATTEMPTED 33
#define TRAPMGR_SPECIFIC_MANAGEMENTACLVIOLATION 34
#define TRAPMGR_SPECIFIC_XFP_INSERTED 35
#define TRAPMGR_SPECIFIC_XFP_REMOVED  36
#define TRAPMGR_SPECIFIC_SFP_INSERTED 37
#define TRAPMGR_SPECIFIC_SFP_REMOVED  38
#define TRAPMGR_SPECIFIC_ENTCONFIGCHANGE 39
#define TRAPMGR_SPECIFIC_STACKRESTARTCOMPLETE 40

#define TRAPMGR_SPECIFIC_DOT1AG_CFM_DEFECT_NOTIFICATION 40

#define TRAPMGR_SPECIFIC_STACKFIRMWARESYNCSTART 42
#define TRAPMGR_SPECIFIC_STACKFIRMWARESYNCFAIL 43
#define TRAPMGR_SPECIFIC_STACKFIRMWARESYNCFINISH 44

#define TRAPMGR_SPECIFIC_CPU_RISING_THRESHOLD           45
#define TRAPMGR_SPECIFIC_CPU_FALLING_THRESHOLD          46
#define TRAPMGR_SPECIFIC_CPU_FREE_MEM_BELOW_THRESHOLD   47
#define TRAPMGR_SPECIFIC_CPU_FREE_MEM_ABOVE_THRESHOLD   48

#define SNMP_SIZE_BUFCHR            256
#define SNMP_SIZE_BUFINT            128
#define SNMP_SIZE_OBJECTID          128
#define SNMP_PDU_TRAP               4

#define SNMP_TRAP_COLDSTART          0
#define SNMP_TRAP_WARMSTART          1
#define SNMP_TRAP_LINKDOWN           2
#define SNMP_TRAP_LINKUP             3
#define SNMP_TRAP_AUTFAILURE         4
#define SNMP_TRAP_EQPNEIGHBORLOSS    5
#define SNMP_TRAP_ENTSPECIFIC        6


typedef union
{
  long           LngInt;
  unsigned int   LngUns;
  unsigned char  BufChr [SNMP_SIZE_BUFCHR];
  unsigned int   BufInt [SNMP_SIZE_BUFINT];
  void     *Ptr;
} snmp_syntax_t;

typedef struct
{
  unsigned short Request;
  unsigned long  Id[SNMP_SIZE_OBJECTID];
  unsigned int   IdLen;
  unsigned short Type;
  snmp_syntax_t  Syntax;
  unsigned int   SyntaxLen;
} snmp_object_t;

/*--------------------------------------
|
|  Trap Manager API Data Structures
|
--------------------------------------*/

typedef struct
{
  L7_ushort16 gen;
  L7_ushort16 spec;
  L7_char8    timestamp[TRAPMGR_TIME_STR_SIZE];
  L7_char8    message[TRAPMGR_MSG_SIZE];
} trapMgrTrapLogEntry_t;

typedef enum
{
  L7_SFP_DATA = 1,
  L7_XFP_DATA,
} sfpXfpType_t;

typedef struct sfpXfpData_s
{
  sfpXfpType_t     type;
  L7_uint32        unitNum;
  L7_uint32        portNum;
  L7_BOOL          state;
}sfpXfpData_t;

#define SFP_XFP_DATA_SIZE sizeof(sfpXfpData_t)

#define L7_MAX_TRAP_OID_LENGTH 100
#define L7_MAX_TRAP_SOURCE_LENGTH 100
#define L7_MAX_TRAP_INFO_LENGTH  100

/* Trap Source Format Strings */
#define FORMAT_CONFIGCHANGED_SRC_STR               "%s"
#define FORMAT_ENDTFTP_SRC_STR                     "Unit=%d"
#define FORMAT_ABORTTFTP_SRC_STR                   "Unit=%d"
#define FORMAT_STARTTFTP_SRC_STR                   "Unit=%d"
#define FORMAT_LINKFAILURE_SRC_STR                 "Link Index=%d"
#define FORMAT_VLANDYNPORTADDED_SRC_STR            "Vlan Index=%d"
#define FORMAT_VLANDYNPORTREMOVED_SRC_STR          "Vlan Index=%d"
#define FORMAT_STACKMASTERFAILED_SRC_STR           "New Stack Master Unit=%d"
#define FORMAT_STACKNEWMASTERELECTED_SRC_STR       "New Stack Master Unit=%d"
#define FORMAT_STACKMEMBERUNITFAILED_SRC_STR       "Stack Master Unit=%d"
#define FORMAT_STACKMEMBERUNITADDED_SRC_STR       "Stack Master Unit=%d"
#define FORMAT_STACKMEMBERUNITREMOVED_SRC_STR       "Stack Master Unit=%d"
#define FORMAT_STACKSPLITMASTERREPORT_SRC_STR      "Stack Master Unit=%d"
#define FORMAT_STACKSPLITNEWMASTERREPORT_SRC_STR      "Stack Master Unit=%d"
#define FORMAT_STACKREJOINED_SRC_STR               "Stack Master Unit=%d"
#define FORMAT_STACKLINKFAILED_SRC_STR             "Unit=%d"
#define FORMAT_DOT1DSTPPORTSTATEFORWARDING_SRC_STR "Unit=%d, Port=%d"
#define FORMAT_DOT1DSTPPORTSTATENOTFORWARDING_SRC_STR "Unit=%d, Port=%d"
#define FORMAT_TRUNKPORTADDEDTRAP_SRC_STR          "Trunk=%d"
#define FORMAT_TRUNKPORTREMOVEDTRAP_SRC_STR          "Trunk=%d"
#define FORMAT_LOCKPORTTRAP_SRC_STR                "Port=%d"
#define FORMAT_VLANDYNVLANADDED_SRC_STR            "Vlan Index=%d"
#define FORMAT_VLANDYNVLANREMOVED_SRC_STR            "Vlan Index=%d"
#define FORMAT_ENVMONFANSTATECHANGE_SRC_STR        "Unit %d, Fan:%d"
#define FORMAT_ENVMONPOWERSUPPLYSTATECHANGE_SRC_STR "Unit %d, Power Supply:%d"
#define FORMAT_ENVMONTEMPERATURERISINGALARM_SRC_STR "Unit=%d"
#define FORMAT_COPYFINISHED_SRC_STR                "Unit=%d"
#define FORMAT_COPYFAILED_SRC_STR                "Unit=%d"
#define FORMAT_DOT1XPORTSTATUSAUTHORIZEDTRAP_SRC_STR   "Unit=%d, Port=%d"
#define FORMAT_DOT1XPORTSTATUSUNAUTHORIZEDTRAP_SRC_STR   "Unit=%d, Port=%d"
#define FORMAT_STPELECTEDASROOT_SRC_STR            ""
#define FORMAT_STPNEWROOTELECTED_SRC_STR           "Unit=%d"
#define FORMAT_INVALIDUSERLOGINATTEMPTED_SRC_STR   "Unit=%d"
#define FORMAT_MANAGEMENTACLVIOLATION_SRC_STR      "Unit=%d"
#define FORMAT_SFP_XFP_INSERTIONREMOVAL_SRC_STR    "Unit=%d"

/* Trap Info Format Strings */
#define FORMAT_CONFIGCHANGED_INFO_STR              "%s"
#define FORMAT_ENDTFTP_INFO_STR                    "Exit code=%d, File=%s"
#define FORMAT_ABORTTFTP_INFO_STR                  "Exit code=%d, File=%s"
#define FORMAT_STARTTFTP_INFO_STR                  "File=%s"
#define FORMAT_LINKFAILURE_INFO_STR                ""
#define FORMAT_VLANDYNPORTADDED_INFO_STR           "Port=%d"
#define FORMAT_VLANDYNPORTREMOVED_INFO_STR         "Port=%d"
#define FORMAT_STACKMASTERFAILED_INFO_STR          "Old Stack Master Unit=%d"
#define FORMAT_STACKNEWMASTERELECTED_INFO_STR      "Old Stack Master Unit=%d"
#define FORMAT_STACKMEMBERUNITFAILED_INFO_STR      "Failed member unit=%d"
#define FORMAT_STACKMEMBERUNITADDED_INFO_STR       "New member unit=%d"
#define FORMAT_STACKMEMBERUNITREMOVED_INFO_STR     "Removed member unit=%d"
#define FORMAT_STACKSPLITMASTERREPORT_INFO_STR     "Splitted units=%s"
#define FORMAT_STACKSPLITNEWMASTERREPORT_INFO_STR  "Old Stack Master Unit=%d"
#define FORMAT_STACKREJOINED_INFO_STR              "Member units=%s"
#define FORMAT_STACKLINKFAILED_INFO_STR            "%s"
#define FORMAT_DOT1DSTPPORTSTATEFORWARDING_INFO_STR  "Instance %d"
#define FORMAT_DOT1DSTPPORTSTATENOTFORWARDING_INFO_STR  "Instance %d"
#define FORMAT_TRUNKPORTADDEDTRAP_INFO_STR         "Port=%d"
#define FORMAT_TRUNKPORTREMOVEDTRAP_INFO_STR       "Port=%d"
#define FORMAT_LOCKPORTTRAP_INFO_STR               "MAC Address=%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x"
#define FORMAT_VLANDYNVLANADDED_INFO_STR           ""
#define FORMAT_VLANDYNVLANREMOVED_INFO_STR         ""
#define FORMAT_ENVMONFANSTATECHANGE_INFO_STR       "Fan State=%d"
#define FORMAT_ENVMONPOWERSUPPLYSTATECHANGE_INFO_STR "Power Supply State=%d"
#define FORMAT_ENVMONTEMPERATURERISINGALARM_INFO_STR ""
#define FORMAT_COPYFINISHED_INFO_STR                 ""
#define FORMAT_COPYFAILED_INFO_STR                   ""
#define FORMAT_DOT1XPORTSTATUSAUTHORIZEDTRAP_INFO_STR  ""
#define FORMAT_DOT1XPORTSTATUSUNAUTHORIZEDTRAP_INFO_STR ""
#define FORMAT_STPELECTEDASROOT_INFO_STR                ""
#define FORMAT_STPNEWROOTELECTED_INFO_STR            "STP Instance ID=%d, Root ID=%4.4x:%4.4x:%4.4x:%4.4x"
#define FORMAT_INVALIDUSERLOGINATTEMPTED_INFO_STR  "User attempted login through %s from %s IP Address"
#define FORMAT_MANAGEMENTACLVIOLATION_INFO_STR  "User attempted login through %s from %s IP Address"
#define FORMAT_SFP_INSERTION_INFO_STR    "SFP Inserted in Unit %d, Interface %d"
#define FORMAT_SFP_REMOVAL_INFO_STR    "SFP Removed from Unit %d, Interface %d"
#define FORMAT_XFP_INSERTION_INFO_STR    "XFP Inserted in Unit %d, Interface %d"
#define FORMAT_XFP_REMOVAL_INFO_STR    "XFP Removed from Unit %d, Interface %d"


#define D_trapSeverity_info     0
#define D_trapSeverity_minor    1
#define D_trapSeverity_critical 2
#define D_trapSeverity_major    3

/* Severity definiyions */
#define TRAP_CONFIGCHANGED_SEVERITY                  D_trapSeverity_info
#define TRAP_ENDTFTP_SEVERITY                        D_trapSeverity_info
#define TRAP_ABORTTFTP_SEVERITY                      D_trapSeverity_minor
#define TRAP_STARTTFTP_SEVERITY                      D_trapSeverity_info
#define TRAP_LINKFAILURE_SEVERITY                    D_trapSeverity_critical
#define TRAP_VLANDYNPORTADDED_SEVERITY               D_trapSeverity_info
#define TRAP_VLANDYNPORTREMOVED_SEVERITY             D_trapSeverity_minor
#define TRAP_STACKMASTERFAILED_SEVERITY              D_trapSeverity_critical
#define TRAP_STACKNEWMASTERELECTED_SEVERITY          D_trapSeverity_info
#define TRAP_STACKMEMBERUNITFAILED_SEVERITY          D_trapSeverity_major
#define TRAP_STACKMEMBERUNITADDED_SEVERITY           D_trapSeverity_info
#define TRAP_STACKMEMBERUNITREMOVED_SEVERITY         D_trapSeverity_minor
#define TRAP_STACKSPLITMASTERREPORT_SEVERITY         D_trapSeverity_info
#define TRAP_STACKSPLITNEWMASTERREPORT_SEVERITY      D_trapSeverity_info
#define TRAP_STACKREJOINED_SEVERITY                  D_trapSeverity_info
#define TRAP_STACKLINKFAILED_SEVERITY                D_trapSeverity_major
#define TRAP_DOT1DSTPPORTSTATEFORWARDING_SEVERITY    D_trapSeverity_info
#define TRAP_DOT1DSTPPORTSTATENOTFORWARDING_SEVERITY D_trapSeverity_info
#define TRAP_TRUNKPORTADDEDTRAP_SEVERITY             D_trapSeverity_info
#define TRAP_TRUNKPORTREMOVEDTRAP_SEVERITY           D_trapSeverity_minor
#define TRAP_LOCKPORTTRAP_SEVERITY                   D_trapSeverity_info
#define TRAP_VLANDYNVLANADDED_SEVERITY               D_trapSeverity_info
#define TRAP_VLANDYNVLANREMOVED_SEVERITY             D_trapSeverity_minor
#define TRAP_ENVMONFANSTATECHANGE_SEVERITY           D_trapSeverity_minor  
#define TRAP_ENVMONPOWERSUPPLYSTATECHANGE_SEVERITY   D_trapSeverity_minor
#define TRAP_ENVMONTEMPERATURERISINGALARM_SEVERITY   D_trapSeverity_major
#define TRAP_COPYFINISHED_SEVERITY                   D_trapSeverity_info
#define TRAP_COPYFAILED_SEVERITY                     D_trapSeverity_info
#define TRAP_DOT1XPORTSTATUSAUTHORIZEDTRAP_SEVERITY  D_trapSeverity_info
#define TRAP_DOT1XPORTSTATUSUNAUTHORIZEDTRAP_SEVERITY D_trapSeverity_minor
#define TRAP_STPELECTEDASROOT_SEVERITY               D_trapSeverity_info
#define TRAP_STPNEWROOTELECTED_SEVERITY              D_trapSeverity_info
#define TRAP_INVALIDUSERLOGINATTEMPTED_SEVERITY      D_trapSeverity_minor
#define TRAP_MANAGEMENTACLVIOLATION_SEVERITY         D_trapSeverity_minor
#define TRAP_SFP_XFP_INSERTIONREMOVAL_SEVERITY       D_trapSeverity_info

typedef enum
{
  TRAP_CONFIG_CHANGED = 1,
  TRAP_END_TFTP,
  TRAP_ABORT_TFTP,
  TRAP_START_TFTP,
  TRAP_LINK_FAILURE,
  TRAP_VLAN_DYN_PORT_ADDED,
  TRAP_VLAN_DYN_PORT_REMOVED,
  TRAP_STACK_MASTER_FAILED,
  TRAP_STACK_NEW_MASTER_ELECTED,
  TRAP_STACK_NEW_MEMBER_UNIT_ADDED,
  TRAP_STACK_MEMBER_UNIT_REMOVED,
  TRAP_STACK_LINK_FAILED,
  TRAP_STACK_RESTART_COMPLETE,
  TRAP_DOT1D_STP_PORTSTATE_FWD,
  TRAP_DOT1D_STP_PORTSTATE_NOT_FWD,
  TRAP_TRUNK_PORT_ADDED,
  TRAP_TRUNK_PORT_REMOVED,
  TRAP_LOCK_PORT,
  TRAP_VLAN_DYN_VLAN_ADDED,
  TRAP_VLAN_DYN_VLAN_REMOVED,
  TRAP_ENV_MON_FAN_STATE_CHANGE,
  TRAP_ENV_MON_POWER_SUPPLY_STATE_CHANGE,
  TRAP_ENV_MON_TEMP_RISING,
  TRAP_COPY_FINISHED,
  TRAP_COPY_FAILED,
  TRAP_DOT1X_PORT_STATUS_AUTHORIZED,
  TRAP_DOT1X_PORT_STATUS_UNAUTHORIZED,
  TRAP_STP_ELECTED_AS_ROOT,
  TRAP_STP_NEW_ROOT_ELECTED,
  TRAP_INVALID_USER_LOGIN_ATTEMPTED,
  TRAP_MGMT_ACL_VIOLATION,
  TRAP_XFP_INSERTION_REMOVAL,
  TRAP_SFP_INSERTION_REMOVAL,
  TRAP_EDB_CONFIG_CHANGE,
  TRAP_LINK_DOWN,
  TRAP_LINK_UP,
  TRAP_DOT1AG_CFM_DEFECT_NOTIFY,
  TRAP_STACK_FIRMWARE_SYNC_START,
  TRAP_STACK_FIRMWARE_SYNC_FINISH,
  TRAP_STACK_FIRMWARE_SYNC_FAIL,
  TRAP_CPU_RISING_THRESHOLD,
  TRAP_CPU_FALLING_THRESHOLD,

  L7_LAST_TRAP_ID
} L7_TRAP_ID_t;


typedef struct l7_trap_s
{
  L7_TRAP_ID_t   trapId;
  L7_ushort16    gen;
  L7_ushort16    spec;
  L7_char8       trapStringBuf[TRAPMGR_MSG_SIZE];

  union
  {
    struct
    {
      L7_uchar8  trapSource[L7_MAX_TRAP_SOURCE_LENGTH];
      L7_uchar8  trapInfo[L7_MAX_TRAP_INFO_LENGTH];
    } configChanged;
    struct
    {
      L7_uint32  exitCode;
      L7_uchar8  fName[L7_MAX_TRAP_INFO_LENGTH];
    } endTftp;
    struct
    {
      L7_uint32  exitCode;
      L7_uchar8  fName[L7_MAX_TRAP_INFO_LENGTH];
    } abortTftp;
    struct
    {
      L7_int32 ifIndex;
    } linkFailure;
    struct
    {     
      L7_uchar8  fName[L7_MAX_TRAP_INFO_LENGTH];
    } startTftp;
    struct
    {
      L7_int32 dot1qVlanIndex;
      L7_int32 port;
    } vlanDynPortAdded;
    struct
    {
      L7_int32 dot1qVlanIndex;
      L7_int32 port;
    } vlanDynPortRemoved;
    struct
    {
      L7_int32 oldUnitNumber;
    } stackMasterFailed;
    struct
    {
      L7_int32 oldUnitNumber;
    } stackNewMasterElected;
    struct
    {
      L7_int32 newUnitNumber;
    } stackNewMemberUnitAdded;
    struct
    {
      L7_int32 removedUnitNumber;
    } stackMemberUnitRemoved;
    struct
    {
      L7_int32 identifiedUnit;
      L7_uchar8 info[L7_MAX_TRAP_INFO_LENGTH];
    } stackLinkFailed;
    struct
    {
      L7_int32 unitId;
      L7_LAST_STARTUP_REASON_t reason;
    } stackRestartComplete;
    struct
    {
      L7_int32 port;
      L7_uint32 instanceIdx;
    } dot1dStpPortStateForwarding;
    struct
    {
      L7_int32 port;
      L7_uint32 instanceIdx;
    } dot1dStpPortStateNotForwarding;
    struct
    {
      L7_int32 trunkIfIndex;
      L7_int32 port;
    } trunkPortAddedTrap;
    struct
    {
      L7_int32 trunkIfIndex;
      L7_int32 port;
    } trunkPortRemovedTrap;
    struct
    {
      L7_int32 port;
      L7_enetMacAddr_t macAddr;
    } lockPortTrap;
    struct
    {
      L7_int32 dot1qVlanIndex;
    } vlanDynVlanAdded;
    struct
    {
      L7_int32 dot1qVlanIndex;
    } vlanDynVlanRemoved;

    struct
    {
      L7_uint32 trap_unit;
      L7_int32 fanIndex;
      L7_BOOL status;
    } envMonFanStateChange;

    struct
    {
      L7_uint32 trap_unit;
      L7_int32 envMonSupplyIndex;
      L7_BOOL status;
    } envMonPowerSupplyStateChange;
    struct
    {
      L7_uint32 unit;
    } envMonTemperatureRisingAlarm;
    struct
    {
      L7_uint32 noParameter;
    } copyFinished;
    struct
    {
      L7_uint32 noParameter;
    } copyFailed;
    struct
    {
      L7_int32 port;
    } dot1xPortStatusAuthorizedTrap;
    struct
    {
      L7_int32 port;
    } dot1xPortStatusUnauthorizedTrap;
    struct
    {
      L7_uint32 noParameter;
    } stpElectedAsRoot;
    struct
    {
      L7_int32 instanceID;
      L7_uchar8 rootId[8];
    } stpNewRootElected;
    struct
    {
      L7_uchar8 uiMode[L7_MAX_TRAP_SOURCE_LENGTH];
      L7_uchar8 fromIpAddress[L7_MAX_TRAP_INFO_LENGTH];
    } invalidUserLoginAttempted;
    struct
    {
      L7_uchar8 uiMode[L7_MAX_TRAP_SOURCE_LENGTH];
      L7_uchar8 fromIpAddress[L7_MAX_TRAP_INFO_LENGTH];
    } managementACLViolation;
    struct
    {
      L7_int32 Unit;
      L7_int32 intIfNum;
      L7_BOOL status;
    } xfpInsertionRemoval;
    struct
    {
      L7_int32 Unit;
      L7_int32 intIfNum;
      L7_BOOL status;
    } sfpInsertionRemoval;
    struct
    {
      L7_int32 intIfNum;
    } linkStatus;
    struct 
    {
      L7_int32 stackMember;
    }stackFirmwareSync;
    struct
    {
      L7_uint32 threshold;
      L7_uchar8 buf[TRAPMGR_CPU_UTIL_BUF_SIZE];
    } cpuUtil;
  } u;
} L7_TRAP_t;



typedef struct
{
  L7_TRAP_ID_t  registrar_ID;
  L7_RC_t       (*sendTrap)(L7_TRAP_t*);        /* trap send routine */
} trapMgrFunctionList_t;


/******************** conditional Override *****************************/

#ifdef INCLUDE_TRAPMGR_EXPORTS_OVERRIDES
#include "trapmgr_exports_overrides.h"
#endif

#endif /* __TRAPMGR_EXPORTS_H_*/
