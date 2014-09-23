/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename snmp_migrate.h
*
* @purpose snmp Configuration Migration
*
* @component snmp
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "snmp_exports.h"


#ifndef SNMP_MIGRATE_H
#define SNMP_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "snmp_util.h"
#include "nvstoreapi.h"

typedef struct snmpTrapMgr_ver2_s
{
  L7_char8       agentTrapMgrCommunityName[L7_SNMP_SIZE];
  L7_uint32      agentTrapMgrIpAddr;
  L7_in6_addr_t  agentTrapMgrIPv6Addr;
  L7_uint32      agentTrapMgrStatus;
  snmpTrapVer_t  agentTrapMgrVersion;
  L7_uint32      agentTrapMgrIpAddrType;
} snmpTrapMgr_ver2_t;

typedef struct snmpTrapMgr_ver1_s
{
  L7_char8    agentTrapMgrCommunityName[L7_SNMP_SIZE];
  L7_uint32   agentTrapMgrIpAddr;
  L7_uint32   agentTrapMgrStatus;
  snmpTrapVer_t  agentTrapMgrVersion;
  L7_uint32      agentTrapMgrIpAddrType;
} snmpTrapMgr_ver1_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  snmpComm_t          snmpComm[L7_MAX_SNMP_COMM];
  snmpTrapMgr_ver2_t  snmpTrapMgr[L7_MAX_SNMP_COMM];
  L7_uint32           checkSum;
} snmpCfgData_ver2_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  snmpComm_t          snmpComm[L7_MAX_SNMP_COMM];
  snmpTrapMgr_ver1_t  snmpTrapMgr[L7_MAX_SNMP_COMM];
  L7_uint32           checkSum;
} snmpCfgData_ver1_t;

#endif /* SNMP_MIGRATE_H */
