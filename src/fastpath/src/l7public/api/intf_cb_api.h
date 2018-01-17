/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename intf_cb_api.h
*
* @purpose The purpose of this file is to have a central location for
*          common constants to be used by the interface call backs.
*
* @component cp
*
* @comments none
*
* @create 09/17/2007
*
* @author hshi
* @end
*
**********************************************************************/

#ifndef INCLUDE_INTF_CP_API_H
#define INCLUDE_INTF_CP_API_H

#include "captive_portal_commdefs.h"
#include "l3_addrdefs.h"


typedef enum
{
  CP_IPV4 = 1,
  CP_IPV6,
} CP_IP_TYPE_t;


typedef struct
{
    L7_uint8 redirIpAddrType;

    union
    {
        L7_IP_ADDR_t  redirIpv4Addr;
        L7_in6_addr_t redirIpv6Addr;
    }ipAddr;
} cpRedirIpAddr_t;


typedef enum
{
  CP_ENABLE,
  CP_DISABLE,
  CP_CLIENT_NOT_AUTH,
  CP_CLIENT_AUTH,
  CP_CLIENT_DEAUTH,
  CP_BLOCK,
  CP_UNBLOCK,
  CP_CLIENT_CONN,
  CP_AUTH_PORT,
  CP_AUTH_SECURE_PORT_1,
  CP_AUTH_SECURE_PORT_2
} CP_OP_t;


typedef struct txRxCount_s
{
  L7_uint64               bytesTransmitted;   
  L7_uint64               bytesReceived;      
  L7_uint64               packetsTransmitted; 
  L7_uint64               packetsReceived;    
} txRxCount_t;


/*
 * Type used for the Captive Portal Call Back
 */

typedef struct intfCpCallbacks
{
  L7_RC_t (*cpCapabilityGet) (L7_uint32 intIfNum,
                              L7_INTF_PARM_CP_TYPES_t  cpCapType,
                              L7_uint32 *cpCap);
  L7_RC_t (*cpCapabilitySet) (L7_uint32 intIfNum,
                              L7_INTF_PARM_CP_TYPES_t  cpCapType,
                              L7_uint32 cpCap);
  L7_RC_t (*cpEnableDisableFeature) (CP_OP_t cpCmd, L7_uint32 intIfNum, 
                                     cpRedirIpAddr_t redirIp, L7_uint32 redirIpMask, 
                                     L7_uchar8 intfMac[L7_ENET_MAC_ADDR_LEN]);
  L7_RC_t (*cpBlockUnblockAccess) (CP_OP_t cpCmd, L7_uint32 intIfNum);
  L7_RC_t (*cpAuthenUnauthenClient) (CP_OP_t cpCmd, L7_enetMacAddr_t clientMacAddr, 
                                     L7_uint32 upRate, L7_uint32 downRate);
  L7_RC_t (*cpDeauthenClient) (L7_enetMacAddr_t clientMacAddr);
  L7_RC_t (*cpClientStatisticsGet) (L7_enetMacAddr_t clientMacAddr, 
                                    txRxCount_t *clientCounter, L7_uint32 *timePassed);
  L7_RC_t (*cpSetAuthPort) (L7_uint32 intIfNum, L7_ushort16 authPortNum);
  L7_RC_t (*cpSetAuthSecurePort1) (L7_uint32 intIfNum, L7_ushort16 authSecurePort1Num);
  L7_RC_t (*cpSetAuthSecurePort2) (L7_uint32 intIfNum, L7_ushort16 authSecurePort2Num);
}intfCpCallbacks_t;

#endif
