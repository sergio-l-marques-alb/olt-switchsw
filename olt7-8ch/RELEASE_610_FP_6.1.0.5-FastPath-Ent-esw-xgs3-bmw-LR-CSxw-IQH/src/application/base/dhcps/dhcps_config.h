/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   dhcps_config.h
*
* @purpose    DHCP Server Implementation
*
* @component  DHCP Server Component
*
* @comments   none
*
* @create     09/15/2003
*
* @author     athakur
* @end
*
**********************************************************************/
/*********************************************************************
 *
 ********************************************************************/
#ifndef _DHCPS_CONFIG_H
#define _DHCPS_CONFIG_H

#include "l7_common.h"


/****************************************
*
*  DHCP Server Global Configuration Data                
*
*****************************************/

typedef struct dhcpsCfgData_s
{
  L7_uint32           dhcpsAdminMode;     /* DHCP Server Mode           */
  L7_uint32           dhcpsPingPktNo;     /* No. of ping pkts           */
  L7_BOOL             dhcpConflictLogging; /* Conflict logging status */
  L7_BOOL             dhcpBootpAuto;   /* Bootp address assignment type */
} dhcpsCfgData_t;     

/*****************************************
*
* DHCP Server notification Data
*
******************************************/


typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID;
  L7_uint32 (*notify_dhcps_send_type)(L7_uint32 sendMessageType,
                                      L7_enetMacAddr_t chAddr,
                                      L7_uint32 ipAddr,
                                      L7_uint32 leaseTime);
} dhcpsSendMessageNotifyList_t;

/***********************************************************************
 * @purpose  Register a component to inform type of DHCP server message
 *           going out.
 *
 * @param    notifyFunctionList   pointer to structure with function pointer
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSendTypeNotifyRegister(L7_uint32 registrar_ID,
      L7_uint32 (*notify_func)(L7_uint32 sendMsgType,
                               L7_enetMacAddr_t chAddr,
                               L7_uint32 ipAddr,
                               L7_uint32 leaseTime));

/***********************************************************************
 * @purpose  deregister a registered component for DHCP server message.
 *
 * @param    registrar_ID to deregister
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t dhcpsSendTypeNotifyDeRegister(L7_uint32 registrar_ID);







/****************************************
*
*  DHCP Server Pool Configuration Data         
*
*****************************************/

typedef struct poolCfgData_s
{
  /* read-only (after pool allocation) */
  L7_char8    poolName[L7_DHCPS_POOL_NAME_MAXLEN];
  L7_IP_ADDR_t  ipAddr;
  L7_IP_MASK_t  ipMask;
  L7_uchar8     poolType;
# define          INACTIVE_POOL    L7_DHCPS_INACTIVE_POOL /* New pool. Yet to set binding */
# define          DYNAMIC_POOL     L7_DHCPS_DYNAMIC_POOL
# define          MANUAL_POOL      L7_DHCPS_MANUAL_POOL

/* Pool Types to support SNMP create operation */
# define          NOT_READY_POOL      L7_DHCPS_NOT_READY_POOL

  /* read-write */
  L7_IP_ADDR_t  defaultrouterAddrs[L7_DHCPS_DEFAULT_ROUTER_MAX];
  L7_IP_ADDR_t  DNSServerAddrs[L7_DHCPS_DNS_SERVER_MAX];
  L7_uint32     leaseTime;                /* Lease time in minutes  */

  /* Extra fields for a Manual pool */
  L7_uchar8     hostIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN];
  L7_uchar8     hostIdLength;
  L7_uchar8     hostAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uchar8     hostAddrtype; /* Hardware addr type, ethernet - 1 */
  L7_uchar8     hostAddrLength;
  L7_uchar8     hostName[L7_DHCPS_HOST_NAME_MAXLEN];

  /* Phase 2 fields*/
  L7_IP_ADDR_t  netBiosNameServer[L7_DHCPS_NETBIOS_NAME_SERVER_MAX]; /* netbios name server */
  L7_uchar8     netBiosNodeType;  /* Netbios node type */
  L7_IP_ADDR_t  nextServer; /* Next server address */
  L7_uchar8  domainName[L7_DHCPS_DOMAIN_NAME_MAXLEN];  /* Client domain name */
  L7_uchar8  clientBootFile[L7_DHCPS_BOOT_FILE_NAME_MAXLEN] ; /* Boot file name */
  L7_uchar8  options[L7_DHCPS_POOL_OPTION_BUFFER];   /* DHCP option, compact form*/
} poolCfgData_t;

/****************************************
*
*  DHCP Server Lease Data         
*
*****************************************/

typedef struct leaseCfgData_s
{
  /* read-only (after lease allocation) */
  L7_char8      poolName[L7_DHCPS_POOL_NAME_MAXLEN];
  L7_uchar8     clientIdLength;
  L7_uchar8     clientIdentifier[L7_DHCPS_CLIENT_ID_MAXLEN];
  L7_uchar8     hwAddr[L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uchar8     hAddrtype; /* Hardware addr type, ethernet -1 */
  L7_uchar8     hwAddrLength;
  L7_IP_ADDR_t  ipAddr;
  L7_IP_MASK_t  ipMask;

  /* read-write */
  L7_uint32     leaseTime;        /* Lease duration in minutes */
  L7_uchar8     state;
# define          FREE_LEASE       L7_DHCPS_FREE_LEASE 
# define          ACTIVE_LEASE     L7_DHCPS_ACTIVE_LEASE
# define          OFFERED_LEASE    L7_DHCPS_OFFERED_LEASE
# define          EXPIRED_LEASE    L7_DHCPS_EXPIRED_LEASE
# define          ABANDONED_LEASE  L7_DHCPS_ABANDONED_LEASE
  L7_BOOL       IsBootp;
  L7_uchar8     conflictDetectionMethod;
} leaseCfgData_t;

/****************************************
*
*  DHCP Server Excluded Address Data
*
*****************************************/
typedef struct excludedAddrCfgData_s 
{
  L7_IP_ADDR_t         fromAddr;
  L7_IP_ADDR_t         toAddr;
}excludedAddrCfgData_t;

/****************************************
*
*  DHCP Server Configuration & Lease File                
*
*****************************************/

#define L7_DHCPS_CFG_FILENAME       "dhcpsMap.cfg"
#define L7_DHCPS_CFG_VER_1          0x1
#define L7_DHCPS_CFG_VER_2          0x2
#define L7_DHCPS_CFG_VER_3          0x3
#define L7_DHCPS_CFG_VER_4          0x4
#define L7_DHCPS_CFG_VER_CURRENT    L7_DHCPS_CFG_VER_4

#define L7_DHCPS_LEASE_CFG_FILENAME       "dhcpsLeases.cfg"
#define L7_DHCPS_LEASE_CFG_VER_1          0x1
#define L7_DHCPS_LEASE_CFG_VER_2          0x2
#define L7_DHCPS_LEASE_CFG_VER_3          0x3
#define L7_DHCPS_LEASE_CFG_VER_CURRENT    L7_DHCPS_LEASE_CFG_VER_3

typedef struct L7_dhcpsMapCfg_s 
{
  L7_fileHdr_t          cfgHdr;
  dhcpsCfgData_t        dhcps;
  poolCfgData_t         pool[L7_DHCPS_MAX_POOL_NUMBER]; /* Max automatic pool                                                     + No of manual pools */
  excludedAddrCfgData_t excludedAddrs[L7_DHCPS_MAX_EXCLUDE_NUMBER]; 
  L7_uint32             checkSum;

} L7_dhcpsMapCfg_t;

typedef struct L7_dhcpsMapLeaseCfg_s
{
  L7_fileHdr_t          cfgHdr;
  leaseCfgData_t        lease[L7_DHCPS_MAX_LEASE_NUMBER];
  L7_uint32             checkSum;

} L7_dhcpsMapLeaseCfg_t;



#endif /* _DHCPS_CONFIG_H */

