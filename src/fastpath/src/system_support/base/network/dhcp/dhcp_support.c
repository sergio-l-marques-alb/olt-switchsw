/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dhcp_support.c
*
* @purpose dhcp main file
*
* @component dhcp
*
* @comments Ported from ecos dhcp_support.c (refer Next Header)
*
* @create 17/05/2001
*
* @author  ported by Paresh Jain
*
* @end
*
**********************************************************************/

/*==========================================================================

        dhcp_support.c

        Support code == friendly API for DHCP client

  ==========================================================================
  ####COPYRIGHTBEGIN####

   -------------------------------------------
   The contents of this file are subject to the Red Hat eCos Public License
   Version 1.1 (the "License"); you may not use this file except in
   compliance with the License.  You may obtain a copy of the License at
   http:  www.redhat.com/

   Software distributed under the License is distributed on an "AS IS"
   basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
   License for the specific language governing rights and limitations under
   the License.

   The Original Code is eCos - Embedded Configurable Operating System,
   released September 30, 1998.

   The Initial Developer of the Original Code is Red Hat.
   Portions created by Red Hat are
   Copyright (C) 1998, 1999, 2000 Red Hat, Inc.
   All Rights Reserved.
   -------------------------------------------

  ####COPYRIGHTEND####
  ==========================================================================
  #####DESCRIPTIONBEGIN####

   Author(s):   hmt
   Contributors: gthomas
   Date:        2000-07-01
   Purpose:     DHCP support
   Description:

  ####DESCRIPTIONEND####

  ========================================================================*/

#include "l7_common.h"
#include "usmdb_common.h"
#include "l7netapi.h"
#include "datatypes.h"
#include "default_cnfgr.h"

#include "osapi.h"
#include "osapi_support.h"
#include "simapi.h"
#include "log.h"
#include "l7_dhcp.h"
#include "dhcp_debug.h"
#include "dtlapi.h"
#include "bspapi.h"
#include "dhcps_exports.h"
#include "usmdb_util_api.h"
#include "l7_cnfgr_api.h"
#include "dhcp_map.h"
#include "rto_api.h"
#include "ipstk_api.h"

#ifdef _L7_OS_ECOS_
#include <sys/ioctl.h>
#endif

/* eth0 stands for service port and eth1 stands for network port */
struct bootp_serverinfo l7_eth0_bootp_data;
struct bootp_serverinfo l7_eth1_bootp_data;

/* Can someone tell me how these state variables are related to the DHCP
 * state of an interface? Are these always true if the DCHP is in BOUND state?
 * If so, what do these tell me that the state does not tell me? */
L7_BOOL   l7_eth0_up;
L7_BOOL   l7_eth1_up;

/* DHCP client state as defined in RFC 2131 section 4.4 for service port (eth0)
 * and network port (eth1). */
L7_short16   l7_eth0_dhcpstate = 0;
L7_short16   l7_eth1_dhcpstate = 0;        /* why would this not be INIT state? */

/* Information on our lease for each management interface */
dhcpLeaseInfo_t l7_eth0_lease;
dhcpLeaseInfo_t l7_eth1_lease;

L7_int32 dhcpv4client_task_id;

/* Set to L7_SYSCONFIG_MODE_COMPLETE when we have an IPv4 address.
 * Not sure this tells us anything we don't already know from the state. */
L7_uint32 servPortConfigStatus = L7_SYSCONFIG_MODE_NOT_COMPLETE;
L7_uint32 networkConfigStatus = L7_SYSCONFIG_MODE_NOT_COMPLETE;

/* L7_TRUE if the last restart was warm */
L7_BOOL dhcpcWarmRestart = L7_FALSE;

/* do_dhcp() and other functions access shared global data and modify
 * the linux IP stack routing table. There are at least four threads
 * involved:  the L7_dhcp thread, UI threads that invoke do_dhcp()
 * through l7_dhcp_release(), the Configurator thread that calls
 * l7_dhcp_release() on p1 uninit, and OSAPI timer thread running
 * the alarm_function(), which modifies lease data. Protocol operation
 * on the two management interfaces is independent. The following two
 * semaphores control access to the do_dhcp() code and the global data
 * it manipulates. They should be taken
 * before a thread runs this code or accesses the interface's global data,
 * and given only after the protocol operation is complete. */
void *servPortSema = NULL;
void *networkPortSema = NULL;

static L7_RC_t
dhcpClientIntfInfoInit (L7_uint32 intIfNum,
                        dhcpClientInfo_t *clientIntfInfo,
                        L7_MGMT_PORT_TYPE_t mgmtPortType);

L7_RC_t
dhcpClientReleaseInitiate (L7_uint32 intIfNum,
                           L7_MGMT_PORT_TYPE_t mgmtPortType);

/**************************************************************************
* @purpose  Extract parameters from a given DHCP message.
*
* @param    dhcpParams   (input)  a DCHP message received from a DHCP server
* @param    networkParams   (output)  parameters extracted from the DCHP message
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Reworked for @2975.
*
*           Sets client IP address to yipaddr. If a default address is set on
*           the interface, the caller should override this value.
*
* @end
*************************************************************************/
L7_RC_t getParamsFromDhcpStruct(struct bootp *dhcpParams, L7_bootp_dhcp_t *networkParams)
{
  L7_uint32 netMask    = L7_NULL;
  L7_uint32 siaddr     = L7_NULL;
  L7_uint32 tftpServerIpAddr = L7_NULL;
  L7_uint32 dnsServerIpAddr[L7_DNS_NAME_SERVER_ENTRIES]  = {L7_NULL};
  L7_uint32 count;
  L7_uchar8 bootFileName[BOOTP_FILE_LEN + 1];
  L7_uchar8 tftpServerName[BOOTP_SNAME_LEN + 1];
  L7_uint32 defaultGateway[L7_DHCPS_DEFAULT_ROUTER_MAX];

  L7_BOOL   tagOptOverExists = L7_FALSE;
  dhcpOptionOverload optionOverloadVal = L7_NULL;

  memset(bootFileName,   0, BOOTP_FILE_LEN + 1);
  memset(tftpServerName, 0, BOOTP_SNAME_LEN + 1);
  memset(defaultGateway, 0, L7_DHCPS_DEFAULT_ROUTER_MAX);
  memset(networkParams,  0, sizeof(L7_bootp_dhcp_t));

  /* Get subnet mask & default gw from bootp packet*/
  if (l7_get_bootp_option( dhcpParams, TAG_SUBNET_MASK, &netMask,  sizeof(netMask) ) != L7_TRUE)
    netMask = osapiHtonl(0xFFFFFF00); /* At least it's not uninitialized */
  if (l7_get_bootp_option( dhcpParams, TAG_GATEWAY, (void *)defaultGateway, sizeof(defaultGateway)) != L7_TRUE)
    defaultGateway[0] = 0;
  if (l7_get_bootp_option( dhcpParams, TAG_DOMAIN_SERVER, dnsServerIpAddr, sizeof(dnsServerIpAddr) )!= L7_TRUE)
  {
      dnsServerIpAddr[0] = 0;
  }
  if (l7_get_bootp_option( dhcpParams, TAG_TFTP_SERVER, &tftpServerIpAddr, sizeof(tftpServerIpAddr) ) != L7_TRUE)
  {
    tftpServerIpAddr = L7_NULL;
  }

  memcpy(&siaddr, &(dhcpParams->bp_siaddr), sizeof(dhcpParams->bp_siaddr));

  if (l7_get_bootp_option( dhcpParams, TAG_DHCP_OPTOVER, &optionOverloadVal, sizeof(optionOverloadVal) ) == L7_TRUE)
  {
    tagOptOverExists = L7_TRUE;
  }

  /* Start look for TFTP server hostname */
  if ( tagOptOverExists == L7_TRUE )
  {
    if ( optionOverloadVal != DHCP_OPTION_OVERLOAD_SNAME &&
         optionOverloadVal != DHCP_OPTION_OVERLOAD_SNAME_BOOTFILE &&
         strlen(dhcpParams->bp_sname) != L7_NULL )
    {
      osapiStrncpy(networkParams->sname, dhcpParams->bp_sname, BOOTP_SNAME_LEN);
    }
  }

  else if ( strlen(dhcpParams->bp_sname) != L7_NULL )
  {
    osapiStrncpy(networkParams->sname, dhcpParams->bp_sname, BOOTP_SNAME_LEN);
  }

  if (l7_get_bootp_option( dhcpParams, TAG_TFTP_SERVER_NAME, tftpServerName, sizeof(tftpServerName) ) == L7_TRUE)
  {
    osapiStrncpy(networkParams->optTftpSvrName, tftpServerName, strlen(tftpServerName));
  }
  /* Finish look for TFTP server hostname */

  /* Start look for boot file name */
  if ( tagOptOverExists == L7_TRUE )
  {
    if ( optionOverloadVal != DHCP_OPTION_OVERLOAD_BOOTFILE &&
         optionOverloadVal != DHCP_OPTION_OVERLOAD_SNAME_BOOTFILE &&
         strlen(dhcpParams->bp_file) != L7_NULL )
    {
      osapiStrncpy(networkParams->bootFile, dhcpParams->bp_file, BOOTP_FILE_LEN);
    }
  }

  else if ( strlen(dhcpParams->bp_file) != L7_NULL )
  {
    osapiStrncpy(networkParams->bootFile, dhcpParams->bp_file, BOOTP_FILE_LEN);
  }

  if (l7_get_bootp_option( dhcpParams, TAG_BOOT_FILENAME, bootFileName, sizeof(bootFileName) ) == L7_TRUE)
  {
    osapiStrncpy(networkParams->optBootFile, bootFileName, strlen(bootFileName));
  }
  /* Finish look for boot file name */

  networkParams->ip = osapiNtohl(dhcpParams->bp_yiaddr.s_addr);
  networkParams->netMask = osapiNtohl(netMask);
  networkParams->gateway = osapiNtohl(defaultGateway[0]);
  count = 0;
  networkParams->optDnsServerIpAddr[count]  = osapiNtohl(dnsServerIpAddr[count]);
  for (count=1; count<L7_DNS_NAME_SERVER_ENTRIES; count++)
  {
    if (dnsServerIpAddr[count] == 0)
    {
      break;
    }
    networkParams->optDnsServerIpAddr[count]  = osapiNtohl(dnsServerIpAddr[count]);
  }

  networkParams->siaddr = osapiNtohl(siaddr);
  networkParams->optTftpSvrIpAddr = osapiNtohl(tftpServerIpAddr);

  /* Set offered parameters mask */
  if (networkParams->ip != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_IP_ADDR;
  }

  if (networkParams->netMask != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_NETMASK;
  }

  if (networkParams->gateway != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_GATEWAY;
  }

  if (strlen(networkParams->sname) != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_SNAME;
  }

  if (strlen(networkParams->optTftpSvrName) != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_OPTION_TFTP_SVR_NAME;
  }

  if (strlen(networkParams->bootFile) != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_BOOTFILE;
  }

  if (strlen(networkParams->optBootFile) != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_OPTION_BOOTFILE;
  }

  if (networkParams->optDnsServerIpAddr[0] != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_OPTION_DNS_SVR_ADDR;
  }

  if (networkParams->siaddr != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_SIADDR;
  }

  if (networkParams->optTftpSvrIpAddr != L7_NULL)
  {
    networkParams->offeredOptionsMask |= BOOTP_DHCP_OPTION_TFTP_SVR_ADDR;
  }

  /* Verify the validity of the ip address, subnet mask & gateway together */
  return usmDbIpInfoValidate(networkParams->ip, networkParams->netMask, networkParams->gateway);
}

/**************************************************************************
* @purpose  Print dhcp parameters from specified bootp structure
*
* @param    bootp      DHCP structure
* @param    L7_uint8   port type
*
* @returns  L7_SUCCESS or L7_FAILURE
*
*
* @end
*************************************************************************/
L7_int32 dhcpDebugPortParamsPrint(struct bootp *dhcpParams, L7_uint8 portType)
{
  L7_bootp_dhcp_t networkParams;
  L7_uchar8 ipString[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 count =0;

  memset(&networkParams, 0, sizeof(L7_bootp_dhcp_t));
  memset(ipString, 0, sizeof(ipString));

  if (getParamsFromDhcpStruct(dhcpParams, &networkParams) != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to get DHCP Parameters");
    return L7_FAILURE;
  }

  if( portType == DHCP_SERVICE_PORT ) sysapiPrintf("Service port settings:\n");
  else
    if( portType == DHCP_NETWORK_PORT ) sysapiPrintf("Network port settings:\n");
  else
    return L7_FAILURE;

  osapiInetNtoa(networkParams.ip, ipString);
  sysapiPrintf("DHCP offered ip address %s \n", ipString);

  osapiInetNtoa(networkParams.netMask, ipString);
  sysapiPrintf("DHCP offered netMask %s \n", ipString);

  osapiInetNtoa(networkParams.gateway, ipString);
  sysapiPrintf("DHCP offered gateway %s \n", ipString);

  sysapiPrintf("DHCP offered DNS server address ");
  for(count=0;count<L7_DNS_NAME_SERVER_ENTRIES;count++)
  {
    if ((networkParams.optDnsServerIpAddr[count]!= 0) || (count==0))
    {
      osapiInetNtoa(networkParams.optDnsServerIpAddr[count], ipString);
      sysapiPrintf("%s ",ipString);
    }
    else
    {
      break;
    }
  }

  osapiInetNtoa(networkParams.siaddr, ipString);
  sysapiPrintf("\nDHCP offered TFTP server address %s \n", ipString);

  osapiInetNtoa(networkParams.optTftpSvrIpAddr, ipString);
  sysapiPrintf("DHCP offered TFTP option server address %s \n", ipString);

  sysapiPrintf("DHCP offered TFTP server name %s \n", networkParams.sname);
  sysapiPrintf("DHCP offered TFTP option server name %s \n", networkParams.optTftpSvrName);
  sysapiPrintf("DHCP offered boot file %s \n", networkParams.bootFile);
  sysapiPrintf("DHCP offered option boot file %s \n\n", networkParams.optBootFile);

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Print DHCP parameters in DHCP message received from the
*           DHCP server on both management interfaces.
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/
L7_RC_t dhcpDebugParamsPrint(void)
{
  dhcpDebugPortParamsPrint((struct bootp *)&l7_eth0_bootp_data, DHCP_SERVICE_PORT);
  dhcpDebugPortParamsPrint((struct bootp *)&l7_eth1_bootp_data, DHCP_NETWORK_PORT);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  If there is a default IP address for the service port, get it.
*
* @param    void
*
* @returns  default IP address, or 0 if no default is set
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dhcpConfiguredServPortAddrGet(void)
{
  return simConfiguredServPortIPAddrGet();
}

/*********************************************************************
* @purpose  Determine if the DHCP client wants to request a specific
*           IPv4 address from the server. If a stack is restarting
*           and there is a checkpointed IPv4 address, request that address.
*           Otherwise, if the client previously had a DHCP address,
*           request that one.
*
* @param    prevIpAddr - Address this client previously learned from a
*                        DHCP server.
*
* @returns  IPv4 address to request, or 0 if no preference
*
* @notes    This function is only called when DHCP is enabled on the
*           service port and the client is not in the BOUND state on
*           the service port. So if SIM has an address on this interface,
*           it must either be a default address or a checkpointed address.
*
* @end
*********************************************************************/
L7_uint32 dhcpRequestedServPortAddrGet(L7_uint32 prevIpAddr)
{
  /* If this is a warm restart, request the checkpointed address. */
  if (dhcpcWarmRestart && simIsServPortAddrCheckpointed())
  {
    return simGetServPortIPAddr();
  }

  /* No checkpointed address. Use the address this DHCP client previously
   * received from the DHCP server. */
  return prevIpAddr;
}

/*********************************************************************
* @purpose  If there is a configured IP address for the network port, get it.
*
* @param    void
*
* @returns  configured IP address, or 0 if no address is configured
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dhcpConfiguredNetworkAddrGet(void)
{
  return simConfiguredSystemIPAddrGet();
}

/*********************************************************************
* @purpose  Determine if the DHCP client wants to request a specific
*           IPv4 address from the server. If the system is restarting
*           and there is a checkpointed IPv4 address, request that address.
*           Otherwise, if the client previously had a DHCP address,
*           request that one.
*
* @param    prevIpAddr - Address this client previously learned from a
*                        DHCP server.
*
* @returns  IPv4 address to request, or 0 if no preference
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 dhcpRequestedNetworkAddrGet(L7_uint32 prevIpAddr)
{
  /* If this is a warm restart, request the checkpointed address. */
  if (dhcpcWarmRestart && simIsSystemAddrCheckpointed())
  {
    return simGetSystemIPAddr();
  }

  /* No checkpointed address. Use the address this DHCP client previously
   * received from the DHCP server. */
  return prevIpAddr;
}

/**************************************************************************
* @purpose  Get the notification semaphore.
*
* @comments
*
* @end
*************************************************************************/
void dhcpNotificationSemGet (void)
{
  L7_RC_t rc;

  rc = osapiSemaTake (dhcpClientCB.notifyListSem, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }
}

/**************************************************************************
* @purpose  Give the notification semaphore.
*
* @comments
*
* @end
*************************************************************************/
void dhcpNotificationSemFree (void)
{
  L7_RC_t rc;

  rc = osapiSemaGive (dhcpClientCB.notifyListSem);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR(rc);
  }
}

/*********************************************************************
* @purpose  Notification Registration
*
* @param    component_ID          @b((input)) componant id
* @param    requestedOptionsMask  @b((input))  options of interest to this component
* @param    notifyFuncPtr         @b((input))  function
*           to callback
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dhcpNotificationRegister( L7_COMPONENT_IDS_t component_ID,
                                  L7_uint32 requestedOptionsMask,
                                  L7_RC_t (*notifyFuncPtr)(L7_bootp_dhcp_t *networkParamsPtr))
{
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];
  L7_RC_t rc;

  if ((rc = cnfgrApiComponentNameGet(component_ID, name)) != L7_SUCCESS)
  {
    osapiStrncpySafe(name, "Unknown", 8);
  }

  if (component_ID >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_CLIENT_COMPONENT_ID,
            "Invalid DHCP registrar ID %u.", component_ID);
    return(L7_FAILURE);
  }

  if ((L7_uint32)dhcpClientCB.dhcpNotifyList[component_ID].notifyFunction != L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_CLIENT_COMPONENT_ID,
            "DHCP registrar ID %u, %s already registered.", component_ID, name);

    return(L7_FAILURE);
  }

  dhcpNotificationSemGet();
  dhcpClientCB.dhcpNotifyList[component_ID].registrar_ID = component_ID;
  dhcpClientCB.dhcpNotifyList[component_ID].requestedOptionsMask = requestedOptionsMask;
  dhcpClientCB.dhcpNotifyList[component_ID].notifyFunction = notifyFuncPtr;
  dhcpNotificationSemFree();

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose Deregister the routine to be called when a DHCP request completes.
*
* @param   L7_COMPONENT_IDS_t componentId  @b((input)) component ID
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, invalid argument
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t dhcpNotificationDeregister(L7_COMPONENT_IDS_t componentId)
{
  if ( componentId >= L7_LAST_COMPONENT_ID )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_CLIENT_COMPONENT_ID,
            "DHCP registrar ID %u greater than Last Component ID.", componentId);
    return L7_FAILURE;
  }

  dhcpNotificationSemGet();
  dhcpClientCB.dhcpNotifyList[componentId].notifyFunction = L7_NULLPTR;
  dhcpClientCB.dhcpNotifyList[componentId].registrar_ID = L7_NULL;
  dhcpNotificationSemFree();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Call registered users with information
*
* @param    networkParamsPtr  @b((input))  network parameters
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void dhcpNotifyRegisteredUsers(L7_bootp_dhcp_t *networkParamsPtr)
{
  L7_uint32 i;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  dhcpNotificationSemGet();

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (dhcpClientCB.dhcpNotifyList[i].registrar_ID != L7_NULL)
    {
      /* Notify the registered component only if the requested options are present.*/
      if ((networkParamsPtr->offeredOptionsMask &
          dhcpClientCB.dhcpNotifyList[i].requestedOptionsMask) != 0)
      {
        (void)(*dhcpClientCB.dhcpNotifyList[i].notifyFunction)(networkParamsPtr);
      }
    }
  }

  DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Updated Network Parameters to Registered Users");
  dhcpNotificationSemFree();

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}

/*********************************************************************
* @purpose  Send a DHCP release message on the service port.
*
* @param   void
*
* @returns void
*
* @comments This API is not used anymore.  Retaining it just to 
*           resolve a few build breaks.
*           The release is taken care in simSetServPortConfigMode().
*
* @end
*
*********************************************************************/
void dhcpc_svcport_release(void)
{
  return;
}

/*********************************************************************
* @purpose  If the network port has leased a DHCP address, send a DHCP
*           release message on the network port.
*
* @param   void
*
* @returns void
*
* @comments This API is not used anymore.  Retaining it just to 
*           resolve a few build breaks.
*           The release is taken care in simSetSystemConfigMode().
*
* @end
*
*********************************************************************/
void dhcpc_netport_release(void)
{
  return;
}

/*********************************************************************
* @purpose  Post a message to the DHCP Client Task
*
* @param    msg   @b((input)) Pointer to the Q message to be sent
*
* @returns  L7_SUCCESS, if the Message Q Send is successful
* @returns  L7_FAILURE, if the Message Q Send failed
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientUIEventPost (dhcpClientQueueMsg_t *msg)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (msg == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Event message is NULL");
    return L7_FAILURE;
  }

  msg->msgId = DHCP_CLIENT_UI_EVENT_MSG;

  if (dhcpClientMessageSend (msg) != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "UI Event Post Failed for Message-%d",
                 msg->msgId);
    return L7_FAILURE;
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initiate the DHCP Discover process to acquire an
*           IP Address on the interface
*
* @param    intIfNum     @b((input)) Internal Interface Number
* @param    mgmtPortType @b((input)) Mgmt Interface Type
*
* @returns  L7_SUCCESS, if the Discover Initiate processing is
*                       successful
* @returns  L7_FAILURE, if the Discover Initiate processing is
*                       unsuccessful
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientDiscoverInitiate (L7_uint32 intIfNum,
                            L7_MGMT_PORT_TYPE_t mgmtPortType)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, mgmtPortType))
                     == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "clientIntfInfo Get Failed");
    return L7_FAILURE;
  }

  if (dhcpClientMgmtPortTypeIsOperational (intIfNum, mgmtPortType) != L7_TRUE)
  {
    L7_LOGF (L7_LOG_SEVERITY_INFO, L7_DHCP_CLIENT_COMPONENT_ID,
             "%s Link State is Down.  Connect the port and try again.",
             dhcpDebugMgmtPortType[mgmtPortType]);
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "%s is not Operational, intIfNum - %d",
                 dhcpDebugMgmtPortType[mgmtPortType], intIfNum);
    dhcpClientIntfInfoDeInit (clientIntfInfo);
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse == L7_TRUE)
  {
    if (clientIntfInfo->dhcpState == DHCPSTATE_BOUND)
    {
      /* Address is already acquired. Requestors might have missed the earlier
       * updates.
       */
      if (dhcpClientIPAddressUpdate (clientIntfInfo) != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Address Updation Failed");
      }
      DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Client is already in Bound State, "
                   "No changes required");
      return L7_SUCCESS;
    }
    else
    {
      if ((clientIntfInfo->dhcpState <= DHCPSTATE_SELECTING) &&
          (clientIntfInfo->nextTimeout != L7_NULLPTR))
      {
        /* If the Timeout timer is running, just ignore this notification ...
         * It will take care of getting the address.
         */
        DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Timeout Timer is already running ... "
                     "No need to send any further Discovers.");
        return L7_SUCCESS;
      }
    }
  }

  if (clientIntfInfo->sockFD == 0)
  {
    if (dhcpClientIntfInfoInit (intIfNum, clientIntfInfo, mgmtPortType)
                             != L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Init Failed for intIfNum-%d, "
                   "mgmtPortType-%d", intIfNum, mgmtPortType);
      dhcpClientIntfInfoDeInit (clientIntfInfo);
      return L7_FAILURE;
    }
  }

  clientIntfInfo->dhcpState = DHCPSTATE_INIT;
  DHCPC_TRACE (DHCPC_DEBUG_STATE, "Client State set to INIT on intIfNum-%d for "
               "mgmtPortType-%d ... Invoking DHCP State machine", intIfNum, mgmtPortType);
  osapiSleep(1);
  doDhcp (clientIntfInfo);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initiate the DHCP Renewal process to renew the acquired
*           IP Address on the interface
*
* @param    intIfNum     @b((input)) Internal Interface Number
* @param    mgmtPortType @b((input)) Mgmt Interface Type
*
* @returns  L7_SUCCESS, if the Renew Initiate processing is successful
* @returns  L7_FAILURE, if the Renew Initiate processing is un-successful
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientRenewInitiate (L7_uint32 intIfNum,
                         L7_MGMT_PORT_TYPE_t mgmtPortType)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, mgmtPortType))
                     == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d, "
                 "mgmtPortType-%d", intIfNum, mgmtPortType);
    return L7_SUCCESS;
  }

  if (dhcpClientMgmtPortTypeIsOperational (intIfNum, mgmtPortType) != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "%s is not Operational, intIfNum - %d",
                 dhcpDebugMgmtPortType[mgmtPortType], intIfNum);
    dhcpClientIntfInfoDeInit (clientIntfInfo);
    return L7_FAILURE;
  }

  if ((clientIntfInfo->inUse != L7_TRUE) ||
      (clientIntfInfo->dhcpState <= DHCPSTATE_SELECTING))
  {
    DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Interface doesn't have leased IP address");
    return dhcpClientDiscoverInitiate (intIfNum, mgmtPortType);
  }
  else if (clientIntfInfo->sockFD == 0)
  {
    DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Interface doesn't have leased IP address");
    if (dhcpClientSocketCreate (intIfNum, clientIntfInfo, mgmtPortType)
                             != L7_SUCCESS)
    {
      dhcpClientSocketClose (clientIntfInfo);
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket Creation Failed for intIfNum-%d, "
                   "mgmtPortType-%d", intIfNum, mgmtPortType);
      return L7_FAILURE;
    }
  }
  else
  {
    /* Do Nothing */
  }

  DHCPC_TRACE (DHCPC_DEBUG_STATE, "Client State set to RENEW on intIfNum-%d for "
               "mgmtPortType-%d ... Invoking DHCP State machine", intIfNum, mgmtPortType);

  clientIntfInfo->dhcpState = DHCPSTATE_RENEWING;
  doDhcp (clientIntfInfo);

  if (clientIntfInfo->dhcpState == DHCPSTATE_BOUND)
  {
    dhcpClientSocketClose (clientIntfInfo);
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initiate the DHCP Release process to release the acquired
*           IP Address on the interface
*
* @param    intIfNum     @b((input)) Internal Interface Number
* @param    mgmtPortType @b((input)) Mgmt Interface Type
*
* @returns  L7_SUCCESS, if the Release Initiate processing is successful
* @returns  L7_FAILURE, if the Release Initiate processing is un-successful
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientReleaseInitiate (L7_uint32 intIfNum,
                           L7_MGMT_PORT_TYPE_t mgmtPortType)

{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_DHCP_CLIENT_STATE_t dhcpState;
  L7_bootp_dhcp_t dhcpParams;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  DHCPC_TRACE (DHCPC_DEBUG_EVENTS, "Initiating Release process for intIfNum-%d, "
               "mgmtPortType-%d", intIfNum, mgmtPortType);

  if ((clientIntfInfo = dhcpClientIntfInfoGet (intIfNum, mgmtPortType))
                     == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Info Get Failed for intIfNum-%d, "
                 "mgmtPortType-%d", intIfNum, mgmtPortType);

#ifdef L7_ROUTING_PACKAGE
    /* It could be possible that when the IP interface has an address and the
     * routing mode is disabled.  Now when DHCP is disabled on the interface,
     * IPMAP posts the Release event and DHCP will not be able to service this.
     * So, in such scenario's, Get the IP details from IPMAP and remove the
     * configured address.
     */
    if (mgmtPortType == L7_MGMT_IPPORT)
    {
      L7_IP_ADDR_t ipAddr = 0;
      L7_IP_MASK_t ipMask = 0;
      L7_INTF_IP_ADDR_METHOD_t method;
      if (ipMapRtrIntfCfgIpAddressMethodGet (intIfNum, &method) == L7_SUCCESS)
      {
        if (ipMapRtrIntfIpAddressGet (intIfNum, &ipAddr, &ipMask) == L7_SUCCESS)
        {
          DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Removing Router Interface Parameters ... ");
          if (ipMapRtrIntfIpAddressRemove (intIfNum, ipAddr, ipMask) != L7_SUCCESS)
          {
            DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Removing Router Interface Parameters in IPMAP Failed ");
          }
          DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Removed Router Interface Parameters ... ");
          dhcpClientCleanupIntfInfoByAddress (ipAddr);
          return L7_SUCCESS;
        }
      }
    }
#endif /* L7_ROUTING_PACKAGE */
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Interface is not Active");
    return L7_FAILURE;
  }

  if (dhcpClientMgmtPortTypeIsOperational (intIfNum, mgmtPortType) != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "%s is not Operational, intIfNum - %d",
                 dhcpDebugMgmtPortType[mgmtPortType], intIfNum);
    dhcpClientIntfInfoDeInit (clientIntfInfo);
    return L7_FAILURE;
  }

  dhcpState = clientIntfInfo->dhcpState;
  if ((clientIntfInfo->dhcpState < DHCPSTATE_BOUND) ||
      (dhcpState == DHCPSTATE_BOOTP_FALLBACK) ||
      (dhcpState == DHCPSTATE_NOTBOUND) ||
      (dhcpState == DHCPSTATE_FAILED) ||
      (dhcpState == DHCPSTATE_INFORM_FAILED))
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Incompatible DHCP State - %d to do a Release for "
                 "intIfNum - %d, mgmtPortType - %d", dhcpState, intIfNum, mgmtPortType);
    dhcpClientIntfInfoDeInit (clientIntfInfo);
    return L7_FAILURE;
  }

  if (getParamsFromDhcpStruct ((struct bootp*) &clientIntfInfo->txBuf,
                               &dhcpParams)
                            != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP Params Get Failed ..."
                 "intIfNum - %d, mgmtPortType - %d", intIfNum, mgmtPortType);
    return L7_FAILURE;
  }

  if (dhcpParams.ip == 0)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "IP Address doesn't exist for intIfNum - %d, "
                 "mgmtPortType - %d", intIfNum, mgmtPortType);
    return L7_FAILURE;
  }

  if (mgmtPortType == L7_MGMT_IPPORT)
  {
#ifdef L7_ROUTING_PACKAGE
    L7_IP_ADDR_t ipAddr = 0;
    L7_IP_MASK_t ipMask = 0;
    L7_INTF_IP_ADDR_METHOD_t method;
    ipMapRtrIntfCfgIpAddressMethodGet (intIfNum, &method);
    if (method == L7_INTF_IP_ADDR_METHOD_DHCP)
    {
      if (ipMapRtrIntfIpAddressGet (intIfNum, &ipAddr, &ipMask) == L7_SUCCESS)
      {
        if ((ipAddr == 0) && (ipMask == 0))
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "IP Port intIfNum - %d doesn't have "
                       "an IP Address", intIfNum);
          return L7_SUCCESS;
        }
      }
    }
#endif /* L7_ROUTING_PACKAGE */
  }

  if (clientIntfInfo->sockFD == 0)
  {
    if (dhcpClientSocketCreate (intIfNum, clientIntfInfo, mgmtPortType)
                             != L7_SUCCESS)
    {
      dhcpClientSocketClose (clientIntfInfo);
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket Creation Failed for intIfNum-%d, "
                   "mgmtPortType-%d", intIfNum, mgmtPortType);
      return L7_FAILURE;
    }
  }

  clientIntfInfo->dhcpState = DHCPSTATE_DO_RELEASE;
  DHCPC_TRACE (DHCPC_DEBUG_STATE, "Client State set to RELEASE on intIfNum-%d for "
               "mgmtPortType-%d ... Invoking DHCP State machine", intIfNum, mgmtPortType);

  doDhcp (clientIntfInfo);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initiate the DHCP Release process to release the acquired
*           IP Address on the interface
*
* @param    clientIntfInfo @b((input)) Pointer to the Client Interface
*                                      Information
*
* @returns  L7_SUCCESS, if the Release Initiate processing is successful
* @returns  L7_FAILURE, if the Release Initiate processing is un-successful
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientImmediateReleaseInitiate (dhcpClientInfo_t *clientIntfInfo)
{
  L7_DHCP_CLIENT_STATE_t dhcpState;
  L7_bootp_dhcp_t dhcpParams;
  L7_MGMT_PORT_TYPE_t mgmtPortType = clientIntfInfo->mgmtPortType;
  L7_uint32 intIfNum = clientIntfInfo->intIfNum;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.cbMembersProtectionSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Release Sem");
    return L7_FAILURE;
  }
  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Interface is not Active");
    osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);
    return L7_FAILURE;
  }

  if (dhcpClientMgmtPortTypeIsOperational (intIfNum, mgmtPortType) != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "%s is not Operational, intIfNum - %d",
                 dhcpDebugMgmtPortType[mgmtPortType], intIfNum);
    dhcpClientIntfInfoDeInit (clientIntfInfo);
    osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);
    return L7_FAILURE;
  }

  dhcpState = clientIntfInfo->dhcpState;
  if ((clientIntfInfo->dhcpState < DHCPSTATE_BOUND) ||
      (dhcpState == DHCPSTATE_BOOTP_FALLBACK) ||
      (dhcpState == DHCPSTATE_NOTBOUND) ||
      (dhcpState == DHCPSTATE_FAILED) ||
      (dhcpState == DHCPSTATE_INFORM_FAILED))
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Incompatible DHCP State - %d to do a Release for "
                 "intIfNum - %d, mgmtPortType - %d", dhcpState, intIfNum, mgmtPortType);
    dhcpClientIntfInfoDeInit (clientIntfInfo);
    osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);
    return L7_FAILURE;
  }

  if (getParamsFromDhcpStruct ((struct bootp*) &clientIntfInfo->txBuf,
                               &dhcpParams)
                            != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "DHCP Params Get Failed ..."
                 "intIfNum - %d, mgmtPortType - %d", intIfNum, mgmtPortType);
    osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);
    return L7_FAILURE;
  }

  if (dhcpParams.ip == 0)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "IP Address doesn't exist for intIfNum - %d, "
                 "mgmtPortType - %d", intIfNum, mgmtPortType);
    osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);
    return L7_FAILURE;
  }

  if (clientIntfInfo->sockFD == 0)
  {
    if (dhcpClientSocketCreate (intIfNum, clientIntfInfo, mgmtPortType)
                             != L7_SUCCESS)
    {
      dhcpClientSocketClose (clientIntfInfo);
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket Creation Failed for intIfNum-%d, "
                   "mgmtPortType-%d", intIfNum, mgmtPortType);
      osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);
      return L7_FAILURE;
    }
  }

  clientIntfInfo->dhcpState = DHCPSTATE_DO_RELEASE;
  DHCPC_TRACE (DHCPC_DEBUG_STATE, "Client State set to RELEASE on intIfNum-%d for "
               "mgmtPortType-%d ... Invoking DHCP State machine", intIfNum, mgmtPortType);

  doDhcp (clientIntfInfo);

  osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);

  DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "DHCP Immediate Release processing successful "
               "for intIfNum - %d", intIfNum);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process the Release event to release the acquired
*           IP Address on the interface
*
* @param    intIfNum     @b((input)) Internal Interface Number
* @param    mgmtPortType @b((input)) Mgmt Interface Type
*
* @returns  L7_SUCCESS, if the Release Initiate processing is successful
* @returns  L7_FAILURE, if the Release Initiate processing is un-successful
*
* @notes    This API is invoked synchronously from different task
*           contexts.  So, introduced protection here.
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientReleaseProcess (L7_uint32 intIfNum,
                          L7_MGMT_PORT_TYPE_t mgmtPortType)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.cbMembersProtectionSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Release Sem");
    return L7_FAILURE;
  }

  if (dhcpClientReleaseInitiate (intIfNum, mgmtPortType) != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "dhcpClientReleaseInitiate Processing Failed");
  }

  osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DHCP Client Interface Information Pointer
*
* @param    intIfNum         @b((input)) Internal Interface Number
* @param    mgmtPortType     @b((input)) Mgmt Interface Type
*
* @returns  dhcpClientInfo_t, pointer to DHCP Client Information for
*                             requested Managemet interface, if found
* @returns  L7_NULLPTR,       L7_NULLPTR, if  Client Information for
*                             requested Managemet interface is not found
*
* @notes    none
*
* @end
*********************************************************************/
dhcpClientInfo_t*
dhcpClientIntfInfoGet (L7_uint32 intIfNum,
                       L7_MGMT_PORT_TYPE_t mgmtPortType)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;

  /* DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry"); */

  if (mgmtPortType == L7_MGMT_SERVICEPORT)
  {
    clientIntfInfo = dhcpClientCB.clientIntfInfo[DHCP_CLIENT_SERVICE_PORT_INTF_NUM-1];
  }
  else if (mgmtPortType == L7_MGMT_NETWORKPORT)
  {
    clientIntfInfo = dhcpClientCB.clientIntfInfo[DHCP_CLIENT_NETWORK_PORT_INTF_NUM-1];
  }
#ifdef L7_ROUTING_PACKAGE
  else if (mgmtPortType == L7_MGMT_IPPORT)
  {
    L7_uint32 rtrIfNum = 0;
    if (ipMapIntIfNumToRtrIntf (intIfNum, &rtrIfNum) != L7_SUCCESS)
    {
      return L7_NULLPTR;
    }
    clientIntfInfo = dhcpClientCB.clientIntfInfo[rtrIfNum - 1];
  }
#endif /* L7_ROUTING_PACKAGE */
  else
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Invalid  mgmtPortType-%d", mgmtPortType);
    return L7_NULLPTR;
  }

  /* DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit"); */
  return clientIntfInfo;
}

/*********************************************************************
* @purpose  Get the DHCP Client Interface Information Pointer
*
* @param    rtrIfNum         @b((input)) Router Interface Number
* @param    mgmtPortType     @b((input)) Mgmt Interface Type
*
* @returns  dhcpClientInfo_t, pointer to DHCP Client Information for
*                             requested Managemet interface, if found
* @returns  L7_NULLPTR,       L7_NULLPTR, if  Client Information for
*                             requested Managemet interface is not found
*
* @notes    none
*
* @end
*********************************************************************/
dhcpClientInfo_t*
dhcpClientIntfInfoByRtrIfNumGet (L7_uint32 rtrIfNum,
                                 L7_MGMT_PORT_TYPE_t mgmtPortType)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;

  /* DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry"); */

  if (mgmtPortType != L7_MGMT_IPPORT)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Invalid  mgmtPortType-%d", mgmtPortType);
    return L7_NULLPTR;
  }

  /* TODO: Need to validate rtrIfNum */
  clientIntfInfo = dhcpClientCB.clientIntfInfo[rtrIfNum - 1];

  /* DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit"); */
  return clientIntfInfo;
}

/*********************************************************************
* @purpose  Create and Initialize the DHCP Client Socket for the
*           specified Mgmt Interface
*
* @param    intIfNum       @b((input)) Internal Interface Number
* @param    clientIntfInfo @b((input)) Pointer to the Client Interface
*                                      Information
* @param    mgmtPortType   @b((input)) Mgmt Interface Type
*
* @returns  L7_SUCCESS, if Socket Create/Init is successful
* @returns  L7_FAILURE, if Socket Create/Init failed
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientSocketCreate (L7_uint32 intIfNum,
                        dhcpClientInfo_t *clientIntfInfo,
                        L7_MGMT_PORT_TYPE_t mgmtPortType)
{
  L7_int32 one = 1;
#ifdef _L7_OS_VXWORKS_
  L7_int32 bootpcPort = IPPORT_BOOTPC;
#endif /* _L7_OS_VXWORKS_ */
  L7_char8 *intfName = L7_NULLPTR;
  L7_uchar8 ttlValue = DHCP_IP_TTL_NUMBER;
  L7_sockaddr_in_t clientInitialAddr;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSocketCreate (L7_AF_INET, L7_SOCK_DGRAM, 0, &clientIntfInfo->sockFD)
                      != L7_SUCCESS)
  {
    L7_LOGF (L7_LOG_SEVERITY_NOTICE, L7_DHCP_CLIENT_COMPONENT_ID,
             "Failed to create socket with domain L7_AF_INET, L7_SOCK_DGRAM");
    return L7_FAILURE;
  }

  memset(clientIntfInfo->intfName, 0, IFNAMSIZ);

  if (mgmtPortType == L7_MGMT_SERVICEPORT)
  {
    if ((intfName = bspapiServicePortNameGet()) == L7_NULLPTR)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Service Port Name Get Failed");
      return L7_FAILURE;
    }
    osapiSnprintf(clientIntfInfo->intfName, sizeof(clientIntfInfo->intfName),
                  "%s%d", intfName, bspapiServicePortUnitGet());
  }
  else if (mgmtPortType == L7_MGMT_NETWORKPORT)
  {
    intfName = L7_DTL_PORT_IF;
    osapiSnprintf (clientIntfInfo->intfName, sizeof(L7_DTL_PORT_IF)+1, "%s0",
                   L7_DTL_PORT_IF);
  }
#ifdef L7_ROUTING_PACKAGE
  else if (mgmtPortType == L7_MGMT_IPPORT)
  {
    L7_uchar8 ifName[IFNAMSIZ];

    if (osapiIfNameStringGet (intIfNum, ifName, IFNAMSIZ) != L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "intIfName IfName Get Failed");
      return L7_FAILURE;
    }
    osapiSnprintf (clientIntfInfo->intfName, sizeof(clientIntfInfo->intfName),
                   "%s", ifName);
  }
#endif /* L7_ROUTING_PACKAGE */
  else
  {
    return L7_FAILURE;
  }

  DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "intIfName - %s", clientIntfInfo->intfName);
  if (osapiSetsockopt (clientIntfInfo->sockFD, L7_SOL_SOCKET, L7_SO_REUSEADDR,
                       (L7_char8*) &one, sizeof(one))
                    != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket option L7_SO_REUSEADDR Set Failed");
    return L7_FAILURE;
  }

#ifdef _L7_OS_VXWORKS_
  if (osapiSetsockopt (clientIntfInfo->sockFD, L7_SOL_SOCKET, L7_SO_REUSEPORT,
                       (L7_char8*) &bootpcPort, sizeof(bootpcPort))
                    != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket option L7_SO_REUSEPORT Set Failed");
    return L7_FAILURE;
  }

  if (osapiSetsockopt (clientIntfInfo->sockFD, L7_SOL_SOCKET,
                       L7_SO_BINDTODEVICE, clientIntfInfo->intfName, IFNAMSIZ)
                    != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket option L7_SO_BINDTODEVICE Set "
                 "for Interface - %s Failed", clientIntfInfo->intfName);
    return L7_FAILURE;
  }
#endif /* _L7_OS_VXWORKS_ */

  if (osapiSetsockopt (clientIntfInfo->sockFD, L7_SOL_SOCKET, L7_SO_BROADCAST,
                       (L7_char8*) &one, sizeof(one))
                    != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket option L7_SO_BROADCAST Set Failed");
    return L7_FAILURE;
  }

  /* Set the mcast TTL = 255 as Cisco DHCP client does  */
  if (osapiSetsockopt (clientIntfInfo->sockFD, IPPROTO_IP, L7_IP_MULTICAST_TTL,
                       (L7_char8 *) &ttlValue, sizeof(ttlValue))
                    != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket option L7_IP_MULTICAST_TTL Set Failed");
    return L7_FAILURE;
  }

  memset ((L7_char8*) &clientInitialAddr, 0, sizeof(clientInitialAddr));
  clientInitialAddr.sin_family = L7_AF_INET;
  clientInitialAddr.sin_addr.s_addr = osapiHtonl(INADDR_ANY);
  clientInitialAddr.sin_port = osapiHtons(IPPORT_BOOTPC);

  if (osapiSocketBind (clientIntfInfo->sockFD, (L7_sockaddr_t*) &clientInitialAddr,
                       sizeof(clientInitialAddr))
                    != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket Bind Failed");
    return L7_FAILURE;
  }

  DHCPC_TRACE (DHCPC_DEBUG_INIT_DEINIT, "Socket Creation Successful for "
               "Interface - %s", clientIntfInfo->intfName);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Close the DHCP Client Socket
*
* @param    clientIntfInfo @b((input)) Pointer to the Client Interface
*                                      Information
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void
dhcpClientSocketClose (dhcpClientInfo_t *clientIntfInfo)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (clientIntfInfo != L7_NULLPTR)
  {
    if (clientIntfInfo->sockFD != 0)
    {
      DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Closing Socket - %d for intfName-%s",
                   clientIntfInfo->sockFD, clientIntfInfo->intfName);
      osapiSocketClose (clientIntfInfo->sockFD);
      clientIntfInfo->sockFD = 0;
    }
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}

/*********************************************************************
* @purpose  Initialize the parameters for the specified Mgmt Interface
*
* @param    intIfNum       @b((input)) Internal Interface Number
* @param    clientIntfInfo @b((input)) Pointer to the Client Interface
*                                      Information
* @param    mgmtPortType   @b((input)) Mgmt Interface Type
*
* @returns  L7_SUCCESS, if Initialization is successful
* @returns  L7_FAILURE, if Initialization is un-successful
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
dhcpClientIntfInfoInit (L7_uint32 intIfNum,
                        dhcpClientInfo_t *clientIntfInfo,
                        L7_MGMT_PORT_TYPE_t mgmtPortType)
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  static L7_uint32 seed = 0;
  extern void srandom (unsigned int seed);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  memset (clientIntfInfo, 0, sizeof(dhcpClientInfo_t));

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  DHCPC_TRACE (DHCPC_DEBUG_INIT_DEINIT, "Initializing Interface Info for intIfNum-%d, "
               "mgmtPortType-%d", intIfNum, mgmtPortType);

  /* Initialize Socket */
  DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket Creation Initiated for intIfNum-%d, "
               "mgmtPortType-%d", intIfNum, mgmtPortType);
  if (dhcpClientSocketCreate (intIfNum, clientIntfInfo, mgmtPortType)
                           != L7_SUCCESS)
  {
    dhcpClientSocketClose (clientIntfInfo);
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Socket Creation Failed for intIfNum-%d, "
                 "mgmtPortType-%d", intIfNum, mgmtPortType);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  /* Initialize Interface MAC Address and other details */
  if (mgmtPortType == L7_MGMT_SERVICEPORT)
  {
    simGetServicePortBurnedInMac (&macAddr[0]);
    clientIntfInfo->mgmtPortType = L7_MGMT_SERVICEPORT;
  }
  else if (mgmtPortType == L7_MGMT_NETWORKPORT)
  {
    simMacAddrGet (&macAddr[0]);
    clientIntfInfo->mgmtPortType = L7_MGMT_NETWORKPORT;
  }
#ifdef L7_ROUTING_PACKAGE
  else if (mgmtPortType == L7_MGMT_IPPORT)
  {
    nimGetIntfL3MacAddress (intIfNum, 0, macAddr);
    clientIntfInfo->mgmtPortType = L7_MGMT_IPPORT;
    clientIntfInfo->intIfNum = intIfNum;
    if (ipMapIntIfNumToRtrIntf (intIfNum, &clientIntfInfo->rtrIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
#endif /* L7_ROUTING_PACKAGE */
  else
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Invalid mgmtPortType-%d", mgmtPortType);
    osapiSemaGive (dhcpClientCB.clientIntfInfoSem);
    return L7_FAILURE;
  }

  memcpy (clientIntfInfo->macAddr, macAddr, sizeof(clientIntfInfo->macAddr));

  if (dhcpClientDebugFlagCheck (DHCPC_DEBUG_GENERAL) == L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "MAC Address - ");
    DHCPC_DEBUG_PRINTF ("%02X:%02X:%02X:%02X:%02X:%02X\n",
                        macAddr[0], macAddr[1], macAddr[2], macAddr[3],
                        macAddr[4], macAddr[5]);
  }

  /* Initialize Interface Transaction ID */
  seed = macAddr[L7_MAC_ADDR_LEN - 1];
  seed |= macAddr[L7_MAC_ADDR_LEN - 2] << 8;
  seed |= macAddr[L7_MAC_ADDR_LEN - 3] << 16;
  seed |= macAddr[L7_MAC_ADDR_LEN - 4] << 24;
  seed += osapiTimeMillisecondsGet();
#if defined(_L7_OS_VXWORKS_)
  srand(seed);
#else
  srandom(seed);
#endif
  clientIntfInfo->transID = seed + intIfNum;

  reset_timeout (clientIntfInfo);

  clientIntfInfo->inUse = L7_TRUE;

  DHCPC_TRACE (DHCPC_DEBUG_INIT_DEINIT, "Client Intf Info Initialization Success for "
               "intIfNum-%d and mgmtPortType-%d", intIfNum, mgmtPortType);

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  De-Initialize the parameters for the specified Mgmt Interface
*
* @param    clientIntfInfo @b((input)) Pointer to the Client Interface
*                                      Information
*
* @returns  L7_SUCCESS, if De-nitialization is successful
* @returns  L7_FAILURE, if De-Initialization is un-successful
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientIntfInfoDeInit (dhcpClientInfo_t *clientIntfInfo)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Interface is not in Use");
    return L7_FAILURE;
  }

  if (osapiSemaTake (dhcpClientCB.clientIntfInfoSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Client Intf Info Sem");
    return L7_FAILURE;
  }

  DHCPC_TRACE (DHCPC_DEBUG_INIT_DEINIT, "De-Initializing Interface Info for intIfNum-%d, "
               "mgmtPortType-%d", clientIntfInfo->intIfNum, clientIntfInfo->mgmtPortType);

  dhcpClientSocketClose (clientIntfInfo);

#ifdef _L7_OS_LINUX_
  if (osapimRouteEntryDelete (clientIntfInfo->intfName, 0xffffffff, 0x0,
                              0xffffffff, 0, 0, 1)
                           != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Broadcast Route Deletion in the IP Stack "
                 "Failed for Interface - %s", clientIntfInfo->intfName);
  }
#endif

  /* Stop timers, if any */
  if (clientIntfInfo->leaseInfo.alarm != L7_NULLPTR)
  {
    no_lease (&clientIntfInfo->leaseInfo);
  }
  if (clientIntfInfo->nextTimeout != L7_NULLPTR)
  {
    osapiTimerFree (clientIntfInfo->nextTimeout);
  }

  DHCPC_TRACE (DHCPC_DEBUG_INIT_DEINIT, "Client Intf Info De-Initialization Success for "
               "intIfNum-%d and mgmtPortType-%d", clientIntfInfo->intIfNum,
               clientIntfInfo->mgmtPortType);

  memset (clientIntfInfo, 0, sizeof(dhcpClientInfo_t));
  clientIntfInfo->inUse = L7_FALSE;

  osapiSemaGive (dhcpClientCB.clientIntfInfoSem);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Receive the DHCP Message from the Socket
*
* @param    readFds @b((input)) Read FDs that are Set
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void
dhcpClientMessageReceive (fd_set *readFds)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_sockaddr_in_t rxAddr;
  L7_uint32 addrLen = 0;
  L7_uint32 rtrIfNum = 0;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  /* Set FDs for Router Interfaces */
  for (rtrIfNum = 0; rtrIfNum < DHCP_CLIENT_MAX_RTR_INTERFACES; rtrIfNum++)
  {
    if ((clientIntfInfo = dhcpClientCB.clientIntfInfo[rtrIfNum]) != L7_NULLPTR)
    {
      if ((clientIntfInfo->inUse == L7_TRUE) && (clientIntfInfo->sockFD != 0))
      {
        if (FD_ISSET (clientIntfInfo->sockFD, readFds))
        {
          L7_uchar8 *sockBuffer = (L7_char8*) &clientIntfInfo->dhcpInfo.rxBuf;
          addrLen = sizeof (rxAddr);
          if (osapiSocketRecvfrom (clientIntfInfo->sockFD,
                         sockBuffer, sizeof(struct bootp), MSG_DONTWAIT,
                         (L7_sockaddr_t*) &clientIntfInfo->rxAddr,
                         &addrLen, &clientIntfInfo->bytesRcvd)
                      == L7_SUCCESS)
          {
            DHCPC_TRACE (DHCPC_DEBUG_TASK, "FD is Set and Message Received on "
                         "rtrIfNum-%d, Router Interface", rtrIfNum);
            doDhcp (clientIntfInfo);
          }
        }
      }
    }
  }

  /* Set FDs for Service Port */
  if ((clientIntfInfo = dhcpClientIntfInfoGet (rtrIfNum, L7_MGMT_SERVICEPORT))
                     != L7_NULLPTR)
  {
    if ((clientIntfInfo->inUse == L7_TRUE) && (clientIntfInfo->sockFD != 0))
    {
      if (FD_ISSET (clientIntfInfo->sockFD, readFds))
      {
          L7_uchar8 *sockBuffer = (L7_char8*) &clientIntfInfo->dhcpInfo.rxBuf;
          addrLen = sizeof (rxAddr);
          if (osapiSocketRecvfrom (clientIntfInfo->sockFD,
                         sockBuffer, sizeof(struct bootp), MSG_DONTWAIT,
                         (L7_sockaddr_t*) &clientIntfInfo->rxAddr,
                         &addrLen, &clientIntfInfo->bytesRcvd)
                      == L7_SUCCESS)
        {
          DHCPC_TRACE (DHCPC_DEBUG_TASK, "FD is Set and Message Received on "
                       "Service Port");
          doDhcp (clientIntfInfo);
        }
      }
    }
  }

  /* Set FDs for Network Port */
  if ((clientIntfInfo = dhcpClientIntfInfoGet (rtrIfNum, L7_MGMT_NETWORKPORT))
                     != L7_NULLPTR)
  {
    if ((clientIntfInfo->inUse == L7_TRUE) && (clientIntfInfo->sockFD != 0))
    {
      if (FD_ISSET (clientIntfInfo->sockFD, readFds))
      {
          L7_uchar8 *sockBuffer = (L7_char8*) &clientIntfInfo->dhcpInfo.rxBuf;
          addrLen = sizeof (rxAddr);
          if (osapiSocketRecvfrom (clientIntfInfo->sockFD,
                         sockBuffer, sizeof(struct bootp), MSG_DONTWAIT,
                         (L7_sockaddr_t*) &clientIntfInfo->rxAddr,
                         &addrLen, &clientIntfInfo->bytesRcvd)
                      == L7_SUCCESS)
        {
          DHCPC_TRACE (DHCPC_DEBUG_TASK, "FD is Set and Message Received on "
                       "Network Port");
          doDhcp (clientIntfInfo);
        }
      }
    }
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return;
}

/*********************************************************************
* @purpose  Update the leased DHCP parameters to the requested
*           Management Interface
*
* @param    clientIntfInfo @b((input)) Pointer to the Client Interface
*                                      Information
*
* @returns  L7_SUCCESS, if the updation is successful
* @returns  L7_FAILURE, if the updation is failed
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientIPAddressUpdate (dhcpClientInfo_t *clientIntfInfo)
{
  L7_bootp_dhcp_t dhcpParams;
  L7_char8 buf1[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8 buf2[OSAPI_INET_NTOA_BUF_SIZE];
  L7_char8 buf3[OSAPI_INET_NTOA_BUF_SIZE];

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (clientIntfInfo == L7_NULLPTR)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "clientIntfInfo is NULL");
    return L7_FAILURE;
  }

  if (clientIntfInfo->inUse != L7_TRUE)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Interface-%d is not in Use",
                 clientIntfInfo->intIfNum);
    return L7_FAILURE;
  }

  if (getParamsFromDhcpStruct ((struct bootp*) &clientIntfInfo->txBuf,
                                 &dhcpParams)
                              != L7_SUCCESS)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to get DHCP Parameters");
      return L7_FAILURE;
    }

  DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "IP Address - %s, Netmask - %s, Gateway - %s",
               dhcpClientDebugAddrPrint(dhcpParams.ip, buf1),
               dhcpClientDebugAddrPrint(dhcpParams.netMask, buf2),
               dhcpClientDebugAddrPrint(dhcpParams.gateway, buf3));

  if (clientIntfInfo->dhcpState == DHCPSTATE_BOUND)
  {
    if (clientIntfInfo->mgmtPortType == L7_MGMT_SERVICEPORT)
    {
      if (simGetServPortConfigMode() == L7_SYSCONFIG_MODE_DHCP)
      {
        DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Updating Service Port Parameters ... ");
        simSetServPortAddrWithMask (dhcpParams.ip, dhcpParams.netMask);
        simSetServPortIPGateway (dhcpParams.gateway);
      }
    }
    else if (clientIntfInfo->mgmtPortType == L7_MGMT_NETWORKPORT)
    {
      if (simGetSystemConfigMode() == L7_SYSCONFIG_MODE_DHCP)
      {
        DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Updating Network Port Parameters ... ");
        if (simSetSystemAddrWithMask (dhcpParams.ip, dhcpParams.netMask) != L7_SUCCESS)
        {
          DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Updating Network Port Parameters Failed");
        }
        simSetSystemIPGateway (dhcpParams.gateway);
      }
    }
#ifdef L7_ROUTING_PACKAGE
    else if (clientIntfInfo->mgmtPortType == L7_MGMT_IPPORT)
    {
      L7_routeEntry_t routeEntry;
      L7_uint32 gwAddr = 0;

      DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Updating Router Interface Parameters ... ");
      if (ipMapRtrIntfIpAddressSet (clientIntfInfo->intIfNum, dhcpParams.ip,
                                    dhcpParams.netMask,
                                    L7_INTF_IP_ADDR_METHOD_DHCP)
                                 != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Updating Router Interface Parameters to IPMAP Failed ");
        return L7_FAILURE;
      }
      if ((rtoIsDhcpDefGatewayPresent (&gwAddr) != L7_TRUE) &&
          (dhcpParams.gateway != 0))
      {
        DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Updating Gateway Parameters ... ");
        memset (&routeEntry, 0, sizeof(L7_routeEntry_t));
        routeEntry.ipAddr = 0;
        routeEntry.subnetMask = 0;
        routeEntry.protocol = RTO_DEFAULT;
        routeEntry.pref = ipMapRouterPreferenceGet(ROUTE_PREF_DEFAULT_ROUTE_DHCP);
        routeEntry.metric = FD_RTR_ROUTE_DEFAULT_COST;
        routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr = dhcpParams.gateway;
        routeEntry.ecmpRoutes.numOfRoutes = 1;
        routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.intIfNum = clientIntfInfo->intIfNum;
        if (rtoRouteAdd (&routeEntry) != L7_SUCCESS)
        {
          DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Updating Default Gateway to RTO Failed ");
          return L7_FAILURE;
        }

        /* Update in the IP Stack */
        ipstkDefGwUpdate();
      }
    }
#endif /* L7_ROUTING_PACKAGE */
    else
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Invalid Management Port Type - %d",
                   clientIntfInfo->mgmtPortType);
      return L7_FAILURE;
    }
  }
  else if (/*(clientIntfInfo->dhcpState == DHCPSTATE_FAILED) ||*/
           (clientIntfInfo->dhcpState == DHCPSTATE_DO_RELEASE) ||
           (clientIntfInfo->dhcpState == DHCPSTATE_NOTBOUND))
  {
    DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Resetting Address Parameters ... ");

    if (clientIntfInfo->mgmtPortType == L7_MGMT_SERVICEPORT)
    {
      if ((simGetServPortConfigMode() == L7_SYSCONFIG_MODE_DHCP) &&
          (simGetServPortIPAddr() != 0))
      {
        simSetServPortAddrWithMask (0, 0);
        simSetServPortIPGateway (0);
      }
    }
    else if (clientIntfInfo->mgmtPortType == L7_MGMT_NETWORKPORT)
    {
      if (simGetSystemConfigMode() == L7_SYSCONFIG_MODE_DHCP)
      {
        simSetSystemAddrWithMask (0, 0);
        simSetSystemIPGateway (0);
      }
    }
#ifdef L7_ROUTING_PACKAGE
    else if (clientIntfInfo->mgmtPortType == L7_MGMT_IPPORT)
    {
      DHCPC_TRACE (DHCPC_DEBUG_UPDATES, "Removing Router Interface Parameters ... ");
      if (ipMapRtrIntfIpAddressRemove (clientIntfInfo->intIfNum, dhcpParams.ip,
                                       dhcpParams.netMask)
                                      != L7_SUCCESS)
      {
        DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Removing Router Interface Parameters in IPMAP Failed ");
        return L7_FAILURE;
      }
    }
#endif /* L7_ROUTING_PACKAGE */
    else
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Invalid Management Port Type - %d",
                   clientIntfInfo->mgmtPortType);
      return L7_FAILURE;
    }
  }
  else
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "This API is not supposed to be called in "
                 "State-%d", clientIntfInfo->dhcpState);
    return L7_FAILURE;
  }

  dhcpNotifyRegisteredUsers (&dhcpParams);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process the Configured Network parameters
*
* @param    mgmtPortType   @b((input)) Mgmt Interface Type
* @param    mgmtPortType   @b((input)) IP Address to be configured
* @param    mgmtPortType   @b((input)) Network Mask to be configured
* @param    mgmtPortType   @b((input)) Default Gateway to be configured
*
* @returns  L7_SUCCESS, if the Configuration process is successful
* @returns  L7_FAILURE, if the Configuration process failed
*
* @notes    This API is only applicable when the IP address
*           information are configured to the Network and Service ports
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientConfiguredIpAddressProcess (L7_MGMT_PORT_TYPE_t mgmtPortType,
                                      L7_uint32 ipAddr,
                                      L7_uint32 netMask,
                                      L7_uint32 ipGateway)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_RC_t retVal = L7_FAILURE;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if ((mgmtPortType != L7_MGMT_SERVICEPORT) &&
      (mgmtPortType != L7_MGMT_NETWORKPORT))
  {
    return L7_FAILURE;
  }

  if ((clientIntfInfo = dhcpClientIntfInfoGet (0, mgmtPortType)) == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Release if an IP address is already acquired */
  if (clientIntfInfo->inUse == L7_TRUE)
  {
    if ((clientIntfInfo->dhcpState != DHCPSTATE_NOTBOUND) ||
        (clientIntfInfo->dhcpState != DHCPSTATE_FAILED))
    {
      clientIntfInfo->dhcpState = DHCPSTATE_DO_RELEASE;
      doDhcp (clientIntfInfo);
    }
  }
  if (dhcpClientIntfInfoInit (0, clientIntfInfo, mgmtPortType)
                           != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Client Intf Init Failed for mgmtPortType-%d",
                 mgmtPortType);
    dhcpClientIntfInfoDeInit (clientIntfInfo);
    return L7_FAILURE;
  }

  bzero ((L7_char8*) &clientIntfInfo->dhcpInfo.rxBuf,
         sizeof(clientIntfInfo->dhcpInfo.rxBuf));

  clientIntfInfo->dhcpState = DHCPSTATE_INFORM_REQUEST;

  retVal = doDhcp (clientIntfInfo);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return retVal;
}

/*********************************************************************
* @purpose  Check the status of Service and Network ports and if set
*           initiate the address lease process
*
* @param    none
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void
dhcpClientServiceAndNetworkPortStatusCheck (void)
{
  dhcpClientInfo_t *clientIntfInfo = L7_NULLPTR;
  L7_uint32 configMode = L7_SYSCONFIG_MODE_NONE;

  DHCPC_TRACE (DHCPC_DEBUG_TASK, "Entry");

  if ((configMode = simGetServPortConfigMode()) == L7_SYSCONFIG_MODE_DHCP)
  {
    DHCPC_TRACE (DHCPC_DEBUG_TASK, "Service port Mode is set to DHCP");
    /* Check for Service port configuration */
    if ((clientIntfInfo = dhcpClientIntfInfoGet (0, L7_MGMT_SERVICEPORT))
                                              != L7_NULLPTR)
    {
      if ((clientIntfInfo->inUse != L7_TRUE) ||
          (clientIntfInfo->dhcpState <= DHCPSTATE_SELECTING))
      {
        DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Service port doesn't have leased IP address");
        dhcpClientDiscoverInitiate (0, L7_MGMT_SERVICEPORT);
      }
    }
  }

  if ((configMode = simGetSystemConfigMode()) == L7_SYSCONFIG_MODE_DHCP)
  {
    DHCPC_TRACE (DHCPC_DEBUG_TASK, "Network port Mode is set to DHCP");
    /* Check for Service port configuration */
    if ((clientIntfInfo = dhcpClientIntfInfoGet (0, L7_MGMT_NETWORKPORT))
                                              != L7_NULLPTR)
    {
      if ((clientIntfInfo->inUse != L7_TRUE) ||
          (clientIntfInfo->dhcpState <= DHCPSTATE_SELECTING))
      {
        DHCPC_TRACE (DHCPC_DEBUG_GENERAL, "Network port doesn't have leased IP address");
        dhcpClientDiscoverInitiate (0, L7_MGMT_NETWORKPORT);
      }
    }
  }

  DHCPC_TRACE (DHCPC_DEBUG_TASK, "Exit");
  return;
}

/*********************************************************************
* @purpose  Sets the DHCP Vendor Class Option string
*
* @param    optionString   @b((input)) DHCP Vendor Class Option string
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpVendorClassOptionStringUpdate (L7_uchar8 *optionString)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.cbMembersProtectionSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Release Sem");
    return L7_FAILURE;
  }

  memcpy (&dhcpClientCB.dhcpOptions.vendorClassOption.vendorClassString,
          optionString, DHCP_VENDOR_CLASS_STRING_MAX);

  osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the DHCP Vendor Class Option string
*
* @param    optionString   @b((input)) DHCP Vendor Class Option Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
dhcpVendorClassOptionAdminModeUpdate (L7_BOOL mode)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (osapiSemaTake (dhcpClientCB.cbMembersProtectionSem, L7_WAIT_FOREVER)
                  != L7_SUCCESS)
  {
    DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Failed to acquire Release Sem");
    return L7_FAILURE;
  }

  dhcpClientCB.dhcpOptions.vendorClassOption.optionMode = mode;

  osapiSemaGive (dhcpClientCB.cbMembersProtectionSem);

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check to see if the Management port type is Operational
*
* @param    intIfNum     @b((input)) Internal Interface Number
* @param    mgmtPortType @b((input)) Type of the Management Port
*
* @returns  L7_TRUE, if operational
* @returns  L7_FALSE, if not operational
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL
dhcpClientMgmtPortTypeIsOperational (L7_uint32 intIfNum,
                                     L7_MGMT_PORT_TYPE_t mgmtPortType)
{
  L7_BOOL isOperational = L7_FALSE;

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

  if (mgmtPortType == L7_MGMT_SERVICEPORT)
  {
    if ((simGetServPortAdminState() == L7_ENABLE) &&
        (simGetServPortLinkState() == L7_ENABLE))
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Service port Admin/Link State is Operational");
      isOperational = L7_TRUE;
    }
    else
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Service port Admin/Link State is Not Operational");
    }
  }
  else if (mgmtPortType == L7_MGMT_NETWORKPORT)
  {
    if (simMayEnableNetworkPort() == L7_TRUE)
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Network port Admin/Link State is Operational");
      isOperational = L7_TRUE;
    }
    else
    {
      DHCPC_TRACE (DHCPC_DEBUG_FAILURE, "Network port Admin/Link State is Not Operational");
    }
  }
  else if (mgmtPortType == L7_MGMT_IPPORT)
  {
    /* TODO ... Not sure what to put in here ... */
    isOperational = L7_TRUE;
  }
  else
  {
    /* Do Nothing */
  }

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return isOperational;
}

/*********************************************************************
* @purpose  Clean-up the Interface info identified by an IP Address
*
* @param    intIfNum     @b((input)) Internal Interface Number
* @param    mgmtPortType @b((input)) Type of the Management Port
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if not successful
*
* @notes    When rtrIfNum is not available, the clientIntfInfo cannot
*           be indexed.  Use this API when rtrIfNum is not available.
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientCleanupIntfInfoByAddress (L7_uint32 ipAddr)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

#ifdef L7_ROUTING_PACKAGE
  dhcpClientInfo_t* clientIntfInfo = L7_NULLPTR;
  L7_bootp_dhcp_t dhcpParams;
  L7_uint32 index = 0;

  for (index = 0; index < DHCP_CLIENT_MAX_RTR_INTERFACES; index++)
  {
    if ((clientIntfInfo = dhcpClientCB.clientIntfInfo[index]) != L7_NULLPTR)
    {
      if (clientIntfInfo->inUse == L7_TRUE)
      {
        if (getParamsFromDhcpStruct ((struct bootp*) &clientIntfInfo->txBuf,
                                     &dhcpParams)
                                  == L7_SUCCESS)
        {
          if  (ipAddr == dhcpParams.ip)
          {
            dhcpClientIntfInfoDeInit (clientIntfInfo);
            break;
          }
        }
      }
    }
  }
#endif

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clean-up all the Interface info
*
* @param    void
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if not successful
*
* @notes    Use this API upon Clear config
*
* @end
*********************************************************************/
L7_RC_t
dhcpClientAllIntfInfoCleanup (void)
{
  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Entry");

#ifdef L7_ROUTING_PACKAGE
  dhcpClientInfo_t* clientIntfInfo = L7_NULLPTR;
  L7_uint32 index = 0;

  for (index = 0; index < DHCP_CLIENT_MAX_RTR_INTERFACES; index++)
  {
    if ((clientIntfInfo = dhcpClientCB.clientIntfInfo[index]) != L7_NULLPTR)
    {
      if (clientIntfInfo->inUse == L7_TRUE)
      {
        dhcpClientIntfInfoDeInit (clientIntfInfo);
      }
    }
  }
#endif

  DHCPC_TRACE (DHCPC_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

