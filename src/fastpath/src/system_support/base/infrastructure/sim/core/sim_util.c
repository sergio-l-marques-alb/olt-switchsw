/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename sim_util.c
 *
 * @purpose System Interface Manager Utilities File
 *
 * @component sim
 *
 * @comments none
 *
 * @create 08/21/2000
 *
 * @author bmutz
 *
 * @end
 *
 **********************************************************************/

#include <string.h>                /* for memcpy() etc... */
#include "l7_common.h"
#include "l7netapi.h"
#include "nvstoreapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "l7_cnfgr_api.h"
#include "nimapi.h"
#include "simapi.h"
#include "sim.h"
#include "sysapi.h"
#include "l7_product.h"
#include "log.h"
#include "nvstoreapi.h"
#include "fdb_api.h"
#include "usmdb_util_api.h"
#include "rng_api.h"
#include "usmdb_sim_api.h"
#include "dhcp_bootp_api.h"
#include "dtlapi.h"
#include "ipstats_api.h"
#include "ping_api.h"
#include "traceroute_map_api.h"
#include "dhcp_cnfgr.h"
#include "bootp_cnfgr.h"
#include "dhcp6c_api.h"
#include "l7_tftp.h"
#include "ipv6_commdefs.h"
#include "sim_pts_api.h"
#include "ms_api.h"
#include "bspapi.h"
#include "cli_txt_cfg_api.h"
#include "dhcp_client_api.h"
#include "mirror_api.h"
#include "dot3ad_api.h"


#ifdef L7_NSF_PACKAGE
  #include "sim_ckpt.h"
#endif

extern simCfgData_t        simCfgData;
extern simOperInfo_t     *simOperInfo;
extern summerTimeCfgData_t summerTimeCfgData;
extern simTransferInfo_t   simTransferInfo;
extern L7_BOOL        transferInProgress;
extern L7_BOOL        suspendMgmtAccess;
extern void          *transferContext;
extern simAddrChangeNotifyList_t simAddrChangeNotifyList[SIM_ADDR_CHANGE_REGISTRATIONS_MAX];

extern void* simImageSemaphore;

L7_int32 L7_transfer_task_id;
L7_int32 addrConflictTaskId;

simOperInfo_t     *simOperInfo = NULL;

/* Flag to indicate whether baud rate needs to be overwritten.
 */
static L7_BOOL OverrideBaudRate = L7_FALSE;
static L7_BAUDRATES_t NewBaudRate;

extern simDebugCfg_t simDebugCfg;    /* Debug Configuration File Overlay */

extern void simDebugRegister(void);
extern void simDebugCfgRead(void);

L7_RC_t simCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t simCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t simCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t simCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason );

/* Storage for a learned route same as servPort and netPort */
simRouteStorage_t  * servPortRoutingProtRoute;
simRouteStorage_t  * netPortRoutingProtRoute;

#ifdef FEAT_METRO_CPE_V1_0
extern L7_ulong32 bootpDhcpServerAddress;
static osapiTimerDescr_t *t_startupConfigNotify = L7_NULLPTR;
extern L7_RC_t SnmpTrapMgrStatusSet(L7_uint32 index, L7_uint32 status);
extern L7_RC_t SnmpTrapMgrCommunitySet(L7_uint32 index, L7_char8 *commName);
extern L7_RC_t SnmpTrapMgrIpAddrSet(L7_uint32 index, L7_uint32 ipAddr);
extern L7_RC_t SnmpTrapMgrVersionSet(L7_uint32 index, snmpTrapVer_t version);
extern L7_RC_t trapMgrNoStartupConfigLogTrap(void);
extern L7_RC_t SnmpTrapMgrCommunityGet(L7_uint32 index, L7_char8 *commName);
extern L7_RC_t SnmpTrapMgrIpAddrGet(L7_uint32 index, L7_uint32 *ipAddr);
#endif

/*********************************************************************
 * @purpose  Saves sim user config file to NVStore
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t simSave(void)
{
  if (simCfgData.cfgHdr.dataChanged == L7_TRUE)
  {
    simCfgData.cfgHdr.dataChanged = L7_FALSE;

    simCfgData.checkSum = nvStoreCrc32((L7_char8 *)&simCfgData,
                                       (sizeof (simCfgData) - sizeof (simCfgData.checkSum)));

    /* call save NVStore routine */
    if (sysapiCfgFileWrite(L7_SIM_COMPONENT_ID, SIM_CFG_FILENAME, (L7_char8 *)&simCfgData,
                           sizeof (simCfgData)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
              "Error on call to sysapiCfgFileWrite routine on config file %s\n",SIM_CFG_FILENAME);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Deletes all network port ipv6 addresses
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void simDeleteSystemConfiguredIpv6Addr()
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uint32 prefixLen, euiFlag, nextPrefixLen, nextEuiFlag;
  L7_in6_addr_t ipAddr, nextIpAddr;
  L7_BOOL last = L7_FALSE;
  L7_RC_t rc;
  L7_uchar8 address[IPV6_DISP_ADDR_LEN];

  memset(&ipAddr, 0, sizeof(ipAddr));
  rc = simGetNextSystemIPV6Prefix( &ipAddr, &prefixLen, &euiFlag );
  if (rc == L7_SUCCESS)
  {
    osapiInetNtop (L7_AF_INET6,
                   (L7_uchar8 *)&ipAddr,
                   address, IPV6_DISP_ADDR_LEN);

    memcpy(&nextIpAddr, &ipAddr, sizeof(L7_in6_addr_t));
    nextPrefixLen = prefixLen;
    nextEuiFlag = euiFlag;

    last = L7_FALSE;
    while (!last)
    {
      if (simGetNextSystemIPV6Prefix( &nextIpAddr,
                                      &nextPrefixLen, &nextEuiFlag ) != L7_SUCCESS)
      {
        last = L7_TRUE;
      }

      if (simDeleteSystemIPV6Prefix(&ipAddr, prefixLen, euiFlag)
          != L7_SUCCESS)
      {
        break;
      }
      memcpy(&ipAddr, &nextIpAddr, sizeof(L7_in6_addr_t));
      prefixLen = nextPrefixLen;
      euiFlag = nextEuiFlag;
    }
  }
  /* Delete network port ipv6 gateway address */
  memset(&ipAddr, 0, sizeof(ipAddr));
  simSetSystemIPV6Gateway(&ipAddr, L7_FALSE);
#endif
  return;
}

/*********************************************************************
 * @purpose  Delete all configured serviceport ipv6 addresses and gateway
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
void simDeleteServPortConfiguredIpv6Addr()
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uint32 prefixLen, euiFlag, nextPrefixLen, nextEuiFlag;
  L7_in6_addr_t ipAddr, nextIpAddr;
  L7_BOOL last = L7_FALSE;
  L7_RC_t rc;

  memset(&ipAddr, 0, sizeof(ipAddr));
  rc = simGetNextServPortIPV6Prefix( &ipAddr, &prefixLen, &euiFlag );
  if (rc == L7_SUCCESS)
  {
    memcpy(&nextIpAddr, &ipAddr, sizeof(L7_in6_addr_t));
    nextPrefixLen = prefixLen;
    nextEuiFlag = euiFlag;

    last = L7_FALSE;
    while (!last)
    {
      if (simGetNextServPortIPV6Prefix( &nextIpAddr,
                                        &nextPrefixLen, &nextEuiFlag ) != L7_SUCCESS)
      {
        last = L7_TRUE;
      }

      if (simDeleteServPortIPV6Prefix(&ipAddr, prefixLen, euiFlag)
          != L7_SUCCESS)
      {
        break;
      }
      memcpy(&ipAddr, &nextIpAddr, sizeof(L7_in6_addr_t));
      prefixLen = nextPrefixLen;
      euiFlag = nextEuiFlag;
    }
  }
  /* Delete serviceport ipv6 gateway address */
  memset(&ipAddr, 0, sizeof(ipAddr));
  simSetServPortIPV6Gateway(&ipAddr, L7_FALSE);
#endif
  return;
}

/*********************************************************************
 * @purpose  Checks if sim user config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL simHasDataChanged(void)
{
  return simCfgData.cfgHdr.dataChanged;
}

void simResetDataChanged(void)
{
  simCfgData.cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
 * @purpose  Start tasks for cli, web & snmp. Then start cli & web tasks.
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_uint32 simStartTasks(void)
{
  /*  sysapiTimerTaskStart(); */
  L7_transfer_task_id = osapiTaskCreate("TransferTask", L7_transfer_task, 0, 0,
                                        L7_DEFAULT_STACK_SIZE,
                                        L7_DEFAULT_TASK_PRIORITY,
                                        L7_DEFAULT_TASK_SLICE);
  if (L7_transfer_task_id == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
            "Failed to Create Transfer Task.\n");
  }

  addrConflictTaskId = osapiTaskCreate("SimAddrConflictTask", simAddrConflictTask, 0, 0,
                                       L7_DEFAULT_STACK_SIZE,
                                       L7_DEFAULT_TASK_PRIORITY,
                                       L7_DEFAULT_TASK_SLICE);
  if (addrConflictTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
            "Failed to Create Address Conflict Detection Task.\n");
  }

  bootpCnfgrInitPhase1Process();

  pingCnfgrInitPhase1Process();
  traceRouteCnfgrInitPhase1Process();

  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  Print the current System Interface Manager config values to
 *           serial port
 *
 * @param    parmType    Describes 2nd parm (filename or structure)
 * @param    buffer      pointer to structure of filename
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t simConfigDump(void)
{
  L7_RC_t rc=L7_SUCCESS;
  L7_char8 buf[40];
  L7_uchar8 address[IPV6_DISP_ADDR_LEN];
  L7_uint32 i = 0;

  printf ("System Name - %s\n",simCfgData.systemName);
  printf ("System Location - %s\n",simCfgData.systemLocation);
  printf ("System systemContact - %s\n",simCfgData.systemContact);

  osapiInetNtoa (simCfgData.servPortIpAddress,buf);
  printf ("Service Port IP - %s\n",buf);
  osapiInetNtoa (simCfgData.servPortNetMask,buf);
  printf ("Service Port NetMask - %s\n",buf);
  osapiInetNtoa (simCfgData.servPortGateway,buf);
  printf ("Service Port Gateway - %s\n",buf);

  osapiInetNtoa (simCfgData.systemIpAddress,buf);
  printf ("System IP - %s\n",buf);
  osapiInetNtoa (simCfgData.systemNetMask,buf);
  printf ("System NetMask - %s\n",buf);
  osapiInetNtoa (simCfgData.systemGateway,buf);
  printf ("System Gateway - %s\n",buf);

  sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X",
          0xFF & systemBIA[0], 0xFF & systemBIA[1],
          0xFF & systemBIA[2], 0xFF & systemBIA[3],
          0xFF & systemBIA[4], 0xFF & systemBIA[5]);
  printf ("System Burned-in MAC Address - %s\n",buf);
  sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X",
          0xFF & simCfgData.systemLAA[0],0xFF & simCfgData.systemLAA[1],
          0xFF & simCfgData.systemLAA[2],0xFF & simCfgData.systemLAA[3],
          0xFF & simCfgData.systemLAA[4],0xFF & simCfgData.systemLAA[5]);
  printf ("System Locally Administered MAC Address - %s\n",buf);
  switch (simCfgData.systemMacType)
  {
    case L7_SYSMAC_BIA:
      sprintf (buf,"%s","L7_SYSMAC_BIA");
      break;
    case L7_SYSMAC_LAA:
      sprintf (buf,"%s","L7_SYSMAC_LAA");
      break;
  }
  printf("System MAC Type - %s\n",buf);

  switch (simCfgData.systemConfigMode)
  {
    case L7_SYSCONFIG_MODE_NONE:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_NONE");
      break;
    case L7_SYSCONFIG_MODE_BOOTP:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_BOOTP");
      break;
    case L7_SYSCONFIG_MODE_DHCP:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_DHCP");
      break;
  }
  printf ("Network Protocol - %s\n",buf);

  switch (simCfgData.systemIPv6ConfigMode)
  {
    case L7_SYSCONFIG_MODE_NONE:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_NONE");
      break;
    case L7_SYSCONFIG_MODE_DHCP:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_DHCP");
      break;
  }
  printf ("Network Port IPv6 Protocol - %s\n",buf);

  switch (simCfgData.systemIPv6AddrAutoConfig)
  {
    case L7_ENABLE:
      sprintf (buf,"%s","Enabled");
      break;
    case L7_DISABLE:
      sprintf (buf,"%s","Disabled");
      break;
  }
  printf ("Network Port IPv6 AutoConfig Mode - %s\n",buf);

  printf("Network IPv6 Admin Mode - %s\n",
         ((simCfgData.systemIpv6AdminMode==L7_ENABLE)?"ENABLED":"DISABLED"));

  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Intfs[i].ip6Addr))
    {
      memset(address, 0, sizeof(address));
      osapiInetNtop (L7_AF_INET6,
                     (L7_uchar8 *)&simCfgData.systemIpv6Intfs[i].ip6Addr,
                     address, IPV6_DISP_ADDR_LEN);
      printf("Network IPv6 Prefix - %s/%d %s\n",
             address,simCfgData.systemIpv6Intfs[i].ip6PrefixLen,
             ((simCfgData.systemIpv6Intfs[i].eui_flag!=0)?"eui":""));
    }
  }

  if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.systemIpv6Gateway))
  {
    memset(address, 0, sizeof(address));
    osapiInetNtop (L7_AF_INET6,
                   (L7_uchar8 *)&simCfgData.systemIpv6Gateway,
                   address, IPV6_DISP_ADDR_LEN);
    printf("Network IPv6 Gateway - %s\n", address);
  }

  switch (simCfgData.servPortAdminState)
  {
    case L7_ENABLE:
      sprintf(buf,"%s","L7_ENABLE");
      break;
    case L7_DISABLE:
      sprintf(buf,"%s","L7_DISABLE");
      break;
  }
  printf("Service Port Admin State - %s\n",buf);

  switch (simCfgData.servPortConfigMode)
  {
    case L7_SYSCONFIG_MODE_NONE:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_NONE");
      break;
    case L7_SYSCONFIG_MODE_BOOTP:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_BOOTP");
      break;
    case L7_SYSCONFIG_MODE_DHCP:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_DHCP");
      break;
  }
  printf ("Service Port Protocol - %s\n",buf);

  switch (simCfgData.servPortIPv6ConfigMode)
  {
    case L7_SYSCONFIG_MODE_NONE:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_NONE");
      break;
    case L7_SYSCONFIG_MODE_DHCP:
      sprintf (buf,"%s","L7_SYSCONFIG_MODE_DHCP");
      break;
  }
  printf ("Service Port IPv6 Protocol - %s\n",buf);

  switch (simCfgData.servPortIPv6AddrAutoConfig)
  {
    case L7_ENABLE:
      sprintf (buf,"%s","Enabled");
      break;
    case L7_DISABLE:
      sprintf (buf,"%s","Disabled");
      break;
  }
  printf ("Service Port IPv6 AutoConfig Mode - %s\n",buf);

  printf("Service Port IPv6 Admin Mode - %s\n",
         ((simCfgData.servPortIpv6AdminMode==L7_ENABLE)?"ENABLED":"DISABLED"));

  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
    {
      memset(address, 0, sizeof(address));
      osapiInetNtop (L7_AF_INET6,
                     (L7_uchar8 *)&simCfgData.servPortIpv6Intfs[i].ip6Addr,
                     address, IPV6_DISP_ADDR_LEN);
      printf("Service Port IPv6 Prefix - %s/%d %s\n",
             address,simCfgData.servPortIpv6Intfs[i].ip6PrefixLen,
             ((simCfgData.servPortIpv6Intfs[i].eui_flag!=0)?"eui":""));
    }
  }

  if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
  {
    memset(address, 0, sizeof(address));
    osapiInetNtop (L7_AF_INET6,
                   (L7_uchar8 *)&simCfgData.servPortIpv6Gateway,
                   address, IPV6_DISP_ADDR_LEN);
    printf("Service Port IPv6 Gateway - %s\n", address);
  }

  switch (simCfgData.systemBaudRate)
  {
    case L7_BAUDRATE_1200:
      sprintf(buf,"%s","L7_BAUDRATE_1200");
      break;
    case L7_BAUDRATE_2400:
      sprintf(buf,"%s","L7_BAUDRATE_2400");
      break;
    case L7_BAUDRATE_4800:
      sprintf(buf,"%s","L7_BAUDRATE_4800");
      break;
    case L7_BAUDRATE_9600:
      sprintf(buf,"%s","L7_BAUDRATE_9600");
      break;
    case L7_BAUDRATE_19200:
      sprintf(buf,"%s","L7_BAUDRATE_19200");
      break;
    case L7_BAUDRATE_38400:
      sprintf(buf,"%s","L7_BAUDRATE_38400");
      break;
    case L7_BAUDRATE_57600:
      sprintf(buf,"%s","L7_BAUDRATE_57600");
      break;
    case L7_BAUDRATE_115200:
      sprintf(buf,"%s","L7_BAUDRATE_115200");
      break;
  }
  printf("System Baud Rate - %s\n",buf);

  printf("Serial Port Timeout - %d\n",simCfgData.serialPortTimeOut);

  switch (simCfgData.serialPortStopBits)
  {
    case L7_STOP_BIT_1:
      sprintf(buf,"%s","L7_STOP_BIT_1");
      break;
    case L7_STOP_BIT_2:
      sprintf(buf,"%s","L7_STOP_BIT_2");
      break;
  }
  printf("Serial Port Stop Bits - %s\n",buf);

  switch (simCfgData.serialPortParityType)
  {
    case L7_PARITY_EVEN:
      sprintf(buf,"%s","L7_PARITY_EVEN");
      break;
    case L7_PARITY_ODD:
      sprintf(buf,"%s","L7_PARITY_ODD");
      break;
    case L7_PARITY_NONE:
      sprintf(buf,"%s","L7_PARITY_NONE");
      break;
  }
  printf("Serial Port Parity Type - %s\n",buf);

  switch (simCfgData.serialPortFlowControl)
  {
    case L7_ENABLE:
      sprintf(buf,"%s","L7_ENABLE");
      break;
    case L7_DISABLE:
      sprintf(buf,"%s","L7_DISABLE");
      break;
  }
  printf("Serial Port Flow Control Mode - %s\n",buf);

  printf("Serial Port Char Size - %d\n",simCfgData.serialPortCharSize);

  switch (simTransferInfo.systemTransferMode)
  {
    case L7_TRANSFER_XMODEM:
      sprintf(buf,"%s","L7_TRANSFER_XMODEM");
      break;
    case L7_TRANSFER_TFTP:
      sprintf(buf,"%s","L7_TRANSFER_TFTP");
      break;
    case L7_TRANSFER_YMODEM:
      sprintf(buf,"%s","L7_TRANSFER_YMODEM");
      break;
    case L7_TRANSFER_ZMODEM:
      sprintf(buf,"%s","L7_TRANSFER_ZMODEM");
      break;
  }
  printf("System Transfer Mode - %s\n",buf);

  memset(address, 0, sizeof(address));
  osapiInetNtop ((L7_uint32)simTransferInfo.systemTransferServerIp.family,
                 (L7_uchar8 *)&(simTransferInfo.systemTransferServerIp.addr),
                 address, sizeof(address));
  printf("System Transfer Server IP - %s\n",address);
  printf("System Transfer File Path - %s\n",simTransferInfo.systemTransferFilePathRemote);
  printf("System Transfer File Name - %s\n",simTransferInfo.systemTransferFileNameRemote);

  switch (simTransferInfo.systemUploadFileType)
  {
    case L7_FILE_TYPE_CODE:
      sprintf(buf,"%s","L7_FILE_TYPE_CODE");
      break;
    case L7_FILE_TYPE_CONFIG:
      sprintf(buf,"%s","L7_FILE_TYPE_CONFIG");
      break;
    case L7_FILE_TYPE_TXTCFG:
      sprintf(buf,"%s","L7_FILE_TYPE_TXTCFG");
      break;
    case L7_FILE_TYPE_ERRORLOG:
      sprintf(buf,"%s","L7_FILE_TYPE_ERRORLOG");
      break;
    case L7_FILE_TYPE_SYSTEM_MSGLOG:
      sprintf(buf,"%s","L7_FILE_TYPE_SYSTEM_LOG");
      break;
    case L7_FILE_TYPE_TRAP_LOG:
      sprintf(buf,"%s","L7_FILE_TYPE_TRAP_LOG");
      break;
    case L7_FILE_TYPE_TRACE_DUMP:
      sprintf(buf,"%s","L7_FILE_TYPE_TRACE_DUMP");
      break;
  }
  printf("System Upload File Type - %s\n",buf);

  switch (simTransferInfo.systemDownloadFileType)
  {
    case L7_FILE_TYPE_CODE:
      sprintf(buf,"%s","L7_FILE_TYPE_CODE");
      break;
    case L7_FILE_TYPE_CONFIG:
      sprintf(buf,"%s","L7_FILE_TYPE_CONFIG");
      break;
    case L7_FILE_TYPE_TXTCFG:
      sprintf(buf,"%s","L7_FILE_TYPE_TXTCFG");
      break;
    case L7_FILE_TYPE_ERRORLOG:
      sprintf(buf,"%s","L7_FILE_TYPE_ERRORLOG");
      break;
    case L7_FILE_TYPE_SYSTEM_MSGLOG:
      sprintf(buf,"%s","L7_FILE_TYPE_SYSTEM_LOG");
      break;
    case L7_FILE_TYPE_TRAP_LOG:
      sprintf(buf,"%s","L7_FILE_TYPE_TRAP_LOG");
      break;
    case L7_FILE_TYPE_TRACE_DUMP:
      sprintf(buf,"%s","L7_FILE_TYPE_TRACE_DUMP");
      break;
    case L7_FILE_TYPE_AUTO_INSTALL_SCRIPT:
      sprintf(buf,"%s","L7_FILE_TYPE_AUTO_INSTALL_SCRIPT");
      break;
  }
  printf("System Download File Type - %s\n",buf);

  printf("Transfer Status - %d\n", transferInProgress);
  printf("Suspend Mgmt Access - %d\n", suspendMgmtAccess);

  printf("System Unit ID - %d\n",simCfgData.systemUnitID);

  printf("System VLAN ID - %d\n",simCfgData.systemMgmtVlanId);
  printf("Management Port- %d\n",simCfgData.systemMgmtPortNum);

  printf("Scaling Constants\n");
  printf("-----------------\n");

  printf("L7_SYS_SIZE - %d\n", L7_SYS_SIZE);
  printf("L7_MAX_FILENAME - %d\n", L7_MAX_FILENAME);
  printf("L7_MAC_ADDR_LEN - %d\n", L7_MAC_ADDR_LEN);

  /* timezone details */
  printf ("TIMEZONE \r\n ---------------\r\n");
  printf ("\r\nTimeZoneName :%s", simCfgData.systemTimeZone.tzName);
  printf ("\r\n hours :%u", simCfgData.systemTimeZone.hour);
  printf ("\r\n minutes :%u", simCfgData.systemTimeZone.minutes);

  /* summer time details */
  printf ("SUMMERTIME \r\n ---------------\r\n");

  printf ("\r\n mode :%u", simCfgData.summerTime.mode);
  printf ("\r\n persistMode :%u", simCfgData.summerTime.persistMode);
  printf ("\r\nstTimeInSeconds :%u", simCfgData.summerTime.stTimeInSeconds);
  printf ("\r\nendTimeInSeconds :%u", simCfgData.summerTime.endTimeInSeconds );

  printf ("\r\n -- recurring --\r\n");
  printf ("\r\n start_month :%u",simCfgData.summerTime.nonrecurring.start_month);
  printf ("\r\n start_date :%u",simCfgData.summerTime.nonrecurring.start_date);
  printf ("\r\n start_year :%u",simCfgData.summerTime.nonrecurring.start_year);
  printf ("\r\n start_time :%u",simCfgData.summerTime.nonrecurring.start_time);
  printf ("\r\n end_month :%u",simCfgData.summerTime.nonrecurring.end_month);
  printf ("\r\n end_date :%u",simCfgData.summerTime.nonrecurring.end_date);
  printf ("\r\n end_year :%u",simCfgData.summerTime.nonrecurring.end_year);
  printf ("\r\n end_time :%u",simCfgData.summerTime.nonrecurring.end_time);
  printf ("\r\n offset_minutes :%u", simCfgData.summerTime.nonrecurring.offset_minutes);
  printf ("\r\n zone :%s", simCfgData.summerTime.nonrecurring.zone);

  printf ("\r\n -- non-recurring --\r\n");
  printf ("\r\n start_week :%u",simCfgData.summerTime.recurring.start_week);
  printf ("\r\n start_day :%u",simCfgData.summerTime.recurring.start_day);
  printf ("\r\n start_month:%u",simCfgData.summerTime.recurring.start_month);
  printf ("\r\n start_time :%u",simCfgData.summerTime.recurring.start_time);
  printf ("\r\n end_week :%u",simCfgData.summerTime.recurring.end_week);
  printf ("\r\n end_day :%u",simCfgData.summerTime.recurring.end_day);
  printf ("\r\n end_month:%u",simCfgData.summerTime.recurring.end_month);
  printf ("\r\n end_time :%u",simCfgData.summerTime.recurring.end_time);
  printf ("\r\n offset_minutes :%u", simCfgData.summerTime.recurring.offset_minutes);
  printf ("\r\n zone :%s", simCfgData.summerTime.recurring.zone);

  printf("\r\n CPU Free memory monitor threshold :%u\n", 
                             simCfgData.memoryMonitorData.freeMemoryThreshold);
  printf("\r\n CPU Util monitor: Rising threshold :%u Rising Period :%u\n", 
                             simCfgData.utilMonitorData.risingThreshold,
                             simCfgData.utilMonitorData.risingPeriod);
  printf("\r\n CPU Util monitor: Falling threshold :%u Falling Period :%u\n", 
                             simCfgData.utilMonitorData.fallingThreshold,
                             simCfgData.utilMonitorData.fallingPeriod);

  return(rc);
}

L7_RC_t simConfigClockDump(void)
{
  L7_RC_t rc = L7_SUCCESS;
  /* timezone details */
  printf ("\r\n  TIMEZONE \r\n  ---------------\r\n");
  printf ("\r\nTimeZoneName :%s", simCfgData.systemTimeZone.tzName);
  printf ("\r\n hours :%d", simCfgData.systemTimeZone.hour);
  printf ("\r\n minutes :%u", simCfgData.systemTimeZone.minutes);

  /* summer time details */
  printf ("\r\n  SUMMERTIME \r\n  ---------------\r\n");

  printf ("\r\n mode :%u", simCfgData.summerTime.mode);
  printf ("\r\n persistMode :%u", simCfgData.summerTime.persistMode);
  printf ("\r\nstTimeInSeconds :%u", simCfgData.summerTime.stTimeInSeconds);
  printf ("\r\nendTimeInSeconds :%u", simCfgData.summerTime.endTimeInSeconds );

  printf ("\r\n -- non-recurring --\r\n");
  printf ("\r\n start_month :%u",simCfgData.summerTime.nonrecurring.start_month);
  printf ("\r\n start_date :%u",simCfgData.summerTime.nonrecurring.start_date);
  printf ("\r\n start_year :%u",simCfgData.summerTime.nonrecurring.start_year);
  printf ("\r\n start_time :%u",simCfgData.summerTime.nonrecurring.start_time);
  printf ("\r\n end_month :%u",simCfgData.summerTime.nonrecurring.end_month);
  printf ("\r\n end_date :%u",simCfgData.summerTime.nonrecurring.end_date);
  printf ("\r\n end_year :%u",simCfgData.summerTime.nonrecurring.end_year);
  printf ("\r\n end_time :%u",simCfgData.summerTime.nonrecurring.end_time);
  printf ("\r\n offset_minutes :%u", simCfgData.summerTime.nonrecurring.offset_minutes);
  printf ("\r\n zone :%s", simCfgData.summerTime.nonrecurring.zone);

  printf ("\r\n -- recurring --\r\n");
  printf ("\r\n start_week :%u",simCfgData.summerTime.recurring.start_week);
  printf ("\r\n start_day :%u",simCfgData.summerTime.recurring.start_day);
  printf ("\r\n start_month:%u",simCfgData.summerTime.recurring.start_month);
  printf ("\r\n start_time :%u",simCfgData.summerTime.recurring.start_time);
  printf ("\r\n end_week :%u",simCfgData.summerTime.recurring.end_week);
  printf ("\r\n end_day :%u",simCfgData.summerTime.recurring.end_day);
  printf ("\r\n end_month:%u",simCfgData.summerTime.recurring.end_month);
  printf ("\r\n end_time :%u",simCfgData.summerTime.recurring.end_time);
  printf ("\r\n offset_minutes :%u", simCfgData.summerTime.recurring.offset_minutes);
  printf ("\r\n zone :%s", simCfgData.summerTime.recurring.zone);

  return (rc);
}

/*********************************************************************
 * @purpose  Tell SIM to override serial port rate with the setting
 *       passed to this function.
 *
 * @param    baud    Baud rate.
 *
 * @returns  none
 *
 * @notes    This function may be used to coordinate the baud rate
 *       between boot code and the operational code.
 *
 * @end
 *********************************************************************/
void simSerialBaudOverride (L7_BAUDRATES_t baud_rate)
{
  OverrideBaudRate = L7_TRUE;
  NewBaudRate = baud_rate;
}

/*********************************************************************
 * @purpose  Apply Sim Configuration Data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS or L7_FALIURE
 *
 * @notes    Called in p3 init. On a product using text based configuration,
 *           we have a default configuration at this point. On a system using
 *           binary configuration, we have the startup configuration at this
 *           point.
 *
 * @end
 *********************************************************************/
L7_RC_t simApplyConfigData(void)
{
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  L7_uint32 i=0;
  char ifname[32];
#endif
  L7_uchar8 null_mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  osapiBaudRateChange(simCfgData.systemBaudRate, L7_FALSE);

  /* On a warm restart, decision about what address to use gets deferred
   * until NIM activate startup. */
  if (!simOperInfo->simWarmRestart)
  {
    if (simCfgData.servPortConfigMode == L7_SYSCONFIG_MODE_NONE)
    {
      /* Service port using configured address. If non-zero, apply it. */
      if (simCfgData.servPortIpAddress)
      {
        simServPortOperAddrSet(simCfgData.servPortIpAddress,
                               simCfgData.servPortNetMask, SIM_MGMT_ADDR_CONF);
      }
      if (simCfgData.servPortGateway)
      {
        simServPortOperGatewaySet(simCfgData.servPortGateway);
      }
    }
  }
#if SERVICE_PORT_PRESENT
  else
  {
    if (sysapiServicePortDebugFlagGet() == L7_FALSE)
    {
      L7_RC_t retCode;
      L7_uchar8 servPortMac[L7_MAC_ADDR_LEN];
      simGetServicePortBurnedInMac(servPortMac);
      retCode = osapiMacAddrChange(servPortMac, bspapiServicePortNameGet(), bspapiServicePortUnitGet());
    }
  }
#endif
#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  simSetServPortIPV6AdminMode(simCfgData.servPortIpv6AdminMode);

  for (i=0; i<L7_RTR6_MAX_INTF_ADDRS; i++)
  {
    if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Intfs[i].ip6Addr))
    {
      simSetServPortIPV6Prefix(&simCfgData.servPortIpv6Intfs[i].ip6Addr,
                               simCfgData.servPortIpv6Intfs[i].ip6PrefixLen,
                               simCfgData.servPortIpv6Intfs[i].eui_flag,
                               L7_TRUE);
    }
  }

  if (!L7_IP6_IS_ADDR_UNSPECIFIED(&simCfgData.servPortIpv6Gateway))
  {
    simSetServPortIPV6Gateway(&simCfgData.servPortIpv6Gateway, L7_TRUE);
  }
#endif

  /* Set system interface MAC address. */
  if (simCfgData.systemMacType == L7_SYSMAC_LAA )
  {
    fdbSysMacDelEntry(systemBIA,
                      simCfgData.systemMgmtVlanId,
                      1, DTL_MAC_TYPE_BURNED_IN);

    osapiUpdateNetworkMac( simCfgData.systemLAA );
  }
  else
  {
    if (memcmp(simCfgData.systemLAA, null_mac, sizeof(simCfgData.systemLAA)) != 0)
    {
      fdbSysMacDelEntry(simCfgData.systemLAA,
                        simCfgData.systemMgmtVlanId,
                        1, DTL_MAC_TYPE_LOCAL_ADMIN);
    }

    osapiUpdateNetworkMac( systemBIA );
  }

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  
  if (simCfgData.servPortIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP)
  {
    dhcpv6SendCtlPacket(L7_MGMT_SERVICEPORT, L7_SYSCONFIG_MODE_DHCP,ifname,0);
  }

  /* Tell stack to configure IPv6 Address Autoconfiguration mode
   * on service port */
  sprintf(ifname, "%s%d", L7_DTL_PORT_IF, 0);
  osapiIfIpv6AddrAutoConfigSet(ifname, simCfgData.systemIPv6AddrAutoConfig);
  osapiIfIpv6AddrAutoConfigSet(bspapiServicePortNameGet(),
                               simCfgData.servPortIPv6AddrAutoConfig);
#endif

  simSetServPortAdminState(simCfgData.servPortAdminState);

  simAddrConflictDetectApply();

  return(L7_SUCCESS);
}

/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair Unconfigure Phase 2.
 *
 * @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
 *
 * @param    pReason   - @b{(output)}  Reason if L7_ERROR.
 *
 * @returns  L7_SUCCESS - There were no errors. Response is available.
 *
 * @returns  L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           L7_CNFGR_ERR_RC_FATAL
 *
 * @notes
 *
 *
 * @end
 *********************************************************************/
L7_RC_t simCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc;

  /* Free bootp session */
#ifndef L7_PRODUCT_SMARTPATH
  bootpCnfgrUconfigPhase2();

#endif

  /* Free ping and traceroute sessions */
  pingCnfgrUconfigPhase2Process();
  traceRouteCnfgrUconfigPhase2Process();

  rc = ipStatsReset();

  /* reset operational data */
  memset(simOperInfo, 0, sizeof(simOperInfo_t));
  memset(&simCfgData, 0, sizeof(simCfgData_t));

  if(rc == L7_SUCCESS)
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }
  else
  {
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    *pResponse  = 0;
  }
  return rc;
}

/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair.
 *
 * @param    pCmdData  - @b{(input)}  command to be processed.
 *
 * @returns  None
 *
 * @notes    This function completes synchronously. The return value
 *           is presented to the configurator by calling the cnfgrApiCallback().
 *           The following are the possible return codes:
 *           L7_SUCCESS - There were no errors. Response is available.
 *           L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           L7_CNFGR_CB_ERR_RC_INVALID_RQST
 *           L7_CNFGR_CB_ERR_RC_INVALID_CMD
 *           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
 *           L7_CNFGR_ERR_RC_INVALID_PAIR
 *           L7_CNFGR_ERR_RC_FATAL
 *
 * @notes    This function runs in the configurator's thread. This thread MUST NOT
 *           be blocked!
 *
 * @end
 *********************************************************************/
void simApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             simRC    = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ( request > L7_CNFGR_RQST_FIRST &&
           request < L7_CNFGR_RQST_LAST )
      {
        /* validate command/event pair */
        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                simRC = simCnfgrInitPhase1Process( &response, &reason );
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                simRC = simCnfgrInitPhase2Process( &response, &reason );
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                /* Determine if this is a cold or warm restart */
                simOperInfo->simWarmRestart = (pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM);
                simRC = simCnfgrInitPhase3Process( &response, &reason );
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                simRC = simCnfgrNoopProcess( &response, &reason );
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch (pCmdData->u.rqstData.request)
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                {
                  bootpTaskReInit();

                  bootpConfigSemGet();

                  simSystemOperAddrSet(0, 0, SIM_MGMT_ADDR_CONF);
                  simSystemOperGatewaySet(0);
                  simDeleteSystemConfiguredIpv6Addr();

                  if (cnfgrIsComponentPresent(L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)
                  {
                    simServPortOperAddrSet(0, 0, SIM_MGMT_ADDR_CONF);
                    simServPortOperGatewaySet(0);
                    simDeleteServPortConfiguredIpv6Addr();
                  }

                  /* Restore management VLAN to default setting. */
                  simMgmtVlanIdSet (FD_SIM_DEFAULT_MGMT_VLAN_ID);

                  /* Restore Cpu monitoring to default settings. */
                  simCpuMonitorUnconfigure();

                  response  = L7_CNFGR_CMD_COMPLETE;
                  reason    = 0;
                  simRC = L7_SUCCESS;
                }
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                simRC = simCnfgrUnconfigPhase2( &response, &reason );
                break;

              default:
                simRC = L7_ERROR;
                reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
                break;
            }
            break;

          case L7_CNFGR_CMD_EXECUTE:
          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
          case L7_CNFGR_CMD_RESUME:
            simRC = simCnfgrNoopProcess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */
      }
      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      } /* endif validate request */
    }
    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
    } /* endif validate command type */
  }
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
  } /* check for command valid pointer */

  /* return value to caller -
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = simRC;
  if (simRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}

/*
 *********************************************************************
 *             SIM cnfgr Internal Function Calls
 *********************************************************************
 */

/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair Init Phase 1.
 *
 * @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
 *
 * @param    pReason   - @b{(output)}  Reason if L7_ERROR.
 *
 * @returns  L7_SUCCESS - There were no errors. Response is available.
 *
 * @returns  L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           L7_CNFGR_ERR_RC_FATAL
 *
 * @notes    This function runs in the configurator's thread. This thread MUST NOT
 *           be blocked!
 *
 * @end
 *********************************************************************/
L7_RC_t simCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t simRC = L7_SUCCESS;
  L7_uint32 milliSecs;

  servPortRoutingProtRoute = (simRouteStorage_t *) osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(simRouteStorage_t));
  if (servPortRoutingProtRoute == L7_NULLPTR)
  {
    L7_LOG_ERROR (0);
  }
  memset((void *)servPortRoutingProtRoute, 0, sizeof(simRouteStorage_t));
  netPortRoutingProtRoute = (simRouteStorage_t *) osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(simRouteStorage_t));
  if (netPortRoutingProtRoute == L7_NULLPTR)
  {
    L7_LOG_ERROR (0);
  }
  memset((void *)netPortRoutingProtRoute, 0, sizeof(simRouteStorage_t));

  simOperInfo = (simOperInfo_t*) osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(simOperInfo_t));
  if (!simOperInfo)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
            "Failed to allocate memory for SIM operational data.");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_FAILURE;
  }

  simImageSemaphore = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if(L7_NULLPTR == simImageSemaphore)
  {
    L7_LOG_ERROR (0);
  }

#ifdef L7_NSF_PACKAGE
  /* Initialize the checkpoint semaphore */
  simRC = simCkptInit();
  if (simRC != L7_SUCCESS)
  {
    L7_LOG_ERROR (0);
  }
#endif

  simPtsInit();
  /* Initialize the random number generator */
  milliSecs = osapiTimeMillisecondsGet();

  randInitRNG((L7_long32)(milliSecs));

  if ((bspapiMacAddrGet((L7_char8 *)&systemBIA)) != L7_SUCCESS)
  {
    L7_LOG_ERROR (0);
  }

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  (void)simStartTasks();

  /* Initialize address change registration list */
  memset(simAddrChangeNotifyList, 0x00, sizeof(simAddrChangeNotifyList_t) *
         SIM_ADDR_CHANGE_REGISTRATIONS_MAX);

  /* Initialize OS specific IP statistics */
  if (ipStatsCreate() != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
            "simCnfgrInitPhase1Process: Error could not create ip stats\n");

  /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
  simDebugCfgRead();
  simApplyDebugConfigData();

  return (simRC);
}

/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair Init Phase 2.
 *
 * @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
 *
 * @param    pReason   - @b{(output)}  Reason if L7_ERROR.
 *
 * @returns  L7_SUCCESS - There were no errors. Response is available.
 *
 * @returns  L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           L7_CNFGR_ERR_RC_FATAL
 *
 * @notes    This function runs in the configurator's thread. This thread MUST NOT
 *           be blocked!
 *
 * @end
 *********************************************************************/
L7_RC_t simCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  nvStoreFunctionList_t notifyFunctionList;
  L7_RC_t simRC = L7_SUCCESS;
  L7_uint32 eventMask = 0;

  /*nvStoreRegister(L7_SIM_COMPONENT_ID, simSave, simRestore);*/
  (void)memset((void *) &notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID         = L7_SIM_COMPONENT_ID;
  notifyFunctionList.notifySave           = simSave;
  notifyFunctionList.hasDataChanged       = simHasDataChanged;
  notifyFunctionList.notifyConfigDump     = simConfigDump;
  notifyFunctionList.resetDataChanged = simResetDataChanged;

  simRC = nvStoreRegister(notifyFunctionList);
  if ( simRC != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    simRC = L7_ERROR;
  }
  else
  {
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  } /* endif register SIM with nvStore */

  /* register for debug */
  simDebugRegister();

  /* If network port is included in the build, register callbacks necessary to
   * manage the status of the network port. */
  if (cnfgrIsFeaturePresent(L7_SIM_COMPONENT_ID, L7_SIM_NETWORK_PORT_FEATURE_ID))
  {
    /* register NIM callback for front panel interface changes */
    if (nimRegisterIntfChange(L7_SIM_COMPONENT_ID, simIntfChangeCallback,
                              simIntfStartupCallback, NIM_STARTUP_PRIO_SIM) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_SIM_COMPONENT_ID,
              "System Interface Manager (SIM) failed to register as NIM client");
      simRC = L7_ERROR;
    }
  
    /* register callback with dot1q to get notifications for vlan delete
     * and port membership change events. Changes in the management VLAN can
     * affect the status of the network port. */
    eventMask = VLAN_DELETE_PENDING_NOTIFY | VLAN_ADD_NOTIFY | VLAN_ADD_PORT_NOTIFY | VLAN_DELETE_PORT_NOTIFY;
    if (vlanRegisterForChange(simVlanChangeCallback,
                              L7_SIM_COMPONENT_ID, eventMask) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
             "System Interface Manager (SIM) failed to register for VLAN changes.");
      simRC = L7_ERROR;
    }
  }

#ifdef L7_NSF_PACKAGE
  /* register with the checkpoint service */
  if (ckptMgrCheckpointCallbackRegister(L7_SIM_COMPONENT_ID,simCkptMgrCb) !=
      L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
           "SIM failed to register callback with Checkpoint Service.");
    simRC = L7_ERROR;
  }
  if (ckptBackupMgrCheckpointCallbackRegister(L7_SIM_COMPONENT_ID,
                                              simCkptBackupCb) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_SIM_COMPONENT_ID,
           "SIM failed to register backup callback with Checkpoint Service.");
    simRC = L7_ERROR;
  }
#endif

  /* Set the management VLAN to default setting. */
  simMgmtVlanIdSet(FD_SIM_DEFAULT_MGMT_VLAN_ID);

  /* Return Value to caller */
  return (simRC);
}

#ifdef FEAT_METRO_CPE_V1_0
/*********************************************************************
* @purpose  Checks if text-based startup-config file is present
*
* @param
*
* @returns  L7_BOOL  L7_TRUE if file exist, otherwise L7_FALSE
*
* @end
*********************************************************************/
L7_BOOL simStartupConfigIsExist()
{
  L7_int32 fileDesc;

  /* SYSAPI_TXTCFG_FILENAME is defined as "startup-config" */
  if (osapiFsOpen(SYSAPI_TXTCFG_FILENAME, &fileDesc) != L7_ERROR )
  {
    L7_uint32 startupFileLen = L7_NULL;
    L7_RC_t   rc             = L7_SUCCESS;

    osapiFsClose(fileDesc);
    rc = osapiFsFileSizeGet(SYSAPI_TXTCFG_FILENAME, &startupFileLen);
    if ((rc == L7_SUCCESS) && (startupFileLen != L7_NULL))
    {
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}
#endif

/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair Init Phase 3.
 *
 * @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
 *
 * @param    pReason   - @b{(output)}  Reason if L7_ERROR.
 *
 * @returns  L7_SUCCESS - There were no errors. Response is available.
 *
 * @returns  L7_ERROR   - There were errors. Reason code is available.
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           L7_CNFGR_ERR_RC_FATAL
 *
 * @notes    This function runs in the configurator's thread. This thread MUST NOT
 *           be blocked!
 *
 * @end
 *********************************************************************/
L7_RC_t simCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t simRC = L7_SUCCESS;
  L7_int32 fileDesc;

  simRC = sysapiCfgFileGet(L7_SIM_COMPONENT_ID, SIM_CFG_FILENAME, (L7_char8 *)&simCfgData,
                             sizeof(simCfgData), &simCfgData.checkSum, SIM_CFG_VER_CURRENT,
                             simBuildDefaultConfigData, simMigrateConfigData);

  if ( simRC != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    simRC     = L7_ERROR;
  }
  else
  {
    /* If user wants to override the baud rate then do it now.
     */
    if (OverrideBaudRate == L7_TRUE)
    {
      if (simCfgData.systemBaudRate != NewBaudRate)
      {
        simCfgData.systemBaudRate = NewBaudRate;
        simCfgData.cfgHdr.dataChanged = L7_TRUE;
      }
    }

    if (!simOperInfo->simWarmRestart)
    {
      /* On a cold restart, forget any previously learned IP addresses */
      memset(simOperInfo, 0, sizeof(simOperInfo_t));

      simRestoreSystemMac();
    }

    /* Check this order whenever new parameters added to simCfgData */

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    simRC = simApplyConfigData();
    if (simRC != L7_SUCCESS)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_FATAL;
      simRC     = L7_ERROR;
    }
    else
    {
      *pResponse = L7_CNFGR_CMD_COMPLETE;
      *pReason   = 0;
    } /* endif apply config data */

    transferInProgress        = L7_FALSE;
    suspendMgmtAccess         = L7_FALSE;
    transferContext           = L7_NULLPTR;
  } /* endif get file migrate */

  /* Tell DHCP and BOOTP tasks to start operation.
   */
  bootpConfigSemFree ();
  /* Intiialize ping and traceroute tables and make ready for operation*/
  pingCnfgrInitPhase3Process();
  traceRouteCnfgrInitPhase3Process();

#ifdef FEAT_METRO_CPE_V1_0
  snmpStartupConfigNotificationProcess();
#endif

 /* SYSAPI_TXTCFG_FILENAME is defined as "startup-config" */
  if (osapiFsOpen(SYSAPI_TXTCFG_FILENAME, &fileDesc) != L7_ERROR )
  {
    L7_uint32 startupFileLen = L7_NULL;
    L7_RC_t   rc             = L7_SUCCESS;

    osapiFsClose(fileDesc);
    rc = osapiFsFileSizeGet(SYSAPI_TXTCFG_FILENAME, &startupFileLen);
    if ((rc == L7_SUCCESS) && (startupFileLen != L7_NULL))
    {
      simOperInfo->startupConfigIsExists = L7_TRUE;
    }
  }
  else
  {
     simOperInfo->startupConfigIsExists = L7_FALSE;
  }

  /* Network port status gets set in activate startup based on state
   * of mgmt VLAN member ports. */
  simOperInfo->networkPortStatus = L7_DISABLE;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&simOperInfo->nimEventMask, 0, sizeof(simOperInfo->nimEventMask));
  nimRegisterIntfEvents(L7_SIM_COMPONENT_ID, simOperInfo->nimEventMask);

  /* Return Value to caller */
  return (simRC);
}

#ifdef FEAT_METRO_CPE_V1_0
void snmpStartupConfigNotificationProcess()
{
  L7_uint32 sshMode,ip_address;
  L7_uchar8 buf[] = "NoStartupConfig";
  L7_uchar8 name[16];

  sshdAdminModeGet(&sshMode);

  /* If startup-config exists and ssh is enabled then generate the trap */
  if (simStartupConfigIsExist() == L7_FALSE)
  {
    if ((bootpDhcpServerAddress != 0) && (sshMode == L7_ENABLE))
    {
      /* Add entry in SNMP receiver table  */
      SnmpTrapMgrStatusSet(0, L7_SNMP_TRAP_MGR_STATUS_CONFIG);
      SnmpTrapMgrCommunitySet(0, buf);
      SnmpTrapMgrIpAddrSet(0, bootpDhcpServerAddress);
      SnmpTrapMgrVersionSet(0, L7_SNMP_TRAP_VER_SNMPV2C);
      SnmpTrapMgrStatusSet(0, L7_SNMP_TRAP_MGR_STATUS_VALID);

        /* Send notification trap */
      if (trapMgrNoStartupConfigLogTrap() != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SIM_COMPONENT_ID,
                "SIM: failed to send a TRAP message\n");
      }
    }

    /* Keep checking for startup-config and ssh status. So restart the timer. */
    osapiTimerAdd((void *)snmpStartupConfigNotificationProcess, L7_NULL, L7_NULL,
                120*1000, &t_startupConfigNotify);
  }
  else
  {
    if (t_startupConfigNotify != L7_NULLPTR)
    {
      /* Delete the entry in SNMP receiver table  */
      SnmpTrapMgrCommunityGet(0, name);
      SnmpTrapMgrIpAddrGet(0, &ip_address);
      if ( (strcmp(name, buf) == 0) && (ip_address == bootpDhcpServerAddress) ) {
        SnmpTrapMgrStatusSet(0, L7_SNMP_COMMUNITY_STATUS_INVALID);
        SnmpTrapMgrStatusSet(0, L7_SNMP_TRAP_MGR_STATUS_DELETE );
        }
      /* Stop the timer */
      osapiTimerFree(t_startupConfigNotify);
      t_startupConfigNotify = L7_NULLPTR;
    }
  }

    return;
}
#endif

/*********************************************************************
 * @purpose  This function process the configurator control commands/request
 *           pair as a NOOP.
 *
 * @param    pResponse - @b{(output)}  Response always command complete.
 *
 * @param    pReason   - @b{(output)}  Always 0
 *
 * @returns  L7_SUCCESS - Always return this value. onse is available.
 *
 *
 * @notes    The following are valid response:
 *           L7_CNFGR_CMD_COMPLETE
 *
 * @notes    The following are valid error reason code:
 *           None.
 *
 * @notes    This function runs in the configurator's thread. This thread MUST NOT
 *           be blocked!
 *
 * @end
 *********************************************************************/
L7_RC_t simCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t simRC = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return (simRC);
}

/*********************************************************************
 *
 * @purpose  Trigger active address conflict detection
 *
 * @returns  void
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
void simAddrConflictDetectApply()
{
  L7_uchar8 ifName[20];

  /* Send a gratuitous ARP packet on service port and
   * network port.
  */

  if(cnfgrIsComponentPresent(L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)
  {
    osapiSnprintf(ifName, sizeof(ifName), "%s%d", bspapiServicePortNameGet(), bspapiServicePortUnitGet());
    osapiIfGratArpSend(ifName);
  }
  osapiSnprintf(ifName, sizeof(ifName), "%s0", L7_DTL_PORT_IF);
  osapiIfGratArpSend(ifName);

#ifdef L7_ROUTING_PACKAGE
  /* On routing/host interfaces */
  ipMapAddrConflictDetectApply();
#endif
}

/*********************************************************************
* @purpose  Set the system mac-address to the local burned-in mac address
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void simRestoreSystemMac(void)
{
  if ((bspapiMacAddrGet((L7_char8 *)&systemBIA)) != L7_SUCCESS)
  {
    L7_LOG_ERROR (0);
  }
}

/*********************************************************************
* @purpose  Callback function to process VLAN changes.
*
* @param    vlanData   @b{(input)}   VLAN ID
* @param    intIfnum   @b{(input)}   internal interface whose state has changed
* @param    event      @b{(input)}   VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t simVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum,
                              L7_uint32 event)
{
  /* Ignore events types we don't care about.  */
  if ((event != VLAN_DELETE_PENDING_NOTIFY) &&
      (event != VLAN_ADD_NOTIFY) &&
      (event != VLAN_ADD_PORT_NOTIFY) &&
      (event != VLAN_DELETE_PORT_NOTIFY))
  {
    return L7_SUCCESS;
  }

  /* Sanity check. Callback should not be invoked if network port is not in the 
   * build, and if network port is in the build, mgmt VLAN ID should not be 0. */
  if (simCfgData.systemMgmtVlanId == 0)
  {
    return L7_SUCCESS;
  }

  /* Ignore events for VLANs other than the management VLAN */
  if (vlanData->numVlans == 1)
  {
    if (vlanData->data.vlanId != simCfgData.systemMgmtVlanId)
    {
      return L7_SUCCESS;
    }
  }
  else if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask, 
                                simCfgData.systemMgmtVlanId) == 0)
  {
    return L7_SUCCESS;
  }

  simNetworkPortStatusUpdate();
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Process interface state changes for front panel ports.
*
* @param    intIfNum        Internal Interface Number
* @param    event           event, defined by L7_PORT_EVENTS_t
* @param    correlator      correlator for the event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Front panel ports that are members of the management VLAN
*           affect the status of the network port.
*
* @end
*********************************************************************/
L7_RC_t simIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  NIM_EVENT_COMPLETE_INFO_t status;
  L7_uint32 sysIntfType;

  status.intIfNum     = intIfNum;
  status.component    = L7_SIM_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.event        = event;
  status.correlator   = correlator;

  /* Only care about interfaces that can be VLAN members */
  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    nimEventStatusCallback(status);
    return L7_FAILURE;
  }
  if ((sysIntfType != L7_PHYSICAL_INTF) &&
      (sysIntfType != L7_LAG_INTF))
  {
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  /* Sanity check. Callback should not be invoked if network port is not in the 
   * build, and if network port is in the build, mgmt VLAN ID should not be 0. */
  if (simCfgData.systemMgmtVlanId == 0)
  {
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }

  /* If interface is a member of the mgmt VLAN, see if change causes 
   * a change in the status of the network port. */
  if (dot1qIntfIsVlanMember(simCfgData.systemMgmtVlanId, intIfNum))
  {
    simNetworkPortStatusUpdate();
  }

  nimEventStatusCallback(status);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase     create or activate
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void simIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  if (startupPhase == NIM_INTERFACE_CREATE_STARTUP)
  {
    simIntfCreateStartup();
  }
  else if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    simIntfActivateStartup();
  }
}

/*********************************************************************
* @purpose  Do NIM create startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    SIM doesn't do anything on the create startup.
*
* @end
*********************************************************************/
L7_RC_t simIntfCreateStartup(void)
{
  nimStartupEventDone(L7_SIM_COMPONENT_ID);

  if (cnfgrIsComponentPresent (L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)
  {
    if (simGetServPortConfigMode() == L7_SYSCONFIG_MODE_DHCP)
    {
      dhcpClientIPAddressMethodSet (0, L7_INTF_IP_ADDR_METHOD_DHCP,
                                    L7_MGMT_SERVICEPORT, L7_FALSE);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Do NIM activate startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t simIntfActivateStartup(void)
{
  /* Now ask NIM to send any future changes for these event types */
  PORTEVENT_SETMASKBIT(simOperInfo->nimEventMask, L7_ACTIVE);
  PORTEVENT_SETMASKBIT(simOperInfo->nimEventMask, L7_INACTIVE);
  PORTEVENT_SETMASKBIT(simOperInfo->nimEventMask, L7_LAG_ACQUIRE);
  PORTEVENT_SETMASKBIT(simOperInfo->nimEventMask, L7_PROBE_SETUP);
  PORTEVENT_SETMASKBIT(simOperInfo->nimEventMask, L7_LAG_RELEASE);
  PORTEVENT_SETMASKBIT(simOperInfo->nimEventMask, L7_PROBE_TEARDOWN);
  nimRegisterIntfEvents(L7_SIM_COMPONENT_ID, simOperInfo->nimEventMask);

  nimStartupEventDone(L7_SIM_COMPONENT_ID);

  if (simOperInfo->simWarmRestart)
  {
    if (simIsSystemAddrCheckpointed())
    {
      if (simCfgData.systemConfigMode == L7_SYSCONFIG_MODE_DHCP)
      {
        /* Apply checkpointed address on network port */
        simSystemIPAddrApply();
        simSystemGatewayApply();
      }
      else
      {
        /* Apply configured address */
        simSystemOperAddrSet(simCfgData.systemIpAddress, simCfgData.systemNetMask,
                             SIM_MGMT_ADDR_CONF);
        simSystemOperGatewaySet(simCfgData.systemGateway);
      }
    }

    if (simIsServPortAddrCheckpointed() &&
        (sysapiServicePortDebugFlagGet() == L7_FALSE))
    {
      if (simCfgData.servPortConfigMode == L7_SYSCONFIG_MODE_DHCP)
      {
        /* Apply checkpointed address on service port */
        simServPortIPAddrApply();
        simServPortGatewayApply();
      }
      else
      {
        /* Apply configured address */
        simServPortOperAddrSet(simCfgData.servPortIpAddress, simCfgData.servPortNetMask,
                               SIM_MGMT_ADDR_CONF);
        simServPortOperGatewaySet(simCfgData.servPortGateway);
      }
    }
  }
  else
  {
    /* Cold restart. Simply apply configured addresses. */
    if (simCfgData.systemConfigMode == L7_SYSCONFIG_MODE_NONE)
    {
      /* Network port configured to use configured IPv4 address */
      /* If system IP address and gateway are defined then set them. */
      if (simCfgData.systemIpAddress)
      {
        simSystemOperAddrSet(simCfgData.systemIpAddress, simCfgData.systemNetMask,
                             SIM_MGMT_ADDR_CONF);
      }
      if (simCfgData.systemGateway)
      {
        simSystemOperGatewaySet(simCfgData.systemGateway);
      }
    }
  }

  simNetworkPortStatusUpdate();

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Determine whether network port is up or down and if the 
*           status has changed, take appropriate action.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void simNetworkPortStatusUpdate(void)
{
  if (simMayEnableNetworkPort())
  {
    if (simOperInfo->networkPortStatus != L7_ENABLE)
    {
      simNetworkPortEnable();
    }
  }
  else
  {
    if (simOperInfo->networkPortStatus != L7_DISABLE)
    {
      #if 0 /* PTIN Modified: When physical interface goes down, FP puts the Linux IP interface down and, therefore, Linux deletes IP Routes. This behavior is not desired */
      simNetworkPortDisable();
      #endif
    }
  }
}

/*********************************************************************
* @purpose  Check all conditions that must be met to consider the 
*           network port up.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    There must be a non-zero management VLAN ID and at least 
*           one port in the management VLAN must be active.
*
* @end
*********************************************************************/
L7_BOOL simMayEnableNetworkPort(void)
{
  NIM_INTF_MASK_t portMask;
  L7_uint32 i;
  L7_uint32 linkState;

  if (simCfgData.systemMgmtVlanId == 0)
  {
    return L7_FALSE;
  }

  /* Get ports that are currently members of the mgmt VLAN */
  if (dot1qVlanEgressPortsGet(simCfgData.systemMgmtVlanId, &portMask) == L7_SUCCESS)
  {
    for (i = 1; i < L7_MAX_INTERFACE_COUNT; i++)
    {
      if (L7_INTF_ISMASKBITSET(portMask, i))
      {
        if ((nimGetIntfActiveState(i, &linkState) == L7_SUCCESS) &&
            (linkState == L7_ACTIVE) &&
            !mirrorIsActiveProbePort(i) &&
            !dot3adIsLagActiveMember(i))
        {
          return L7_TRUE;
        }
      }
    }
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Handle link up of the network port.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void simNetworkPortEnable(void)
{
  char ifName[32];

  /* Bring the network port interface up in the IP stack */
  osapiSnprintf(ifName, sizeof(ifName), "%s0", L7_DTL_PORT_IF);
  osapiIfEnable(ifName);
  osapiIfGratArpSend(ifName);

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
  /* Poke DHCPv6 client protocol on network port */
  if (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP)
  {
    dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_DHCP, ifName, 0);
  }
#endif

  /* If configured, Acquire an Address via DHCP on the Network port. */
  if (simCfgData.systemConfigMode == L7_SYSCONFIG_MODE_DHCP)
  {
    dhcpClientIPAddressMethodSet (0, L7_INTF_IP_ADDR_METHOD_DHCP,
                                  L7_MGMT_NETWORKPORT, L7_FALSE);
  }

  simOperInfo->networkPortStatus = L7_ENABLE;
}

/*********************************************************************
* @purpose  Handle link down of the network port.
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void simNetworkPortDisable(void)
{
  char ifname[32];

  osapiSnprintf(ifname, sizeof(ifname), "%s0", L7_DTL_PORT_IF);

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)

  if (simCfgData.systemIPv6ConfigMode == L7_SYSCONFIG_MODE_DHCP)
  {
    dhcpv6SendCtlPacket(L7_MGMT_NETWORKPORT, L7_SYSCONFIG_MODE_NONE, ifname, 0);
  }
#endif

  /* If configured, Acquire an Address via DHCP on the Network port. */
  if (simCfgData.systemConfigMode == L7_SYSCONFIG_MODE_DHCP)
  {
    dhcpClientIPAddressMethodSet (0, L7_INTF_IP_ADDR_METHOD_NONE,
                                  L7_MGMT_NETWORKPORT, L7_FALSE);
  }

  simOperInfo->networkPortStatus = L7_DISABLE;

  /* Set the network port interface down in the IP stack */
  osapiIfDisable(ifname);
}


/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/

/*********************************************************************
 * @purpose  Build non-default  config data
 *
 * @param    void
 *
 * @returns  void
 *
 * @notes    This routine is based on xxxBuildDefaultConfigData.
 *
 * @end
 *********************************************************************/
void simBuildTestConfigData(void)
{
  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
  strncpy(simCfgData.systemName, "TEST_NAME", L7_SYS_SIZE);

  strncpy(simCfgData.systemLocation, "TEST_PLACE", L7_SYS_SIZE);

  strncpy(simCfgData.systemContact, "TEST_CONTACT", L7_SYS_SIZE);

  simCfgData.servPortIpAddress = 0x0A0A0A01;

  simCfgData.servPortNetMask = 0xFFFF0000;

  simCfgData.servPortGateway = 0x0A0A0A02;

  simCfgData.systemIpAddress = 0x0B0A0A01;

  simCfgData.systemNetMask = 0xFFFF0000;

  simCfgData.systemGateway = 0x0B0A0A02;

  simCfgData.systemLAA[0]=0x02;
  simCfgData.systemLAA[1]=0x02;
  simCfgData.systemLAA[2]=0x03;
  simCfgData.systemLAA[3]=0x04;
  simCfgData.systemLAA[4]=0x05;
  simCfgData.systemLAA[5]=0x06;

  simCfgData.systemBaudRate        = FD_SIM_DEFAULT_SYSTEM_BAUD_RATE;           /* Left this the same to facilitate test */
  simCfgData.serialPortStopBits    = FD_SIM_DEFAULT_SERIAL_PORT_STOPBITS;       /* Left this the same to facilitate test */
  simCfgData.serialPortParityType  = FD_SIM_DEFAULT_SERIAL_PORT_PARITY_TYPE;    /* Left this the same to facilitate test */
  simCfgData.serialPortFlowControl = FD_SIM_DEFAULT_SERIAL_PORT_FLOW_CONTROL;   /* Left this the same to facilitate test */
  simCfgData.serialPortCharSize    = FD_SIM_DEFAULT_SERIAL_PORT_CHAR_SIZE;      /* Left this the same to facilitate test */

  simCfgData.serialPortTimeOut = 10;

  simCfgData.systemMacType = L7_SYSMAC_LAA;

  simCfgData.systemConfigMode = L7_SYSCONFIG_MODE_BOOTP;

  if (SERVICE_PORT_PRESENT)
  {
    simCfgData.servPortConfigMode = L7_SYSCONFIG_MODE_NONE;
  }
  else
    simCfgData.servPortConfigMode = L7_SYSCONFIG_MODE_DHCP;

  simCfgData.systemUnitID        = 6;

  simTransferInfo.systemTransferMode     =  L7_TRANSFER_ZMODEM;

  simTransferInfo.systemTransferServerIp.family              =  L7_AF_INET6;
  /* check this */
  simTransferInfo.systemTransferServerIp.addr.ipv6.in6.addr32[0] =  0x0A0A0A05;
  simTransferInfo.systemTransferServerIp.addr.ipv6.in6.addr32[1] =  0x0A0A0A05;
  simTransferInfo.systemTransferServerIp.addr.ipv6.in6.addr32[2] =  0x0A0A0A05;
  simTransferInfo.systemTransferServerIp.addr.ipv6.in6.addr32[3] =  0x0A0A0A05;

  strncpy(simTransferInfo.systemTransferFilePathRemote, "/tmp/tftp", L7_MAX_FILENAME);
  strncpy(simTransferInfo.systemTransferFileNameRemote, "/transferFile",L7_MAX_FILENAME);
  simTransferInfo.systemUploadFileType   =  L7_FILE_TYPE_TRAP_LOG;
  simTransferInfo.systemDownloadFileType =  L7_FILE_TYPE_CONFIG;

  simCfgData.systemMgmtVlanId = 20;

  /* End of Component's Test Non-default configuration Data */

  /* Force write of config file */
  simCfgData.cfgHdr.dataChanged = L7_TRUE;
  sysapiPrintf("Built test config data\n");
}

/*********************************************************************
 *
 * @purpose  Dump the contents of the config data.
 *
 * @param    void
 *
 * @returns  void
 *
 * @comments
 *
 * @end
 *
 *********************************************************************/
void simConfigDataTestShow(void)
{
  L7_fileHdr_t  *pFileHdr;
  L7_uint32     i;

  /*-----------------------------*/
  /* Config File Header Contents */
  /*-----------------------------*/
  pFileHdr = &simCfgData.cfgHdr;
  sysapiPrintf("pFileHdr->filename    : %s\n", (char *)pFileHdr->filename);
  sysapiPrintf("pFileHdr->version     : %u\n", pFileHdr->version);
  sysapiPrintf("pFileHdr->componentID : %u\n", pFileHdr->componentID);
  sysapiPrintf("pFileHdr->type        : %u\n", pFileHdr->type);
  sysapiPrintf("pFileHdr->length      : %u\n", pFileHdr->length);
  sysapiPrintf("pFileHdr->dataChanged : %u\n", pFileHdr->dataChanged);

  /* Start of release I file header changes */

  sysapiPrintf("pFileHdr->savePointId : %u\n", pFileHdr->savePointId);
  sysapiPrintf("pFileHdr->targetDevice: %u\n", pFileHdr->targetDevice);
  for (i = 0; i < L7_FILE_HDR_PAD_LEN; i++)
  {
    sysapiPrintf("pFileHdr->pad[i]      : %u\n", pFileHdr->pad[i]);
  }

  /*-----------------------------*/
  /* cfgParms                    */
  /*-----------------------------*/

  simConfigDump();

  /*-----------------------------*/
  /* Checksum                    */
  /*-----------------------------*/
  sysapiPrintf("simCfgData.checkSum : %u\n", simCfgData.checkSum);
}

void simDebugMacAdd(L7_uchar8 mac1, L7_uchar8 mac2, L7_uchar8 mac3,
                    L7_uchar8 mac4, L7_uchar8 mac5, L7_uchar8 mac6)
{
  systemBIA[0] = mac1;
  systemBIA[1] = mac2;
  systemBIA[2] = mac3;
  systemBIA[3] = mac4;
  systemBIA[4] = mac5;
  systemBIA[5] = mac6;

  simCfgData.systemLAA[0] = mac1;
  simCfgData.systemLAA[1] = mac2;
  simCfgData.systemLAA[2] = mac3;
  simCfgData.systemLAA[3] = mac4;
  simCfgData.systemLAA[4] = mac5;
  simCfgData.systemLAA[5] = mac6;

  /* Set system interface MAC address.
   */
  if (simCfgData.systemMacType == L7_SYSMAC_LAA)
  {
    fdbSysMacDelEntry(systemBIA,
                      simCfgData.systemMgmtVlanId,
                      1, DTL_MAC_TYPE_BURNED_IN);

    osapiUpdateNetworkMac(simCfgData.systemLAA);
  }
  else
  {
    fdbSysMacDelEntry(simCfgData.systemLAA,
                      simCfgData.systemMgmtVlanId,
                      1, DTL_MAC_TYPE_LOCAL_ADMIN);

    osapiUpdateNetworkMac(systemBIA);
  }
}

void simDebugIPAdd(int ip, int mask)
{
  simSetSystemConfigMode(L7_SYSCONFIG_MODE_NONE);
  simSystemIPAddrConfigure(ip);
  simSystemIPNetMaskConfigure(mask);
}



