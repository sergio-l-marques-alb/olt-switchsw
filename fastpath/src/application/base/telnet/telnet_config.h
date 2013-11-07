/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   telnet_config.h
*
* @purpose    Outbound Telnet Implementation
*
* @component  Outbound Telnet Component
*
* @comments   none
*
* @create     02/27/2004
*
* @author     anindya
*
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/
#ifndef _TELNET_CONFIG_H
#define _TELNET_CONFIG_H

#define L7_TELNET_CFG_FILENAME       "telnetMap.cfg"
#define L7_TELNET_CFG_VER_1          0x1
#define L7_TELNET_CFG_VER_CURRENT    L7_TELNET_CFG_VER_1

/****************************************
*
*  Outbound Telnet Global Configuration Data                
*
*****************************************/

typedef struct telnetCfgData_s
{
  L7_uint32           telnetAdminMode;           /* Outbound Telnet Mode */
  L7_uint32           telnetMaxSessions;         /* Maximum no. of sessions allowed */
  L7_uint32           telnetNoOfActiveSessions;  /* No. of active sessions */
  L7_uint32           telnetTimeout;             /* Login inactivity timeout value */
} telnetCfgData_t;     

/****************************************
*
*  Outbound Telnet Configuration                
*
*****************************************/

typedef struct L7_telnetMapCfg_s 
{
  L7_fileHdr_t          cfgHdr;
  telnetCfgData_t       telnet;
  L7_uint32             checkSum;

} L7_telnetMapCfg_t;

#endif /* _TELNET_CONFIG_H */

