
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename radius_migrate.h
*
* @purpose radius Configuration Migration
*
* @component radius
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#ifndef RADIUS_MIGRATE_H
#define RADIUS_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "radius_include.h"

/* Radius server config structure */
typedef struct {
   L7_uint32               radiusServerConfigIpAddress;
   L7_ushort16             radiusServerConfigUdpPort;
   radiusServerType_t      radiusServerConfigServerType; /* RADIUS_SERVER_TYPE_AUTH,
                                                            RADIUS_SERVER_TYPE_ACCT */ 
   L7_char8                radiusServerConfigSecret[L7_RADIUS_MAX_SECRET+1];
   L7_BOOL                 radiusServerIsSecretConfigured; /* L7_TRUE if secret has been configured
                                                              L7_FALSE if no secret configured */
   radiusServerRowStatus_t radiusServerConfigRowStatus; /* RADIUS_SERVER_ACTIVE,
                                                           RADIUS_SERVER_NOTREADY,
                                                           RADIUS_SERVER_DESTROY,
                                                           RADIUS_SERVER_NOTCONFIGURED */
   L7_RADIUS_SERVER_ENTRY_TYPE_t radiusServerConfigServerEntryType; /* RADIUS_SERVER_ENTRY_TYPE_PRIMARY,
                                                                       RADIUS_SERVER_ENTRY_TYPE_SECONDARY */
   L7_uint32               radiusServerConfigPriority; /* Used but not settable */
   L7_uint32               incMsgAuthMode; /* L7_ENABLE if to be included in all Access-Requests,
                                              L7_DISABLE if not to be included */

} radiusServerConfigEntry_V1_t;

/* Common RADIUS params */
typedef struct 
{
 L7_uint32                 authOperMode;
 L7_uint32                 acctAdminMode;
 L7_uint32                 maxNumRetrans;
 L7_uint32                 timeOutDuration;
 radiusServerConfigEntry_V1_t serverConfigEntries[L7_RADIUS_MAX_SERVERS];
 L7_uint32                 retryPrimaryTime;
} radiusClient_V1_t;


typedef struct
{
  radiusClient_V1_t radiusClient;
} radiusCfgData_V1_t;

typedef struct
{
  L7_fileHdr_t       hdr;
  radiusCfgData_V1_t cfg;
  L7_uint32          checkSum;

} radiusCfgV1_t;

typedef radiusCfg_t radiusCfgV3_t;

#endif /* RADIUS_MIGRATE_H */
