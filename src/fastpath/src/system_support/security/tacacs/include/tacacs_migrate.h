/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename tacacs_migrate.h
*
* @purpose tacacs Configuration Migration
*
* @component tacacs
*
* @comments none
*
* @create  02/14/2007
*
* @author  I. Kiran
* @end
*
**********************************************************************/

#ifndef TACACS_MIGRATE_H
#define TACACS_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

typedef struct tacacServerCfgV1_s
{
  L7_uint32            ip;                /* server IP address */
  L7_ushort16          port;              /* TCP port for connection */
  L7_ushort16          priority;          /* priority to contact server, 0 is highest */
  L7_uchar8            singleConnection;  /* TBD - not implemented, no server support exists */
  L7_uchar8            timeout;           /* connection timeout in seconds */
  L7_uint32            sourceIP;          /* always 0, will support multiple outgoing interfaces for L3 */
  L7_uchar8            key[L7_TACACS_KEY_LEN_MAX+1]; /* encrypt/decrypt key, must match on TACACS server */
  L7_uchar8            useGblTimeout;     /* L7_TRUE to use global configuration */
  L7_uchar8            useGblSourceIP;    /* L7_TRUE to use global configuration */
  L7_uchar8            useGblKey;         /* L7_TRUE to use global configuration */
} tacacsServerCfgV1_t;

typedef struct tacacsCfgV1_s
{
  L7_fileHdr_t             header;
  L7_uint32                timeout;   /* global server connection timeout */
  L7_uint32                sourceIP;  /* always 0, to support multiple outgoing interfaces for L3 */
  L7_uchar8                key[L7_TACACS_KEY_LEN_MAX+1];  /* global encryption key */
  tacacsServerCfgV1_t        servers[L7_TACACS_MAX_SERVERS];
  L7_uint32                checkSum;
} tacacsCfgV1_t;


typedef tacacsCfg_t tacacsCfgV2_t;

void
tacacsMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* TACACS_MIGRATE_H */
