/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename sim.h
*
* @purpose System Interface Manager Constants and Data Structures
*
* @component sim
*
* @comments none
*
* @create 08/21/2000
*
* @author bmutz
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#ifndef INCLUDE_SIM
#define INCLUDE_SIM

#include <nimapi.h>
#include "defaultconfig.h"
#include "comm_structs.h"
#include "l7_cnfgr_api.h"
#include "dot1q_api.h"
#include "portevent_mask.h"

#define SIM_CFG_FILENAME  "simCfgData.cfg"
#define SIM_CFG_VER_1      0x1
#define SIM_CFG_VER_2      0x2
#define SIM_CFG_VER_3      0x3
#define SIM_CFG_VER_4      0x4
#define SIM_CFG_VER_5      0x5
#define SIM_CFG_VER_6      0x6

#define SIM_CFG_VER_CURRENT    SIM_CFG_VER_6

#define SUMMERTIME_CFG_FILENAME "summerTimeCfgData"
#define FD_SUMMER_TIME_MODE     0

#define SIM_DEFAULT_AGING_TIMEOUT 300
#define SUMMERTIME_LAST_WEEK_OF_MONTH 5

#define SUMMERTIME_LAST_WEEK_OF_MONTH 5
#define SIM_DEBUG_CFG_FILENAME "sim_debug.cfg"
#define SIM_DEBUG_CFG_VER_1          0x1
#define SIM_DEBUG_CFG_VER_CURRENT    SIM_DEBUG_CFG_VER_1

/* If free memory has recovered by this percentage, then memory recovered
 * trap is generated.
 */
#define SIM_FREE_MEMORY_RECOVERED_THRESHOLD (10/100)

extern L7_char8         systemBIA[L7_MAC_ADDR_LEN];

typedef enum 
{
  SIM_MGMT_ADDR_CONF = 0,   /* Configured address */
  SIM_MGMT_ADDR_DHCP,       /* Address from DHCP or BOOTP */
  SIM_MGMT_ADDR_CKPT        /* Checkpointed from previous mgmt unit */
} e_simMgmtIpAddrType;

typedef struct
{
  /* If free memory falls blow this then send alarm. A value of 0 indicates
   * no free memory threshold monitoring.
   */
  L7_uint32 freeMemoryThreshold; 

} simCpuMemoryMonitorData_t;

typedef struct
{
  /* If total cpu util goes above the rising threshold then send a alarm. A 
   * value of 0 for risingThreshold indicates that cpu util monitoring is not
   * being done. 
   */
  L7_uint32 risingThreshold;
  L7_uint32 risingPeriod;

  /* After the rising threshold has been crossed, if total cpu util goes below 
   * the falling threshold then send a alarm. 
   */
  L7_uint32 fallingThreshold;
  L7_uint32 fallingPeriod;

} simCpuUtilMonitorData_t;

/****************************************
*
*  SIM Configuration Data
*
*****************************************/

typedef struct summerTimeNonRcurring_s
{
  L7_uint32   start_month;
  L7_uint32   start_date;
  L7_uint32   start_year;
  L7_uint32   start_time;       /*This will store start time in terms of minutes*/

  L7_uint32   end_month;
  L7_uint32   end_date;
  L7_uint32   end_year;
  L7_uint32   end_time;       /*This will store end time in terms of minutes*/

  L7_uint32   offset_minutes;
  L7_char8   zone[L7_TIMEZONE_ACRONYM_LEN + 1];

} summerTimeNonRecurring_t;

typedef struct summerTimeRecurring_s
{
  L7_uint32   start_week;
  L7_uint32   start_day;
  L7_uint32   start_month;
  L7_uint32   start_time;       /*This will store start time in terms of minutes {hours * (60) + minutes}*/

  L7_uint32   end_week;
  L7_uint32   end_day;
  L7_uint32   end_month;
  L7_uint32   end_time;       /*This will store end time in terms of minutes {hours * (60) + minutes} */

  L7_uint32   offset_minutes;
  L7_char8   zone[L7_TIMEZONE_ACRONYM_LEN + 1];

} summerTimeRecurring_t;


typedef struct L7_summerTime_s
{
  summerTimeNonRecurring_t  nonrecurring;
  summerTimeRecurring_t     recurring;

  L7_uint32                 mode;
  L7_uint32                 persistMode;
  L7_uint32                 stTimeInSeconds;
  L7_uint32                 endTimeInSeconds;

} summerTimeCfgData_t;

typedef void (*transferCompletionHandlerFnPtr)(L7_TRANSFER_STATUS_t);

typedef struct
{
  /*
   * This structure contains all the attributes involved in transfer from/to
   * the switch. These attributes need not be part of configuration.
   *  of them are populated during the transfer ( tftp/xmodem/...) command
   * execution
   */
  L7_uint32      systemTransferMode;
  L7_uint32      systemUploadFileType;
  L7_uint32      systemDownloadFileType;
  L7_uint32      systemTransferDirection;
  L7_BOOL        systemTransferComplete;
  L7_uint32      systemBytesTransferred;

  L7_char8       systemTransferFilePathLocal[L7_MAX_FILEPATH+1];
  L7_char8       systemTransferFileNameLocal[L7_MAX_FILENAME+1];
  L7_char8       systemTransferFilePathRemote[L7_MAX_FILEPATH+1];
  L7_char8       systemTransferFileNameRemote[L7_MAX_FILENAME+1];

  L7_inet_addr_t systemTransferServerIp; /* IPv4 / IPv6 tftp server */

  L7_char8       systemFTPServerUserName[L7_CLI_MAX_STRING_LENGTH+1]; /* for ftp upload */
  L7_char8       systemFTPServerPassword[L7_CLI_MAX_STRING_LENGTH+1]; /* for ftp upload */
} simTransferInfo_t;

typedef struct
{
  L7_in6_addr_t       ip6Addr;         /* IPv6 Address                   */
  L7_ushort16         ip6PrefixLen;    /* IPv6 Mask len                  */
  L7_ushort16         eui_flag;
} simCfgIpv6IfData_t;

/* Structure that holds the status information regarding the
 * last detected conflicting address since the reset of the
 * status information */
typedef struct simAddrConflictStatus_s
{
  L7_BOOL	conflictDetectStatus;
  L7_uint32	conflictIPAddr;
  L7_uchar8	conflictMacAddr[L7_MAC_ADDR_LEN];

  /* Uptime in seconds when the conflict was last detected */
  L7_uint32	conflictReportedTime;
} simAddrConflictStatus_t;

/* IP addresses on the network and service ports can be configured 
 * or learned from a DHCP server. In a nonstop forwarding stack, 
 * they can also be checkpointed. As of release 6.2, the configuration
 * structure only holds configured values. Values learned from DHCP
 * are put here directly. */
typedef struct simOperInfo_s
{
  /* Operational service port IPv4 address and default gateway. 
   * On a backup unit in a stack, the checkpointed address is put here.
   * If there is a configured address, the configured address overwrites
   * the checkpointed address as soon as the configuration is pushed. Otherwise,
   * the first time DHCP runs, it either confirms the checkpointed address,
   * setting servPortAddrType to SIM_MGMT_ADDR_DHCP, sets a new address if
   * the DHCP server gives us a new address, or zeroes the address, if DHCP
   * fails. */
  L7_uint32      servPortIpAddress;
  L7_uint32      servPortNetMask;
  L7_uint32      servPortGateway;
  e_simMgmtIpAddrType servPortAddrType;

  /* Manually configured service port IPv6 addresses and default gateway */
  simCfgIpv6IfData_t  servPortIpv6Intfs[FD_SIM_MAX_INTF_IPV6_ADDRS];
  L7_in6_addr_t  servPortIpv6Gateway;

  /* We can have manually ipv6 configured addresses as well as dynamically learnt 
   * address on the service port at the same time in the system.
   *
   * Need to checkpoint the Service port ipv6 address learnt via DHCPv6 client. */
  L7_in6_addr_t  servPortDhcpv6Addr;  /* Is valid if non-zero address */

  /* Operational network port IPv4 address and default gateway */
  L7_uint32      systemIpAddress;
  L7_uint32      systemNetMask;
  L7_uint32      systemGateway;
  e_simMgmtIpAddrType systemAddrType;

  /* Manually configured network port IPv6 addresses and default gateway */
  simCfgIpv6IfData_t  systemIpv6Intfs[FD_SIM_MAX_INTF_IPV6_ADDRS];
  L7_in6_addr_t  systemIpv6Gateway;

  /* We can have ipv6 manually configured addresses as well as dynamically learnt
   * address on the network port at the same time in the system.
   *
   * Need to checkpoint the network port ipv6 address learnt via DHCPv6 client. */
  L7_in6_addr_t  systemDhcpv6Addr;  /* Is valid if non-zero address */

  /* L7_TRUE if Configurator reported last restart WARM */
  L7_BOOL simWarmRestart;

  /* L7_TRUE if StartupConfig file exists */
  L7_BOOL startupConfigIsExists;

  /* Operational information of address conflict feature on 
   * service port and network port */
  simAddrConflictStatus_t	conflictStatus;
  addrConflictStats_t		servicePortConflictStats;
  addrConflictStats_t		networkPortConflictStats;
  simIPAddrConflictCB_t         simIPAddrConflictCallback;

 /* L7_TRUE if free memory threshold crossed trap has already been sent */
  L7_BOOL simFreeMemoryTrapSent;

  /* L7_ENABLE or L7_DISABLE */
  L7_uint32 networkPortStatus;

  /* Bit mask of NIM events that IP MAP is registered to receive */
  PORTEVENT_MASK_t nimEventMask;


} simOperInfo_t;


typedef struct
{
  L7_fileHdr_t   cfgHdr;

  char           systemName[L7_SYS_SIZE];
  char           systemLocation[L7_SYS_SIZE];
  char           systemContact[L7_SYS_SIZE];

  /* Configured service port IPv4 address and gateway */
  L7_uint32      servPortIpAddress;
  L7_uint32      servPortNetMask;
  L7_uint32      servPortGateway;

  simCfgIpv6IfData_t  servPortIpv6Intfs[FD_SIM_MAX_INTF_IPV6_ADDRS];
  L7_in6_addr_t  servPortIpv6Gateway;
  L7_uint32      servPortIpv6AdminMode;

  /* Configured network port IPv4 address and gateway */
  L7_uint32      systemIpAddress;
  L7_uint32      systemNetMask;
  L7_uint32      systemGateway;

  simCfgIpv6IfData_t  systemIpv6Intfs[FD_SIM_MAX_INTF_IPV6_ADDRS];
  L7_in6_addr_t  systemIpv6Gateway;
  L7_uint32      systemIpv6AdminMode;

  /* MAC address removed from structure to avoid clearing on management */
  /* changeover. Place holder left to avoid config migration            */
  L7_char8       unusedBIA[L7_MAC_ADDR_LEN];
  L7_char8       systemLAA[L7_MAC_ADDR_LEN];
  L7_uint32      systemMacType;                /* SIM_MACTYPE_BIA or SIM_MACTYPE_LAA */

  L7_uint32      systemConfigMode;             /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP or L7_SYSCONFIG_MODE_BOOTP */
  L7_uint32      servPortConfigMode;           /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP or L7_SYSCONFIG_MODE_BOOTP */

  L7_uint32      systemIPv6ConfigMode;         /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP */
  L7_uint32      servPortIPv6ConfigMode;       /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP */

  L7_uint32      systemIPv6AddrAutoConfig;     /* If IPv6 Address Autoconfiguration is enabled on network port */
  L7_uint32      servPortIPv6AddrAutoConfig;   /* If IPv6 Address Autoconfiguration is enabled on service port */

  L7_uint32      systemBaudRate;
  L7_uint32      serialPortTimeOut;
  L7_uint32      serialPortStopBits;
  L7_uint32      serialPortParityType;
  L7_uint32      serialPortFlowControl;
  L7_uint32      serialPortCharSize;

  L7_uint32      systemTransferUnitNumber;
  L7_uint32      systemUnitID;                 /* Unit ID in stack */

  L7_uint32      systemMgmtVlanId;             /* Management VLAN ID */

  simTimeZone_t  systemTimeZone;
  summerTimeCfgData_t   summerTime;

  L7_uint32      systemMgmtPortNum;            /* Management Port */
  L7_uint32      serialHistoryBufferSize;
  L7_uint32      telnetHistoryBufferSize;
  L7_uint32      sshHistoryBufferSize;
  L7_uint32      serialHistoryBufferEnable;
  L7_uint32      telnetHistoryBufferEnable;
  L7_uint32      sshHistoryBufferEnable;

  L7_uint32      servPortAdminState;

  simCpuMemoryMonitorData_t memoryMonitorData;
 
  simCpuUtilMonitorData_t   utilMonitorData;

  L7_uint32      checkSum;                     /* check sum of config file NOTE: needs to be last entry */

} simCfgData_t;

typedef struct simDebugCfgData_s
{
  L7_BOOL pingDebugPacketTraceFlag;
} simDebugCfgData_t;

typedef struct simDebugCfg_s
{
  L7_fileHdr_t          hdr;
  simDebugCfgData_t  cfg;
  L7_uint32             checkSum;
} simDebugCfg_t;


/****************************************
*
*  SIM Internal function defines
*
*****************************************/

L7_RC_t simSave(void);
L7_BOOL simHasDataChanged(void);
void simResetDataChanged(void);
void simBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t simApplyConfigData(void);
L7_RC_t simCnfgrUnconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason );

void simAddrConflictDetectApply();
L7_RC_t simServPortIPAddrApply(void);
L7_RC_t simServPortOperAddrSet(L7_uint32 ipAddr, L7_uint32 netMask, 
                               e_simMgmtIpAddrType addrType);
L7_RC_t simServPortOperGatewaySet(L7_uint32 newGateway);
L7_RC_t simServPortGatewayApply(void);

L7_RC_t simSystemIPAddrApply(void);
L7_RC_t simSystemOperGatewaySet(L7_uint32 newGateway);
L7_RC_t simSystemGatewayApply(void);
L7_RC_t simSystemOperAddrSet(L7_uint32 ipAddr, L7_uint32 netMask, 
                             e_simMgmtIpAddrType addrType);

void summerTimeBuildDefaultConfigData(L7_uint32 ver);

L7_RC_t simIPAddrConflictDetectIPMacSet(L7_uint32 conflictIP, L7_uchar8 *conflictMAC);

L7_RC_t simVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum,
                              L7_uint32 event);
L7_RC_t simIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);
void simIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase);
L7_RC_t simIntfCreateStartup(void);
L7_RC_t simIntfActivateStartup(void);
void simNetworkPortStatusUpdate(void);
void simNetworkPortEnable(void);
void simNetworkPortDisable(void);

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    ver   Expected software version of Config Data
* @param    buffer Outdated config buffer
*
* @returns  void
*
* @notes    Current implementation resets to factory default.  Future
*           version could move fields of prior version into new structure.
*
* @end
*********************************************************************/
void simMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 * buffer);

L7_RC_t simDebugSave(void);
L7_RC_t simDebugRestore(void);
L7_BOOL simDebugHasDataChanged(void);
void simDebugBuildDefaultConfigData(L7_uint32 ver);
L7_RC_t simApplyDebugConfigData(void);
L7_RC_t simGetNextMgmtPortIPV6Ndp( L7_uchar8 *ifName,
                               L7_in6_addr_t   *ipv6NetAddress,
                               L7_uint32       *ipv6PhysAddrLen,
                               L7_uchar8       *ipv6PhysAddress,
                               L7_uint32       *ipv6Type,
                               L7_uint32       *ipv6State,
                               L7_uint32       *ipv6LastUpdated,
                               L7_BOOL         *ipv6IsRtr);
L7_RC_t simServPortIPV6AddressConflictCheck(L7_in6_addr_t *ip6Addr, 
                                            L7_uint32 ip6PrefixLen,
                                            L7_BOOL prefixCheckOnly);
L7_RC_t simSystemIPV6AddressConflictCheck(L7_in6_addr_t *ip6Addr, 
                                          L7_uint32 ip6PrefixLen,
                                            L7_BOOL prefixCheckOnly);

void simAddrChangeEventNotify(SIM_ADDR_CHANGE_TYPE_t  addrType,
                              L7_uchar8               addrFamily);;

void simIPAddrConflictNotify(L7_MGMT_PORT_TYPE_t portType,
                             L7_uint32 ipAddr,
                             L7_uchar8 *macAddr);

void simCpuMonitorUnconfigure();

#ifdef FEAT_METRO_CPE_V1_0
void snmpStartupConfigNotificationProcess(void);
L7_BOOL simStartupConfigIsExist(void);
#endif

#endif /* INCLUDE_SIM */
