/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snmp_exports.h
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
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __SNMP_EXPORTS_H_
#define __SNMP_EXPORTS_H_

#include "datatypes.h"

/*  Row status - from RFC 2579 */
typedef enum
{
  L7_ROW_STATUS_INVALID = 0,
  L7_ROW_STATUS_ACTIVE = 1,
  L7_ROW_STATUS_NOT_IN_SERVICE = 2,
  L7_ROW_STATUS_NOT_READY = 3,
  L7_ROW_STATUS_CREATE_AND_GO = 4,
  L7_ROW_STATUS_CREATE_AND_WAIT = 5,
  L7_ROW_STATUS_DESTROY = 6
} L7_ROW_STATUS_t;


#define L7_COMMUNITY_NAME_MIN_SIZE 1
#define L7_COMMUNITY_NAME_MAX_SIZE 20
#define L7_MAX_SNMPSERVER_SYSTEM 255

#define L7_MAX_VIEW_NAME_LEN  30
#define L7_MIN_VIEW_NAME_LEN  1

#define CLI_MAX_FILTERS              64
#define CLI_MAX_GROUPS               8
#define CLI_RULES_PER_VIEW           8
#define CLI_RULES_PER_FILTER         8
#define CLI_MAX_SNMPV3_USERS         8
#define CLIV3HOST_USERNAMELEN        25
#define CLI_MAX_COMMUNITIES          8
#define CLI_MAX_VIEWS                64
#define CLI_MAX_SNMP_TRAPS_RECEIVERS 8
#define CLI_SNMP_MAX_FILTERNAME_SIZE 30
#define CLI_SNMP_MAX_OIDTREE_SIZE 20
#define CLI_SNMP_MAX_INCLUDEXCLUD_SIZE 8
#define L7_SNMP_COMMUNITY_NAME_LENGTH  20
#define L7_SNMP_FILTER_NAME_SIZE    30
#define L7_SNMP_OIDTREE_INCLUDED    1
#define L7_SNMP_OIDTREE_EXCLUDED    2

#define L7_SNMP_MAX_AUTHPRIVKEY_SIZE 64
#define L7_SNMP_MIN_ENGINEID_SIZE    5
#define L7_SNMP_MAX_SHA_AUTHPRIVKEY_SIZE 64
#define L7_SNMP_MAX_ENGINEID_SIZE   32
#define L7_SNMP_MAX_SIZE            31
#define L7_SNMP_MAX_USER_NAME_SIZE  30
#define L7_SNMP_MAX_GROUP_NAME_SIZE 30
#define L7_SNMP_MIN_GROUP_NAME_SIZE 1
#define L7_SNMP_MAX_AUTHPROTO_SIZE  4
#define L7_SNMP_MAX_AUTHPWD_SIZE    32
#define L7_SNMP_MAX_PRIVPROTO       4
#define L7_SNMP_MAX_PRIVKEY         64
#define L7_CLI_SNMP_RETRIES_COUNT_MIN 1
#define L7_CLI_SNMP_RETRIES_COUNT_MAX 255
#define L7_CLI_SNMP_TIMEOUT_COUNT_MIN          1
#define L7_CLI_SNMP_TIMEOUT_COUNT_MAX        300
#define L7_CLI_SNMP_USERNAME_MIN  1
#define L7_CLI_SNMP_USERNAME_MAX  25
#define L7_CLI_SNMP_HOSTNAME_SIZE        255

#define L7_SNMP_DEFAULT_ENGINE   1
#define L7_SNMP_MANUAL_ENGINE   3

#define L7_SNMP_MIN_ENGINE_SIZE   6
#define L7_SNMP_MAX_ENGINE_SIZE   32

#define FD_DEFALUT_SNMP_RETRIES_COUNT      3
#define FD_DEFALUT_SNMP_TIMEOUT_COUNT     15
#define FD_DEFAULT_SNMP_UDP_PORT           1162         /* PTin modified: port connections (162) */
#define FD_DEFAULT_SNMP_VERSION            1
#define FD_DEFAULT_SNMP_SECURITY           1

/*****************snmp/add_remote.html**/
#define L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN   8
#define L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN       8

#define L7_SNMP_NAME_SIZE_VALUE    20

#define L7_CLI_SNMP_HOSTNAME_SIZE        255

/* SNMP User Feature List */
typedef enum
{
  L7_SNMP_USER_SUPPORTED = 0,            /* general support statement */
  L7_SNMP_USER_AUTHENTICATION_FEATURE_ID,
  L7_SNMP_USER_ENCRYPTION_FEATURE_ID,
  L7_SNMP_USER_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_SNMP_USER_FEATURE_IDS_t;




/*--------------------------------------*/
/*  SNMP Constants                      */
/*--------------------------------------*/
#define L7_MAX_SNMP_COMM   6
#define L7_SNMP_SIZE      32
#define L7_SNMP_NAME_SIZE 16  /* snmp string names are 16 char long  */
#define L7_SNMP_NAME_SIZE_VALUE    20

#define L7_SNMP_TASK_NAME   "SNMPTask"


#define L7_SNMP_AUTHENTICATION_PASSWORD_MIN_LEN   8
#define L7_SNMP_ENCRYPTION_PASSWORD_MIN_LEN       8

#define L7_ENCRYPTION_KEY_SIZE    65

#define L7_SNMP_DEFAULT_NETMASK 0xFFFFFF00

#define L7_SNMP_ENGINE_OID {0x80,0x00,0x11,0x3D,0x03,0x00,0x00,0x00,0x00,0x00,0x00}  /* Enterprise: 4413 */

typedef enum {
  L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY=1,
  L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE,
  L7_SNMP_COMMUNITY_ACCESS_LEVEL_SUPER
} L7_SNMP_COMMUNITY_ACCESS_LEVEL_t;

typedef enum {
  L7_SNMP_COMMUNITY_STATUS_VALID=1,
  L7_SNMP_COMMUNITY_STATUS_INVALID,
  L7_SNMP_COMMUNITY_STATUS_DELETE,
  L7_SNMP_COMMUNITY_STATUS_CONFIG
} L7_SNMP_COMMUNITY_STATUS_t;

typedef enum {
  L7_SNMP_TRAP_MGR_STATUS_VALID=1,
  L7_SNMP_TRAP_MGR_STATUS_INVALID,
  L7_SNMP_TRAP_MGR_STATUS_DELETE,
  L7_SNMP_TRAP_MGR_STATUS_CONFIG
} L7_SNMP_TRAP_MGR_STATUS_t;

typedef enum {
  L7_SNMP_USER_ACCESS_LEVEL_READ_ONLY=1,
  L7_SNMP_USER_ACCESS_LEVEL_READ_WRITE
} L7_SNMP_USER_ACCESS_LEVEL_t;

typedef enum {
  L7_SNMP_USER_AUTH_PROTO_NONE=1,
  L7_SNMP_USER_AUTH_PROTO_HMACMD5,
  L7_SNMP_USER_AUTH_PROTO_HMACSHA
} L7_SNMP_USER_AUTH_PROTO_t;

typedef enum {
  L7_SNMP_USER_PRIV_PROTO_NONE=1,
  L7_SNMP_USER_PRIV_PROTO_DES
} L7_SNMP_USER_PRIV_PROTO_t;

typedef enum {
  L7_SNMP_TRAP_MGR_ADDR_TYPE_IPv4=1,
  L7_SNMP_TRAP_MGR_ADDR_TYPE_IPv6,
  L7_SNMP_TRAP_MGR_ADDR_TYPE_DNSv4,
  L7_SNMP_TRAP_MGR_ADDR_TYPE_DNSv6 
} L7_SNMP_TRAP_MGR_ADDR_TYPE;

#define L7_SNMP_SUPPORTED_MIB_COUNT             64
#define L7_SNMP_SUPPORTED_MIB_NAME_SIZE         64
#define L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE  128
#define L7_SNMP_SUPPORTED_MIB_OID_SIZE          128

/* Start Depricated 03/14/2001 */
#define L7_AGENT_TRAP_MGR_STATUS_VALID    1
#define L7_AGENT_TRAP_MGR_STATUS_INVALID  2
#define L7_AGENT_TRAP_MGR_STATUS_DELETE   3

#define L7_AGENT_COMM_ACCESS_LEVEL_READ_ONLY    1
#define L7_AGENT_COMM_ACCESS_LEVEL_READ_WRITE   2

#define L7_AGENT_COMM_ACCESS_STATUS_VALID    1
#define L7_AGENT_COMM_ACCESS_STATUS_INVALID  2
#define L7_AGENT_COMM_ACCESS_STATUS_DELETE   3

#define L7_AGENT_COMM_CLIENT_STATUS_VALID    1
#define L7_AGENT_COMM_CLIENT_STATUS_INVALID  2
#define L7_AGENT_COMM_CLIENT_STATUS_DELETE   3

/* end Depricated 03/14/2001 */


typedef
enum
{
  snmpStorageType_other       = 1,
  snmpStorageType_volatile    = 2,
  snmpStorageType_nonVolatile = 3,
  snmpStorageType_permanent   = 4,
  snmpStorageType_readOnly    = 5,
  snmpStorageType_last                      /* do not remove */
} l7_snmpStorageType_t;

typedef
enum
{
  snmpRowStatus_invalid       = 0,
  snmpRowStatus_active        = 1,
  snmpRowStatus_notInService  = 2,
  snmpRowStatus_notReady      = 3,
  snmpRowStatus_createAndGo   = 4,
  snmpRowStatus_createAndWait = 5,
  snmpRowStatus_destroy       = 6,
  snmpRowStatus_last                        /* do not remove */
} l7_snmpRowStatus_t;

typedef
enum
{
  snmpNotifyFilterType_included = 1,
  snmpNotifyFilterType_excluded = 2,
  snmpNotifyFilterType_last                 /* do not remove */
} l7_snmpNotifyFilterType_t;

typedef
enum
{
  snmpSecurityModel_any     = 0,
  snmpSecurityModel_snmpv1  = 1,
  snmpSecurityModel_snmpv2c = 2,
  snmpSecurityModel_usm     = 3,
  snmpSecurityModel_last                    /* do not remove */
} l7_snmpSecurityModel_t;

typedef
enum
{
  snmpSecurityLevel_noAuthNoPriv = 1,
  snmpSecurityLevel_authNoPriv   = 2,
  snmpSecurityLevel_authPriv     = 3,
  snmpSecurityLevel_last                    /* do not remove */
} l7_snmpSecurityLevel_t;


typedef
enum
{
  snmpMessageProcessingModel_snmpv1  = 0,
  snmpMessageProcessingModel_snmpv2c = 1,
  snmpMessageProcessingModel_snmpv2u = 2,
  snmpMessageProcessingModel_snmpv3  = 3,
  snmpMessageProcessingModel_last           /* do not remove */
} l7_snmpMessageProcessingModel_t;


typedef
enum
{
  snmpNotifyType_trap   = 1,
  snmpNotifyType_inform = 2,
  snmpNotifyType_last                       /* do not remove */
} l7_snmpNotifyType_t;

typedef
enum
{
  vacmAccessContextMatch_exact  = 1,
  vacmAccessContextMatch_prefix = 2,
  vacmAccessContextMatch_last               /* do not remove */
} l7_vacmAccessContextMatch_t;

typedef
enum
{
  vacmViewTreeFamilyType_included = 1,
  vacmViewTreeFamilyType_excluded = 2,
  vacmViewTreeFamilyType_last               /* do not remove */
} l7_vacmViewTreeFamilyType_t;


typedef struct
{
  L7_char8 agentUserName[L7_SNMP_SIZE];
  L7_uint32 agentUserAccessLevel;
  L7_uint32 agentUserAuthProtocol;
  L7_uint32 agentUserPrivProtocol;
  L7_char8 agentUserAuthKey[L7_SNMP_SIZE];
  L7_char8 agentUserPrivKey[L7_SNMP_SIZE];
  L7_uint32 agentUserStatus;
} snmpUser_t;

typedef struct
{
  L7_char8 mibName[L7_SNMP_SUPPORTED_MIB_NAME_SIZE];
  L7_char8 mibDescription[L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE];
  L7_char8 mibOID[L7_SNMP_SUPPORTED_MIB_OID_SIZE];
  L7_uint32 mibEntryCreationTime;       /* Raw uptime, in hundredths of a second (centiseconds) */
  L7_uint32 mibStatus;
} snmpSupportedMib_t;

typedef enum
{
  L7_SNMP_STATE_INIT = 0,
  L7_SNMP_STATE_CONFIG,
  L7_SNMP_STATE_RUN,
  L7_SNMP_STATE_LAST
} snmpState_t;

typedef struct
{
  void (*func_ptr) (L7_uint32, void *);
  L7_uint32 trap_id;
  void *pdata;
} L7_SNMP_TRAP_MESSAGE_t;


typedef enum {
  L7_SNMP_FILTER_STATUS_VALID=1,
  L7_SNMP_FILTER_STATUS_INVALID,
  L7_SNMP_FILTER_STATUS_DELETE,
  L7_SNMP_FILTER_STATUS_CONFIG
} L7_SNMP_FILTER_STATUS_t;

/******************** conditional Override *****************************/

#ifdef INCLUDE_SNMP_EXPORTS_OVERRIDES
#include "snmp_exports_overrides.h"
#endif

#endif /* __SNMP_EXPORTS_H_*/

