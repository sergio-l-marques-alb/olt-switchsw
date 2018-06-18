/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*  All Rights Reserved.
*
*
**********************************************************************
*
* @filename     cp_trap_api.c
*
* @purpose      Captive Portal Client API functions
*
* @component    Captive Portal
*
* @comments     none
*
* @create       11/28/07
*
* @author       dcaugherty
*
* @end
*
**********************************************************************/

#include <string.h>
#include "osapi.h"
#include "log.h"
#include "captive_portal_commdefs.h"
#include "cpdm.h"
#include "cpdm_api.h"
#include "cp_trap_api.h"
#include "trap_captive_portal_api.h"


typedef L7_RC_t (*trapMgrConnFn)( L7_enetMacAddr_t, L7_uint32,
                                  L7_enetMacAddr_t, L7_uint32,
                                  L7_uint32);
typedef L7_RC_t (*trapMgrFn)( void );


static L7_uchar8 * log_fmt  = "No required %s supplied for SNMP trap (%s)";
static L7_uchar8 * failed   = "CP: Failed to send trap: %s";
static L7_uchar8 * no_flags = "Could not retrieve trap flags";

/*********************************************************************
*
* @purpose  Send SNMP trap for client connection
*
* @param    *macAddr       @b{(input)} AP MAC address
* @param    ipAddr         @b{(input)} client IP address
* @param    *switchMacAddr @b{(input)} MAC address of authenticating switch
* @param    cpId           @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex @b{(input)} ifIndex of port on authenticating switch
*
* @returns  void
*
* @end
*********************************************************************/

static
void cpHandleConnectionTrap(L7_enetMacAddr_t * macAddr, L7_uint32 ipAddr,
                            L7_enetMacAddr_t * switchMacAddr, cpId_t cpId,
                            L7_uint32 ifIndex, L7_BOOL bConnected)
{
  L7_uchar8 * trapType;
  trapMgrConnFn fn_to_call = L7_NULLPTR;
  CP_TRAP_FLAGS_t flags;
  L7_uint32 mode;

  if (L7_TRUE == bConnected)
  {
    trapType  = "Client connect";
    flags = CP_TRAP_CLIENT_CONNECTED;
    fn_to_call = trapMgrCaptivePortalClientConnectTrap;
  }
  else
  {
    trapType = "Client disconnect";
    flags = CP_TRAP_CLIENT_DISCONNECTED;
    fn_to_call = trapMgrCaptivePortalClientDisconnectTrap;
  }

  if (L7_NULLPTR == macAddr)
  {
    CP_DLOG(CPD_LEVEL_LOG, log_fmt, "MAC address", trapType);
    return;
  }

  if (L7_NULLPTR == switchMacAddr)
  {
    CP_DLOG(CPD_LEVEL_LOG, log_fmt, "switch MAC", trapType);
    return;
  }

  if (L7_SUCCESS != cpdmGlobalTrapModeGet(flags, &mode))
  {
    CP_DLOG(CPD_LEVEL_LOG, no_flags);
  }
  else if ((L7_ENABLE == mode) &&
           (L7_SUCCESS != fn_to_call(*macAddr, ipAddr, *switchMacAddr,
             (L7_uint32) cpId, ifIndex)))
  {
    CP_DLOG(CPD_LEVEL_LOG, failed, trapType);
  }
}


/*********************************************************************
*
* @purpose  Send SNMP trap for client connection
*
* @param    *macAddr       @b{(input)} AP MAC address
* @param    ipAddr         @b{(input)} client IP address
* @param    *switchMacAddr @b{(input)} MAC address of authenticating switch
* @param    cpId           @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex @b{(input)} ifIndex of port on authenticating switch
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapClientConnected(L7_enetMacAddr_t * macAddr, L7_uint32 ipAddr,
                            L7_enetMacAddr_t * switchMacAddr, cpId_t cpId,
                            L7_uint32 ifIndex)
{
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(ifIndex, L7_SYSNAME, ifName);

  if (macAddr && switchMacAddr)
  {
      CP_DLOG (CPD_LEVEL_DEFAULT,
               "Captive Portal Client Connected. MAC: %02x:%02x:%02x:%02x:%02x:%02x "
               "IP: %d.%d.%d.%d "
               "Switch MAC: %02x:%02x:%02x:%02x:%02x:%02x "
               "CP-ID: %d IfIndex: %d, %s",
               macAddr->addr[0],
               macAddr->addr[1],
               macAddr->addr[2],
               macAddr->addr[3],
               macAddr->addr[4],
               macAddr->addr[5],
               (ipAddr >> 24) & 0xff,
               (ipAddr >> 16) & 0xff,
               (ipAddr >> 8) & 0xff,
               (ipAddr) & 0xff,
               switchMacAddr->addr[0],
               switchMacAddr->addr[1],
               switchMacAddr->addr[2],
               switchMacAddr->addr[3],
               switchMacAddr->addr[4],
               switchMacAddr->addr[5],
               cpId,
               ifIndex, ifName
               );


  }
  cpHandleConnectionTrap(macAddr, ipAddr, switchMacAddr, cpId,
                         ifIndex, L7_TRUE);
}


/*********************************************************************
*
* @purpose  Send SNMP trap for client disconnection
*
* @param    *macAddr       @b{(input)} AP MAC address
* @param    ipAddr         @b{(input)} client IP address
* @param    *switchMacAddr @b{(input)} MAC address of authenticating switch
* @param    cpId           @b{(input)} captive portal instance handling the authentication
* @param    cpAssocIfIndex @b{(input)} ifIndex of port on authenticating switch
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapClientDisconnected(L7_enetMacAddr_t * macAddr, L7_uint32 ipAddr,
                              L7_enetMacAddr_t * switchMacAddr, cpId_t cpId,
                              L7_uint32 ifIndex)
{
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(ifIndex, L7_SYSNAME, ifName);

    if (macAddr && switchMacAddr)
    {
      CP_DLOG (CPD_LEVEL_DEFAULT,
             "Captive Portal Client Disconnected. MAC: %02x:%02x:%02x:%02x:%02x:%02x "
             "IP: %d.%d.%d.%d "
             "Switch MAC: %02x:%02x:%02x:%02x:%02x:%02x "
             "CP-ID: %d IfIndex: %d, %s",
             macAddr->addr[0],
             macAddr->addr[1],
             macAddr->addr[2],
             macAddr->addr[3],
             macAddr->addr[4],
             macAddr->addr[5],
             (ipAddr >> 24) & 0xff,
             (ipAddr >> 16) & 0xff,
             (ipAddr >> 8) & 0xff,
             (ipAddr) & 0xff,
             switchMacAddr->addr[0],
             switchMacAddr->addr[1],
             switchMacAddr->addr[2],
             switchMacAddr->addr[3],
             switchMacAddr->addr[4],
             switchMacAddr->addr[5],
             cpId,
             ifIndex, ifName
             );
    }
  cpHandleConnectionTrap(macAddr, ipAddr, switchMacAddr,
                         cpId, ifIndex, L7_FALSE);
}



/*********************************************************************
*
* @purpose  Send a Captive Portal Client Authentication Failure trap
*
* @param    macAddr        @b{(input)} client MAC address
* @param    ipAddr         @b{(input)} client IP address
* @param    userName       @b{(input)} client user name
* @param    switchMacAddr  @b{(input)} MAC address of authenticating switch
* @param    cpId           @b{(input)} captive portal instance handling the authentication
* @param    ifIndex        @b{(input)} ifIndex of port on authenticating switch
* @param    attempts       @b{(input)} number of connection attempts
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
void
cpTrapClientAuthFailure( L7_enetMacAddr_t * macAddr, L7_uint32 ipAddr,
                         L7_uchar8 *userName, L7_enetMacAddr_t * switchMacAddr,
                         cpId_t cpId, L7_uint32 ifIndex,
                         L7_uint32 attempts)
{
  L7_uchar8 * trapType = "Client authentication failure";
  L7_uint32 mode;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(ifIndex, L7_SYSNAME, ifName);


  if (macAddr && userName && switchMacAddr)
  {
      CP_DLOG (CPD_LEVEL_DEFAULT,
             "Captive Portal Client Auth Failure. MAC: %02x:%02x:%02x:%02x:%02x:%02x "
             "IP: %d.%d.%d.%d "
             "User: %s "
             "Switch MAC: %02x:%02x:%02x:%02x:%02x:%02x "
             "CP-ID: %d IfIndex: %d, %s "
             "Attempts: %d",
             macAddr->addr[0],
             macAddr->addr[1],
             macAddr->addr[2],
             macAddr->addr[3],
             macAddr->addr[4],
             macAddr->addr[5],
             (ipAddr >> 24) & 0xff,
             (ipAddr >> 16) & 0xff,
             (ipAddr >> 8) & 0xff,
             (ipAddr) & 0xff,
             userName,
             switchMacAddr->addr[0],
             switchMacAddr->addr[1],
             switchMacAddr->addr[2],
             switchMacAddr->addr[3],
             switchMacAddr->addr[4],
             switchMacAddr->addr[5],
             cpId,
             ifIndex, ifName,
             attempts
             );
  }

  if (L7_NULLPTR == macAddr)
  {
    CP_DLOG(CPD_LEVEL_LOG, log_fmt, "MAC address", trapType);
    return;
  }

  if (L7_NULLPTR == switchMacAddr)
  {
    CP_DLOG(CPD_LEVEL_LOG, log_fmt, "switch MAC", trapType);
    return;
  }

  if (L7_NULLPTR == userName)
  {
    CP_DLOG(CPD_LEVEL_LOG, log_fmt, "User name", trapType);
    return;
  }

  if (L7_SUCCESS != cpdmGlobalTrapModeGet(CP_TRAP_AUTH_FAILURE, &mode))
  {
    CP_DLOG(CPD_LEVEL_LOG, no_flags);
  }
  else if ((L7_ENABLE == mode) &&
     (L7_SUCCESS !=
            trapMgrCaptivePortalClientAuthFailureTrap(*macAddr, ipAddr, userName,
                  *switchMacAddr, (L7_uint32) cpId,
                  ifIndex, attempts)))
  {
    CP_DLOG(CPD_LEVEL_LOG, failed, trapType);
  }
}



/*********************************************************************
*
* @purpose  Generic trap-sending routine
*
* @param    trapMgrFn fn_to_call  @b{(input)} trap API to invoke
* @param    CP_TRAP_FLAGS_t flags @b{(input)} flags for trap
* @param    L7_uchar8 * description  @b{(input)} trap description
*
* @returns  void
*
* @notes    Cloning code SUCKS.
*
* @end
*********************************************************************/


static
void cpSendTrap(trapMgrFn fn_to_call,
    CP_TRAP_FLAGS_t flags,
                L7_uchar8 *description)
{
  L7_uint32 mode;

  if (L7_NULLPTR == description)
  {
    description = "*no description*";
  }

  if (L7_SUCCESS != cpdmGlobalTrapModeGet(flags, &mode))
  {
    CP_DLOG(CPD_LEVEL_LOG, no_flags);
  }
  else if ((L7_ENABLE == mode) &&
           (L7_SUCCESS != fn_to_call()))
  {
    CP_DLOG(CPD_LEVEL_LOG, failed, description);
  }
}



/*********************************************************************
*
* @purpose  Send SNMP trap for wrapping of authentication
*           failure list
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapClientConnectionDatabaseFull(void)
{
  CP_DLOG (CPD_LEVEL_DEFAULT,
           "Captive Portal Client Connection Database is Full.\n");

  cpSendTrap(trapMgrCaptivePortalClientConnDatabaseFullTrap,
             CP_TRAP_CONNECTION_DB_FULL,
             "Max client connections reached");
}


/*********************************************************************
*
* @purpose  Send SNMP trap for wrapping of authentication
*           failure list
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapAuthFailureLogWrap(void)
{
  cpSendTrap(trapMgrCaptivePortalAuthFailureLogWrapTrap,
             CP_TRAP_AUTH_FAILURE_LOG_WRAP,
             "Authentication failure log wrapped");
}


/*********************************************************************
*
* @purpose  Send SNMP trap for wrapping of authentication
*           failure list
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/

void cpTrapActivityLogWrap(void)
{
  cpSendTrap(trapMgrCaptivePortalActivityLogWrapTrap,
             CP_TRAP_ACTIVITY_LOG_WRAP,
             "Activity log wrapped");
}

/*
** Test Code.
*/

/*********************************************************************
*
* @purpose  Test  trap generation and syslog generation routines.
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/
int cpTrapLogTest(void)
{
  L7_enetMacAddr_t mac1 = {{0x00, 0x00, 0x11, 0xa, 0x4, 0x1}};
  L7_enetMacAddr_t mac2 = {{0x00, 0x02, 0x10, 0xbb, 0x55, 0x10}};

  cpTrapClientConnected(&mac1, 0x0c02020a, &mac2, 3, 22);
  cpTrapClientDisconnected(&mac1, 0x0b01010a, &mac2, 1, 10);
  cpTrapClientAuthFailure(&mac1, 0x0a010102, "My Name", &mac2, 2, 22, 10);
  cpTrapClientConnectionDatabaseFull();

  return 0;
}
