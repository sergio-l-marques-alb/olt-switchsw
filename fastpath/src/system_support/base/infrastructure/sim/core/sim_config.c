/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename sim_config.c
*
* @purpose System Interface Manager Factory Defaults File
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

/*************************************************************

*************************************************************/

#define SIM_FACTORY_DEFAULT   /* needed to remove warning messages during compile */

#include "l7_cnfgr_api.h"
#include "l7_common.h"
#include "nvstoreapi.h"
#include "osapi.h"
#ifdef L7_PRODUCT_SMARTPATH
#include "osapi_support.h"
#endif
#include "defaultconfig.h"
#include "dtlapi.h"
#include "log.h"
#include "sim.h"
#include "simapi.h"
#include "sim_debug_api.h"
#include "support_api.h"
#include "osapi_support.h"

extern simCfgData_t         simCfgData;
extern simTransferInfo_t    simTransferInfo;
mibDescr_t     mibDescr;

simDebugCfg_t simDebugCfg;    /* Debug Configuration File Overlay */

extern void simDebugCfgUpdate(void);

/*********************************************************************
* @purpose  Get MIB capability description
*
* @param    Index
* @param    buf   description
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t simMibDescription(L7_uint32 Index, L7_char8 *buf)
{
#if(FD_SIM_MIB_DESCRIPTION != L7_MIB_SIZE)
#error SIM Build Error
#endif

  memcpy((void*)buf, (void*)&FD_mibDescr[Index], strlen((char *) &(FD_mibDescr[Index])));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default system config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void simBuildDefaultConfigData(L7_uint32 ver)
{
  memset(( void * )&simCfgData, 0, sizeof( simCfgData_t));

  strcpy(simCfgData.cfgHdr.filename, SIM_CFG_FILENAME);

  simCfgData.cfgHdr.version = ver;

  simCfgData.cfgHdr.componentID = L7_SIM_COMPONENT_ID;

  simCfgData.cfgHdr.type = L7_CFG_DATA;

  simCfgData.cfgHdr.length = sizeof(simCfgData);

  simCfgData.cfgHdr.dataChanged = L7_FALSE;

  strcpy(simCfgData.systemName, "");

  strcpy(simCfgData.systemLocation, "");

  strcpy(simCfgData.systemContact, "");

  simCfgData.servPortIpAddress = FD_SIM_DEFAULT_SERVPORT_IP_ADDR;
  simCfgData.servPortNetMask = FD_SIM_DEFAULT_SERVPORT_MASK;
  simCfgData.servPortGateway = FD_SIM_DEFAULT_SERVPORT_GW;

  simCfgData.servPortIpv6AdminMode = FD_SIM_DEFAULT_SERVPORT_IPV6_ADMIN_MODE;

  simCfgData.systemIpAddress = FD_SIM_DEFAULT_NETWORK_IP_ADDR;
  simCfgData.systemNetMask = FD_SIM_DEFAULT_NETWORK_MASK;
  simCfgData.systemGateway = FD_SIM_DEFAULT_NETWORK_GW;

  simCfgData.systemIpv6AdminMode = FD_SIM_DEFAULT_SYSTEM_IPV6_ADMIN_MODE;

  memset(simCfgData.systemLAA, 0, sizeof(simCfgData.systemLAA));

  simCfgData.systemBaudRate        = FD_SIM_DEFAULT_SYSTEM_BAUD_RATE;
  simCfgData.serialPortStopBits    = FD_SIM_DEFAULT_SERIAL_PORT_STOPBITS;
  simCfgData.serialPortParityType  = FD_SIM_DEFAULT_SERIAL_PORT_PARITY_TYPE;
  simCfgData.serialPortFlowControl = FD_SIM_DEFAULT_SERIAL_PORT_FLOW_CONTROL;
  simCfgData.serialPortCharSize    = FD_SIM_DEFAULT_SERIAL_PORT_CHAR_SIZE;

  simCfgData.serialPortTimeOut = FD_SIM_DEFAULT_SERIAL_PORT_TIMEOUT;

  simCfgData.systemMacType = FD_SIM_MACTYPE;

  simCfgData.systemConfigMode = FD_SIM_NETWORK_CONFIG_MODE;
  simCfgData.systemIPv6ConfigMode = FD_SIM_NETWORK_IPV6_CONFIG_MODE;

  if (SERVICE_PORT_PRESENT)
  {
    simCfgData.servPortConfigMode = FD_SIM_SERVPORT_CONFIG_MODE;
    simCfgData.servPortIPv6ConfigMode = FD_SIM_SERVPORT_IPV6_CONFIG_MODE;
    simCfgData.servPortAdminState     = FD_SIM_SERVPORT_ADMIN_STATE;
  }
  else
  {
    simCfgData.servPortConfigMode = L7_SYSCONFIG_MODE_NONE;
    simCfgData.servPortIPv6ConfigMode = L7_SYSCONFIG_MODE_NONE;
    simCfgData.servPortAdminState     = L7_DISABLE;
  }

  simCfgData.systemIPv6AddrAutoConfig   = FD_SIM_NETWORK_IPV6_AUTOCONFIG_MODE;
  simCfgData.servPortIPv6AddrAutoConfig = FD_SIM_SERVPORT_IPV6_AUTOCONFIG_MODE;

  simCfgData.systemUnitID        = FD_SIM_DEFAULT_STACK_ID;

  /* setting default values for the non-configuration transfer attributes */
  simTransferInfo.systemTransferMode            =  FD_SIM_DEFAULT_TRANSFER_MODE;
  simTransferInfo.systemUploadFileType          =  FD_SIM_DEFAULT_UPLOAD_FILE_TYPE;
  simTransferInfo.systemDownloadFileType        =  FD_SIM_DEFAULT_DOWNLOAD_FILE_TYPE;
  simTransferInfo.systemTransferDirection       =  L7_TRANSFER_DIRECTION_DOWN;
  simTransferInfo.systemTransferComplete        =  L7_FALSE;
  simTransferInfo.systemBytesTransferred        =  0;
  simTransferInfo.systemTransferServerIp.family =  FD_SIM_DEFAULT_TRANSFER_SERVER_ADDR_TYPE;
  memset((L7_uchar8 *)&(simTransferInfo.systemTransferServerIp.addr), 0,
         sizeof(simTransferInfo.systemTransferServerIp.addr));

  strcpy(simTransferInfo.systemTransferFilePathLocal, "");
  strcpy(simTransferInfo.systemTransferFileNameLocal, "");
  strcpy(simTransferInfo.systemTransferFilePathRemote, "");
  strcpy(simTransferInfo.systemTransferFileNameRemote, "");

  simCfgData.systemMgmtVlanId = FD_SIM_DEFAULT_MGMT_VLAN_ID;
  simCfgData.systemMgmtPortNum = FD_SIM_DEFAULT_MGMT_PORT;

  simCfgData.serialHistoryBufferSize = FD_SIM_DEFAULT_SYSTEM_HISTORY_SIZE;
  simCfgData.telnetHistoryBufferSize = FD_SIM_DEFAULT_SYSTEM_HISTORY_SIZE;
  simCfgData.sshHistoryBufferSize = FD_SIM_DEFAULT_SYSTEM_HISTORY_SIZE;
  simCfgData.serialHistoryBufferEnable = FD_SERIAL_HISTORY_DEFAULT_ADMIN_MODE;
  simCfgData.telnetHistoryBufferEnable = FD_TELNET_HISTORY_DEFAULT_ADMIN_MODE;
  simCfgData.sshHistoryBufferEnable = FD_SSH_HISTORY_DEFAULT_ADMIN_MODE;


  /* TimeZone elements */
  strcpy ((void *)&simCfgData.systemTimeZone.tzName, "");
  simCfgData.systemTimeZone.hour = FD_TIMEZONE_HRS;
  simCfgData.systemTimeZone.minutes = FD_TIMEZONE_MINS;

  simCfgData.summerTime.persistMode = L7_ENABLE;
  simCfgData.summerTime.mode = FD_TIMEZONE_SUMMERTIME;

  simCfgData.memoryMonitorData.freeMemoryThreshold = 
                                              FD_SIM_DEFAULT_FREE_MEM_THRESHOLD;

  simCfgData.utilMonitorData.fallingThreshold = FD_SIM_DEFAULT_CPU_UTIL_THRESHOLD;
  simCfgData.utilMonitorData.risingThreshold =  FD_SIM_DEFAULT_CPU_UTIL_THRESHOLD;
  simCfgData.utilMonitorData.risingPeriod =  FD_SIM_DEFAULT_CPU_UTIL_PERIOD;
  simCfgData.utilMonitorData.fallingPeriod = FD_SIM_DEFAULT_CPU_UTIL_PERIOD;

#if 0
  /* reset the clock-time */
  /* Not required to do */
  osapiSetTimeToDefault();
#endif
}

/*********************************************************************
* @purpose  Saves sim debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    simDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t simDebugSave(void)
{
  L7_RC_t rc;

  rc = L7_SUCCESS;

  if (simDebugHasDataChanged() == L7_TRUE)
  {
    /* Copy the operational states into the config file */
    simDebugCfgUpdate();

    /* Store the config file */
    simDebugCfg.hdr.dataChanged = L7_FALSE;
    simDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&simDebugCfg,
                                      (L7_uint32)(sizeof(simDebugCfg) - sizeof(simDebugCfg.checkSum)));
        /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_SIM_COMPONENT_ID, SIM_DEBUG_CFG_FILENAME ,
                            (L7_char8 *)&simDebugCfg, (L7_uint32)sizeof(simDebugCfg_t))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
              "Error on call to sysapiSupportCfgFileWrite routine on config file %s\n",SIM_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Restores sim debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    simDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t simDebugRestore(void)
{
  L7_RC_t rc;
  simDebugBuildDefaultConfigData(SIM_DEBUG_CFG_VER_CURRENT);

  simDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = simApplyDebugConfigData();

  return rc;
}

/*********************************************************************
* @purpose  Checks if sim debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL simDebugHasDataChanged(void)
{
  return simDebugCfg.hdr.dataChanged;
}

/*********************************************************************
* @purpose  Build default sim config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void simDebugBuildDefaultConfigData(L7_uint32 ver)
{
  /* setup file header */
  simDebugCfg.hdr.version = ver;
  simDebugCfg.hdr.componentID = L7_SIM_COMPONENT_ID;
  simDebugCfg.hdr.type = L7_CFG_DATA;
  simDebugCfg.hdr.length = (L7_uint32)sizeof(simDebugCfg);
  strcpy((L7_char8 *)simDebugCfg.hdr.filename, SIM_DEBUG_CFG_FILENAME);
  simDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&simDebugCfg.cfg, 0, sizeof(simDebugCfg.cfg));
}

/*********************************************************************
* @purpose  Apply sim debug config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t simApplyDebugConfigData(void)
{
  L7_RC_t rc;

  rc = pingDebugPacketTraceFlagSet(simDebugCfg.cfg.pingDebugPacketTraceFlag);

  return rc;
}
