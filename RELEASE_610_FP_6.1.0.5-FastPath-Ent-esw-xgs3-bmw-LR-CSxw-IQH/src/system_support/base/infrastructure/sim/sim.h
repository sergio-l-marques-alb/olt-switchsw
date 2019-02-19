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

/*
*  used for checksum operation to remove dataChanged and checksum
*  fields in the nimConfiData_t structure
*/
/*

*/
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

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  char           systemName[L7_SYS_SIZE];
  char           systemLocation[L7_SYS_SIZE];
  char           systemContact[L7_SYS_SIZE];

  L7_uint32      servPortIpAddress;
  L7_uint32      servPortNetMask;
  L7_uint32      servPortGateway;

  simCfgIpv6IfData_t  servPortIpv6Intfs[FD_SIM_MAX_INTF_IPV6_ADDRS];
  L7_in6_addr_t  servPortIpv6Gateway;
  L7_uint32      servPortIpv6AdminMode;

  L7_uint32      systemIpAddress;
  L7_uint32      systemNetMask;
  L7_uint32      systemGateway;

  simCfgIpv6IfData_t  systemIpv6Intfs[FD_SIM_MAX_INTF_IPV6_ADDRS];
  L7_in6_addr_t  systemIpv6Gateway;
  L7_uint32      systemIpv6AdminMode;

  L7_char8       systemBIA[L7_MAC_ADDR_LEN];
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

void summerTimeBuildDefaultConfigData(L7_uint32 ver);
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

#ifdef FEAT_METRO_CPE_V1_0
void snmpStartupConfigNotificationProcess(void);
L7_BOOL simStartupConfigIsExist(void);
#endif

#endif /* INCLUDE_SIM */
