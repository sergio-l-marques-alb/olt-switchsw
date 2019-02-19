/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename sim_migrate.h
*
* @purpose System Interface Manager Configuration Migration
*
* @component sim
*
* @comments none
*
* @create 8/23/2004
*
* @author Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#ifndef SIM_MIGRATE_H
#define SIM_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "sim.h"

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  char           systemName[L7_SYS_SIZE_32];          
  char           systemLocation[L7_SYS_SIZE_32];         
  char           systemContact[L7_SYS_SIZE_32];

  L7_uint32      servPortIpAddress;
  L7_uint32      servPortNetMask;
  L7_uint32      servPortGateway;

  L7_uint32      systemIpAddress;
  L7_uint32      systemNetMask;
  L7_uint32      systemGateway;

  L7_char8       systemBIA[L7_MAC_ADDR_LEN];
  L7_char8       systemLAA[L7_MAC_ADDR_LEN];
  L7_uint32      systemMacType;                /* SIM_MACTYPE_BIA or SIM_MACTYPE_LAA */

  L7_uint32      systemConfigMode;             /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP or L7_SYSCONFIG_MODE_BOOTP */
  L7_uint32      servPortConfigMode;           /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP or L7_SYSCONFIG_MODE_BOOTP */

  L7_uint32      systemBaudRate;
  L7_uint32      serialPortTimeOut;
  L7_uint32      serialPortStopBits;
  L7_uint32      serialPortParityType;
  L7_uint32      serialPortFlowControl;
  L7_uint32      serialPortCharSize;

  L7_uint32      systemTransferMode;
  L7_uint32      systemTransferServerIp;
  L7_char8       systemTransferFilePath[L7_MAX_FILENAME];
  L7_char8       systemTransferFileName[L7_MAX_FILENAME];
  L7_uint32      systemUploadFileType;
  L7_uint32      systemDownloadFileType;
   
  L7_uint32      systemUnitID;                 /* Unit ID in stack */
  
  L7_uint32      checkSum;                     /* check sum of config file NOTE: needs to be last entry */

} simCfgData_ver2_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  char           systemName[L7_SYS_SIZE_32];          
  char           systemLocation[L7_SYS_SIZE_32];         
  char           systemContact[L7_SYS_SIZE_32];

  L7_uint32      servPortIpAddress;
  L7_uint32      servPortNetMask;
  L7_uint32      servPortGateway;

  L7_uint32      systemIpAddress;
  L7_uint32      systemNetMask;
  L7_uint32      systemGateway;

  L7_char8       systemBIA[L7_MAC_ADDR_LEN];
  L7_char8       systemLAA[L7_MAC_ADDR_LEN];
  L7_uint32      systemMacType;                /* SIM_MACTYPE_BIA or SIM_MACTYPE_LAA */

  L7_uint32      systemConfigMode;             /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP or L7_SYSCONFIG_MODE_BOOTP */
  L7_uint32      servPortConfigMode;           /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP or L7_SYSCONFIG_MODE_BOOTP */

  L7_uint32      systemBaudRate;
  L7_uint32      serialPortTimeOut;
  L7_uint32      serialPortStopBits;
  L7_uint32      serialPortParityType;
  L7_uint32      serialPortFlowControl;
  L7_uint32      serialPortCharSize;

  L7_uint32      systemTransferMode;
  L7_uint32      systemTransferServerIp;
  L7_char8       systemTransferFilePath[L7_MAX_FILENAME];
  L7_char8       systemTransferFileName[L7_MAX_FILENAME];
  L7_uint32      systemUploadFileType;
  L7_uint32      systemDownloadFileType;
   
  L7_uint32      systemUnitID;                 /* Unit ID in stack */

  L7_uint32      systemMgmtVlanId;             /* Management VLAN ID */

  L7_uint32      checkSum;                     /* check sum of config file NOTE: needs to be last entry */

} simCfgData_ver3_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  char           systemName[L7_SYS_SIZE_32];          
  char           systemLocation[L7_SYS_SIZE_32];         
  char           systemContact[L7_SYS_SIZE_32];

  L7_uint32      servPortIpAddress;
  L7_uint32      servPortNetMask;
  L7_uint32      servPortGateway;

  L7_uint32      systemIpAddress;
  L7_uint32      systemNetMask;
  L7_uint32      systemGateway;

  L7_char8       systemBIA[L7_MAC_ADDR_LEN];
  L7_char8       systemLAA[L7_MAC_ADDR_LEN];
  L7_uint32      systemMacType;                /* SIM_MACTYPE_BIA or SIM_MACTYPE_LAA */

  L7_uint32      systemConfigMode;             /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP or L7_SYSCONFIG_MODE_BOOTP */
  L7_uint32      servPortConfigMode;           /* L7_SYSCONFIG_MODE_NONE, L7_SYSCONFIG_MODE_DHCP or L7_SYSCONFIG_MODE_BOOTP */

  L7_uint32      systemBaudRate;
  L7_uint32      serialPortTimeOut;
  L7_uint32      serialPortStopBits;
  L7_uint32      serialPortParityType;
  L7_uint32      serialPortFlowControl;
  L7_uint32      serialPortCharSize;

  L7_uint32      systemTransferUnitNumber;
  L7_uint32      systemUnitID;                 /* Unit ID in stack */

  L7_uint32      systemMgmtVlanId;             /* Management VLAN ID */

  L7_uint32      checkSum;                     /* check sum of config file NOTE: needs to be last entry */

} simCfgData_ver4_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  char           systemName[L7_SYS_SIZE_32];
  char           systemLocation[L7_SYS_SIZE_32];
  char           systemContact[L7_SYS_SIZE_32];

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

  L7_uint32      systemBaudRate;
  L7_uint32      serialPortTimeOut;
  L7_uint32      serialPortStopBits;
  L7_uint32      serialPortParityType;
  L7_uint32      serialPortFlowControl;
  L7_uint32      serialPortCharSize;

  L7_uint32      systemTransferUnitNumber;
  L7_uint32      systemUnitID;                 /* Unit ID in stack */

  L7_uint32      systemMgmtVlanId;             /* Management VLAN ID */

  L7_uint32      checkSum;                     /* check sum of config file NOTE: needs to be last entry */

} simCfgData_ver5_t;

typedef simCfgData_t simCfgData_ver6_t;

#endif /* SIM_MIGRATE_H */
