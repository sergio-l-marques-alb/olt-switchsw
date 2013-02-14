/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename tacacs_cfg.h
*
* @purpose TACACS+ Client configuration header
*
* @component tacacs+
*
* @comments none
*
* @create 03/10/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_TACACS_CFG_H
#define INCLUDE_TACACS_CFG_H

#include "tacacs_exports.h"

#define L7_TACACS_CFG_FILE_NAME      "tacacs.cfg"
#define L7_TACACS_VER_1              0x01
#define L7_TACACS_VER_2              0x02
#define L7_TACACS_CFG_VER_CURRENT    L7_TACACS_VER_2

typedef struct tacacServerCfg_s
{
  L7_uint32            ip;                /* server IP address */
  dnsHost_t            hostName;          /* Server Host Name */
  L7_ushort16          port;              /* TCP port for connection */
  L7_ushort16          priority;          /* priority to contact server, 0 is highest */
  L7_uchar8            singleConnection;  /* TBD - not implemented, no server support exists */
  L7_uchar8            timeout;           /* connection timeout in seconds */
  L7_uint32            sourceIP;          /* always 0, will support multiple outgoing interfaces for L3 */
  L7_uchar8            key[L7_TACACS_KEY_LEN_MAX+1]; /* encrypt/decrypt key, must match on TACACS server */
  L7_uchar8            useGblTimeout;     /* L7_TRUE to use global configuration */
  L7_uchar8            useGblSourceIP;    /* L7_TRUE to use global configuration */
  L7_uchar8            useGblKey;         /* L7_TRUE to use global configuration */
} tacacsServerCfg_t;

typedef struct tacacsCfg_s
{
  L7_fileHdr_t             header;
  L7_uint32                timeout;   /* global server connection timeout */
  L7_uint32                sourceIP;  /* always 0, to support multiple outgoing interfaces for L3 */
  L7_uchar8                key[L7_TACACS_KEY_LEN_MAX+1];  /* global encryption key */
  tacacsServerCfg_t        servers[L7_TACACS_MAX_SERVERS];
  L7_uint32                checkSum;
} tacacsCfg_t;

/******************************************************
**        Internal function prototypes               **
******************************************************/

L7_RC_t tacacsInit();
void    tacacsInitUndo(void);
L7_RC_t tacacsSave(void);
L7_RC_t tacacsRestore(void);
L7_BOOL tacacsHasDataChanged(void);
void tacacsResetDataChanged(void);
void    tacacsBuildDefaultServerConfigData(L7_uint32 version, tacacsServerCfg_t *serverCfg);
void    tacacsBuildDefaultConfigData(L7_uint32 version);
void    tacacsApplyConfigData(void);

#endif /* INCLUDE_TACACS_CFG_H */
