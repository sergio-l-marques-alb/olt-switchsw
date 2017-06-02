
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dot1x_cfg.h
*
* @purpose   dot1x configuration Migration include file
*
* @component dot1x
*
* @comments  none
*
* @create    8/23/2004
*
* @author    Rama Sasthri, Kristipati
*
* @end
*             
**********************************************************************/

#ifndef DOT1X_MIGRATE_H
#define DOT1X_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "dot1x_include.h"

#define L7_DOT1X_INTF_MAX_COUNT_REL_4_1 (L7_MAX_PORT_COUNT_REL_4_1 + 1)
#define L7_DOT1X_INTF_MAX_COUNT_REL_4_2 (L7_MAX_PORT_COUNT_REL_4_2 + 1)
#define L7_DOT1X_INTF_MAX_COUNT_REL_4_3 (L7_MAX_PORT_COUNT_REL_4_3 + 1)
#define L7_DOT1X_INTF_MAX_COUNT_REL_4_4 (L7_MAX_PORT_COUNT_REL_4_4 + 1)

typedef struct
{
  L7_DOT1X_PORT_DIRECTION_t adminControlledDirections;  /* Mgmt directional control setting (r/o) */
  L7_DOT1X_PORT_CONTROL_t portControlMode;      /* Current control mode setting by mgmt */
  L7_uint32 quietPeriod;        /* Initialization value for quietWhile timer */
  L7_uint32 txPeriod;           /* Initialization value for txWhen timer */
  L7_uint32 suppTimeout;        /* Initialization value for aWhile timer when timing out Supplicant */
  L7_uint32 serverTimeout;      /* Initialization value for aWhile timer when timing out Auth. Server */
  L7_uint32 maxReq;             /* Max EAP Request/Identity retransmits before time out Supplicant */
  L7_uint32 reAuthPeriod;       /* Number of seconds between periodic reauthentication */
  L7_BOOL reAuthEnabled;        /* TRUE if reauthentication is enabled */
  L7_BOOL keyTxEnabled;         /* TRUE if key transmission is enabled */
  L7_uint32 extIfNum;           /* External interface number of this port */
}
dot1xPortCfgV1_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 dot1xAdminMode;     /* Global admin mode (SystemAuthControl) */
  L7_uint32 dot1xLogTraceMode;  /* Enable/disable log file tracing */
  dot1xPortCfgV1_t dot1xPortCfg[L7_MAX_INTERFACE_COUNT_REL_4_0];        /* Per-port config info */
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
}
dot1xCfgV1_t;

typedef struct dot1xPortCfgV2_s
{
  nimConfigID_t configId;   /* NIM config ID for this interface*/
  L7_DOT1X_PORT_DIRECTION_t adminControlledDirections;  /* Mgmt directional control setting (r/o) */
  L7_DOT1X_PORT_CONTROL_t   portControlMode;     /* Current control mode setting by mgmt */
  L7_uint32 quietPeriod;    /* Initialization value for quietWhile timer */
  L7_uint32 txPeriod;       /* Initialization value for txWhen timer */
  L7_uint32 suppTimeout;    /* Initialization value for aWhile timer when timing out Supplicant */
  L7_uint32 serverTimeout;  /* Initialization value for aWhile timer when timing out Auth. Server */
  L7_uint32 maxReq;         /* Max EAP Request/Identity retransmits before time out Supplicant */
  L7_uint32 reAuthPeriod;   /* Number of seconds between periodic reauthentication */
  L7_BOOL   reAuthEnabled;  /* TRUE if reauthentication is enabled */
  L7_BOOL   keyTxEnabled;   /* TRUE if key transmission is enabled */

} dot1xPortCfgV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 dot1xAdminMode;     /* Global admin mode (SystemAuthControl) */
  L7_uint32 dot1xLogTraceMode;  /* Enable/disable log file tracing */
  dot1xPortCfgV2_t dot1xPortCfg[L7_DOT1X_INTF_MAX_COUNT_REL_4_1]; /* Per-port config info */
  L7_uint32 checkSum;           /* check sum of config file NOTE: needs to be last entry */
} dot1xCfgV2_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 dot1xAdminMode;    /* Global admin mode (SystemAuthControl) */
  L7_uint32 dot1xLogTraceMode; /* Enable/disable log file tracing */
  dot1xPortCfgV2_t dot1xPortCfg[L7_DOT1X_INTF_MAX_COUNT_REL_4_3]; /* Per-port config info */
  L7_uint32 checkSum;        /* check sum of config file NOTE: needs to be last entry */
} dot1xCfgV3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 dot1xAdminMode;    /* Global admin mode (SystemAuthControl) */
  L7_uint32 dot1xLogTraceMode; /* Enable/disable log file tracing */
  dot1xPortCfgV2_t dot1xPortCfg[L7_DOT1X_INTF_MAX_COUNT_REL_4_4]; /* Per-port config info */
  L7_uint32 checkSum;        /* check sum of config file NOTE: needs to be last entry */
} dot1xCfgV4_t;


typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 dot1xAdminMode;    /* Global admin mode (SystemAuthControl) */
  L7_uint32 dot1xLogTraceMode; /* Enable/disable log file tracing */
  L7_uint32  vlanAssignmentMode;/* Global mode to enable vlan assignment */
  dot1xPortCfgV2_t dot1xPortCfg[L7_DOT1X_INTF_MAX_COUNT_REL_4_4]; /* Per-port config info */
  L7_uint32 checkSum;        /* check sum of config file NOTE: needs to be last entry */
} dot1xCfgV5_t;


typedef struct 
{
  nimConfigID_t configId;   /* NIM config ID for this interface*/
  L7_DOT1X_PORT_DIRECTION_t adminControlledDirections;  /* Mgmt directional control setting (r/o) */
  L7_DOT1X_PORT_CONTROL_t   portControlMode;     /* Current control mode setting by mgmt */
  L7_uint32 quietPeriod;    /* Initialization value for quietWhile timer */
  L7_uint32 txPeriod;       /* Initialization value for txWhen timer */
  L7_uint32 suppTimeout;    /* Initialization value for aWhile timer when timing out Supplicant */
  L7_uint32 serverTimeout;  /* Initialization value for aWhile timer when timing out Auth. Server */
  L7_uint32 maxReq;         /* Max EAP Request/Identity retransmits before time out Supplicant */
  L7_uint32 reAuthPeriod;   /* Number of seconds between periodic reauthentication */
  L7_BOOL   reAuthEnabled;  /* TRUE if reauthentication is enabled */
  L7_BOOL   keyTxEnabled;   /* TRUE if key transmission is enabled */
  L7_uint32 guestVlanId;    /* Guest Vlan Id for the interface*/
  L7_uint32 guestVlanPeriod; /* Guest Vlan Period for the interface*/
} dot1xPortCfgV3_t;

typedef struct 
{
  L7_fileHdr_t           cfgHdr;
  L7_uint32              dot1xAdminMode;    /* Global admin mode (SystemAuthControl) */
  L7_uint32              dot1xLogTraceMode; /* Enable/disable log file tracing */
  L7_uint32              vlanAssignmentMode;/* Global mode to enable vlan assignment */
  dot1xPortCfgV3_t       dot1xPortCfg[L7_DOT1X_INTF_MAX_COUNT_REL_4_4]; /* Per-port config info */
  L7_uint32              dot1xGuestVlanSupplicantMode; /* Guest vlan supplicant mode*/
  L7_uint32              checkSum;        /* check sum of config file NOTE: needs to be last entry */
} dot1xCfgV6_t;

typedef struct dot1xPortCfgV7_s
{
  nimConfigID_t configId;   /* NIM config ID for this interface*/
  L7_DOT1X_PORT_DIRECTION_t adminControlledDirections;  /* Mgmt directional control setting (r/o) */
  L7_DOT1X_PORT_CONTROL_t   portControlMode;     /* Current control mode setting by mgmt */
  L7_uint32 quietPeriod;    /* Initialization value for quietWhile timer */
  L7_uint32 txPeriod;       /* Initialization value for txWhen timer */
  L7_uint32 suppTimeout;    /* Initialization value for aWhile timer when timing out Supplicant */
  L7_uint32 serverTimeout;  /* Initialization value for aWhile timer when timing out Auth. Server */
  L7_uint32 maxReq;         /* Max EAP Request/Identity retransmits before time out Supplicant */
  L7_uint32 reAuthPeriod;   /* Number of seconds between periodic reauthentication */
  L7_BOOL   reAuthEnabled;  /* TRUE if reauthentication is enabled */
  L7_BOOL   keyTxEnabled;   /* TRUE if key transmission is enabled */
  L7_uint32 guestVlanId;    /* Guest Vlan Id for the interface*/
  L7_uint32 guestVlanPeriod; /* Guest Vlan Period for the interface*/
  L7_uint32 maxUsers;        /*Maximum no. users in Mac-Based Authentication */
} dot1xPortCfgV7_t;

typedef struct dot1xCfgV7_s
{
  L7_fileHdr_t           cfgHdr;
  L7_uint32              dot1xAdminMode;    /* Global admin mode (SystemAuthControl) */
  L7_uint32              dot1xLogTraceMode; /* Enable/disable log file tracing */
  L7_uint32              vlanAssignmentMode;/* Global mode to enable vlan assignment */
  dot1xPortCfgV7_t       dot1xPortCfg[L7_DOT1X_INTF_MAX_COUNT]; /* Per-port config info */
  L7_uint32              dot1xGuestVlanSupplicantMode; /* Guest vlan supplicant mode*/
  L7_uint32              checkSum;        /* check sum of config file NOTE: needs to be last entry */
} dot1xCfgV7_t;

typedef dot1xCfg_t dot1xCfgV8_t;

#endif /* DOT1X_MIGRATE_H */
