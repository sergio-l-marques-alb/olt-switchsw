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
#include "dtlapi.h"
#include "bspapi.h"
#include "dhcps_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_ip_base_api.h"

#ifdef _L7_OS_ECOS_
#include <sys/ioctl.h>
#endif

/*LVL7:eth0 stands for service port and eth1 stands for network port */
struct bootp l7_eth0_bootp_data;
struct bootp l7_eth1_bootp_data;
L7_char8   l7_eth0_up;
L7_char8   l7_eth1_up;

L7_short16   l7_eth0_dhcpstate = 0;
L7_short16   l7_eth1_dhcpstate = 0;

struct dhcp_lease l7_eth0_lease;
struct dhcp_lease l7_eth1_lease;

void* dhcp_needs_attention;
L7_int32 dhcpv4client_task_id;

L7_uint32 servPortConfigStatus = L7_SYSCONFIG_MODE_NOT_COMPLETE;
L7_uint32 networkConfigStatus = L7_SYSCONFIG_MODE_NOT_COMPLETE;

/* Only log changes to DHCP status. */
L7_BOOL servPortLogStatus    = L7_TRUE;
L7_BOOL networkPortLogStatus = L7_TRUE;

L7_short16 servPortDHCPState;
L7_short16 networkDHCPState;

struct in_addr previaddr;

dhcpOptions_t dhcpOptions;

/* DHCP Configuraration protection semaphore.
*/
void * dhcpConfigSem = 0;

/* DHCP Notification protection semaphore.
*/
void * dhcpNotificationSem = L7_NULLPTR;

/* DHCP notification callback functions list */
dhcpNotifyList_t *dhcpNotifyList = L7_NULLPTR;

/* Signals if there is predefined static IP configuration on service port */
static L7_BOOL defaultServiceIpConfigExists = L7_FALSE;

/* Signals if there is predefined static IP configuration on network port*/
static L7_BOOL defaultNetworkIpConfigExists = L7_FALSE;

/**************************************************************************
* @purpose  Returns default IP address on DHCP enabled port
*
* @param    L7_bootp_t   boot params structure
*
* @param    L7_uint32    current DHCP enabled port address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*************************************************************************/
void dhcpEnabledPortAddressGet(struct bootp* dhcpParams, L7_uint32 *portIpAddr)
{
  L7_uint32 servicePortConfigMode = L7_SYSCONFIG_MODE_NONE; /* service port mode -  BOOTP/DHCP or none */
  L7_uint32 networkPortConfigMode = L7_SYSCONFIG_MODE_NONE; /* network port mode -  BOOTP/DHCP or none */
  
  servicePortConfigMode = simGetServPortConfigMode();
  networkPortConfigMode = simGetSystemConfigMode();

  /* Try first if there does not exist factory default IP address */
  if ((servicePortConfigMode == L7_SYSCONFIG_MODE_DHCP) && (defaultServiceIpConfigExists == L7_TRUE) && (simGetServPortIPAddr()))
  {
    *portIpAddr = dhcpParams->bp_ciaddr.s_addr;
  }
  
  else if ((networkPortConfigMode == L7_SYSCONFIG_MODE_DHCP) && (defaultNetworkIpConfigExists == L7_TRUE) && (simGetSystemIPAddr()))
  {
    *portIpAddr = dhcpParams->bp_ciaddr.s_addr;
  }
  
  /* if no default IP config is present - use DHCP offered IP address */    
  else
  {
    *portIpAddr = dhcpParams->bp_yiaddr.s_addr;
  }
}

/**************************************************************************
* @purpose  Extract parameters from dhcp parameters structue.
*
* @param    bootp    DHCP structure
*
* @param    L7_bootp_dhcp_t  Network Param Structure
*
* @param    L7_BOOL  Type of host configuration : 
*                    L7_TRUE - factory default static IP address assigned
*                    L7_FALSE - DHCP assigned IP configuration
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments Reworked for @2975.
*
* @end
*************************************************************************/

L7_int32 getParamsFromDhcpStruct(struct bootp* dhcpParams, L7_bootp_dhcp_t *networkParams)
{
  L7_uint32 netMask    = L7_NULL;
  L7_uint32 siaddr     = L7_NULL;
  L7_uint32 portIpAddr = L7_NULL;  
  L7_uint32 tftpServerIpAddr = L7_NULL;
  L7_uint32 dnsServerIpAddr  = L7_NULL;
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
  if (l7_get_bootp_option( dhcpParams, TAG_SUBNET_MASK, &netMask ) != L7_TRUE)
    netMask = osapiHtonl(0xFFFFFF00); /* At least it's not uninitialized */
  if (l7_get_bootp_option( dhcpParams, TAG_GATEWAY, (void *)defaultGateway) != L7_TRUE)
    defaultGateway[0] = 0;
  if (l7_get_bootp_option( dhcpParams, TAG_DOMAIN_SERVER, &dnsServerIpAddr )!= L7_TRUE)
      dnsServerIpAddr = 0;
  if (l7_get_bootp_option( dhcpParams, TAG_TFTP_SERVER, &tftpServerIpAddr ) != L7_TRUE)
  {
    tftpServerIpAddr = L7_NULL;
  }  

  memcpy(&siaddr, &(dhcpParams->bp_siaddr), sizeof(dhcpParams->bp_siaddr));
	
  if (l7_get_bootp_option( dhcpParams, TAG_DHCP_OPTOVER, &optionOverloadVal ) == L7_TRUE)
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
  
  if (l7_get_bootp_option( dhcpParams, TAG_TFTP_SERVER_NAME, tftpServerName ) == L7_TRUE)
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
  
  if (l7_get_bootp_option( dhcpParams, TAG_BOOT_FILENAME, bootFileName ) == L7_TRUE)
  {
    osapiStrncpy(networkParams->optBootFile, bootFileName, strlen(bootFileName));
  }
  /* Finish look for boot file name */

  dhcpEnabledPortAddressGet(dhcpParams, &portIpAddr);
  	
  networkParams->ip = osapiNtohl(portIpAddr);
  networkParams->netMask = osapiNtohl(netMask);
  networkParams->gateway = osapiNtohl(defaultGateway[0]);
  networkParams->optDnsServerIpAddr  = osapiNtohl(dnsServerIpAddr);
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
    
  if (networkParams->optDnsServerIpAddr != L7_NULL)
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

L7_int32 dhcpDebugPortParamsPrint(struct bootp* dhcpParams, L7_uint8 portType)
{
  L7_bootp_dhcp_t networkParams;
  L7_uchar8 ipString[OSAPI_INET_NTOA_BUF_SIZE];

  memset(&networkParams, 0, sizeof(L7_bootp_dhcp_t));
  memset(ipString, 0, sizeof(ipString));

  getParamsFromDhcpStruct(dhcpParams, &networkParams);

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
  
  osapiInetNtoa(networkParams.optDnsServerIpAddr, ipString);
  sysapiPrintf("DHCP offered DNS server address %s \n", ipString);
  
  osapiInetNtoa(networkParams.siaddr, ipString);
  sysapiPrintf("DHCP offered TFTP server address %s \n", ipString);
  
  osapiInetNtoa(networkParams.optTftpSvrIpAddr, ipString);
  sysapiPrintf("DHCP offered TFTP option server address %s \n", ipString);
  
  sysapiPrintf("DHCP offered TFTP server name %s \n", networkParams.sname);
  sysapiPrintf("DHCP offered TFTP option server name %s \n", networkParams.optTftpSvrName);
  sysapiPrintf("DHCP offered boot file %s \n", networkParams.bootFile);
  sysapiPrintf("DHCP offered option boot file %s \n\n", networkParams.optBootFile);
  
  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Print dhcp parameters
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
*
* @end
*************************************************************************/

L7_int32 dhcpDebugParamsPrint(void)
{
  dhcpDebugPortParamsPrint(&l7_eth0_bootp_data, DHCP_SERVICE_PORT);
  dhcpDebugPortParamsPrint(&l7_eth1_bootp_data, DHCP_NETWORK_PORT);

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Calls DHCP state machine function for service port config
*     parameters.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/
L7_int32 dhcp_bind_service_port( void )
{
  L7_uint32       servPortConfigMode;

  dhcpConfigSemGet ();
  /* for the service port */
  usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &servPortConfigMode);
  dhcpConfigSemFree ();

  /* Run the dhcp state machine for service port if Config Mode and status are true ...*/
  if ( (servPortConfigMode == L7_SYSCONFIG_MODE_DHCP) &&
       (servPortConfigStatus != L7_SYSCONFIG_MODE_COMPLETE) )
  {
    if ((defaultServiceIpConfigExists == L7_TRUE) && (simGetServPortIPAddr()))
    {
      l7_eth0_up = dhcpInformRequest( bspapiServicePortNameGet(), &l7_eth0_bootp_data, &l7_eth0_dhcpstate, &servPortDHCPState);	
    }
    else
    {
      l7_eth0_up = do_dhcp(bspapiServicePortNameGet(), &l7_eth0_bootp_data,
                           &l7_eth0_dhcpstate, &l7_eth0_lease, &servPortDHCPState);
    }
  }

  if ( DHCPSTATE_FAILED == l7_eth0_dhcpstate )
  {
    if ( L7_TRUE == servPortLogStatus )
    {
      LOG_MSG("dhcp_bind(): servPort dhcpstate failed \n");
      servPortLogStatus = L7_FALSE;
    }
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Calls DHCP state machine function for network port config
*     parameters.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/

L7_int32 dhcp_bind_network( void )
{
  L7_uint32       networkConfigMode;

  /* for inband */
  dhcpConfigSemGet ();
  usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &networkConfigMode);
  dhcpConfigSemFree ();

  /* Run the state machine for network port...*/
  if ( (networkConfigMode == L7_SYSCONFIG_MODE_DHCP) &&
       (networkConfigStatus != L7_SYSCONFIG_MODE_COMPLETE) )
  {
    if ((defaultNetworkIpConfigExists == L7_TRUE) && (simGetSystemIPAddr()))
    {
      l7_eth1_up = dhcpInformRequest( L7_DTL_PORT_IF, &l7_eth1_bootp_data, &l7_eth1_dhcpstate, &networkDHCPState);	
    }
    else
    {
      l7_eth1_up = do_dhcp(L7_DTL_PORT_IF, &l7_eth1_bootp_data, &l7_eth1_dhcpstate,
                           &l7_eth1_lease, &networkDHCPState);
    }
  }

  if ( DHCPSTATE_FAILED == l7_eth1_dhcpstate )
  {
    if ( L7_TRUE == networkPortLogStatus )
    {
      LOG_MSG("dhcp_bind(): networkPort dhcpstate failed \n");
      networkPortLogStatus = L7_FALSE;
    }
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  DHCP task entry point for configuration of Service port and
*           network port parameters
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments None.
*
* @end
*************************************************************************/
L7_RC_t L7_dhcp(void)
{

  L7_short16 old_eth0_dhcpstate = l7_eth0_dhcpstate;
  L7_short16 old_eth1_dhcpstate = l7_eth1_dhcpstate;

  L7_bootp_dhcp_t params;
  L7_uint32       ibMode, ibMode_last = 0;             /* bootp/DHCP or none */
  L7_uint32       spMode, spMode_last = 0;             /* bootp/DHCP or none */
  L7_uint32       ibMode2, spMode2;
  L7_uint32       servPortIpAddr    = L7_NULL;  
  L7_uint32       networkPortIpAddr = L7_NULL;  
  l7_eth0_lease.dhcpState= &l7_eth0_dhcpstate;
  l7_eth1_lease.dhcpState= &l7_eth1_dhcpstate;
  l7_eth0_lease.configStatus= &servPortConfigStatus;
  l7_eth1_lease.configStatus= &networkConfigStatus;
  l7_eth0_lease.net_t1 = l7_eth1_lease.net_t1 = 0;
  l7_eth0_lease.net_t2 = l7_eth1_lease.net_t2 = 0;
  l7_eth0_lease.net_expiry = l7_eth1_lease.net_expiry = 0;

  servPortDHCPState=CONFIG_NONE;
  networkDHCPState= CONFIG_NONE;

  previaddr.s_addr = 0;
  dhcpConfigSemGet ();
	
  /* In case default IP address value isn't present or configured,
     the value of servPortIpAddr will be NULL */
  servPortIpAddr = simGetServPortIPAddr();
  if (servPortIpAddr != L7_NULL)
  {
    defaultServiceIpConfigExists = L7_TRUE;
  }

  networkPortIpAddr = simGetSystemIPAddr();		
  if (networkPortIpAddr != L7_NULL)
  {
    defaultNetworkIpConfigExists = L7_TRUE;
  }
  
  dhcpConfigSemFree ();    

  while ( L7_TRUE )
  {

	dhcpConfigSemGet ();

    usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &spMode);
    if (spMode != spMode_last)
    {
      servPortLogStatus = L7_TRUE;
    }
    spMode_last = spMode;

    usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &ibMode);
    if (ibMode != ibMode_last)
    {
      networkPortLogStatus = L7_TRUE;
    }
    ibMode_last = ibMode;

	dhcpConfigSemFree ();

    if ( spMode == L7_SYSCONFIG_MODE_DHCP )
    { 
      /* Service Port configured for DHCP */

      /* When DHCP client is enabled by administrator, it could be either to override 
         the existing static IP or to perform some parameter assigning function for the 
         same address binding. If the IP is set back to zero when DHCP is enabled, which
         means for new address assignment, the new address is taken into account. If not,
         i.e. DHCP is enabled for parameter assignment, then the INFORM message is sent 
         for this.This following flag 'defaultServiceIpConfigExists' help to acheive this. */
      servPortIpAddr = simGetServPortIPAddr();
      if (servPortIpAddr == L7_NULL)
      {
        defaultServiceIpConfigExists = L7_FALSE;
      }
      if ( l7_eth0_dhcpstate != DHCPSTATE_BOUND )
      { /* Not in a bound state */
        if ( dhcp_bind_service_port() == L7_SUCCESS )
        {
          if ( l7_eth0_up &&
               l7_eth0_dhcpstate == DHCPSTATE_BOUND &&
               old_eth0_dhcpstate != l7_eth0_dhcpstate &&
               servPortDHCPState== CONFIG_SET )
          {
		    L7_uchar8 ifname[20];

        sprintf(ifname, "%s0", bspapiServicePortNameGet());

            /* Set new IP parameters */
            if ( L7_SUCCESS == getParamsFromDhcpStruct(&l7_eth0_bootp_data, &params) )
            {
              dhcpConfigSemGet ();
              usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &spMode2);
              if (spMode2 == L7_SYSCONFIG_MODE_DHCP) 
              { /* Close race condition */
                if ((defaultServiceIpConfigExists != L7_TRUE) || (simGetServPortIPAddr() == 0))
                {
                  /*Set IP Addresss, Default Gateway, Netmask */
                  usmDbServicePortIPAddrSet(USMDB_UNIT_CURRENT, params.ip);
                  usmDbServicePortNetMaskSet(USMDB_UNIT_CURRENT, params.netMask);
                }
				
                usmDbServicePortGatewaySet(USMDB_UNIT_CURRENT, params.gateway);
    
                dhcpNotifyRegisteredUsers(&params);
              }
			  
			  dhcpConfigSemFree ();

#ifdef _L7_OS_LINUX_    
              /*delete route entry for 0.0.0.0 IP address */
              osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff,
			    0, 0, 1);
#endif
              servPortConfigStatus = L7_SYSCONFIG_MODE_COMPLETE;
              servPortLogStatus = L7_TRUE;
            }
          }
        }

        if ( l7_eth0_dhcpstate == DHCPSTATE_FAILED )
		{
			l7_eth0_dhcpstate= 0;
		}
      }
    }

    if ( ibMode == L7_SYSCONFIG_MODE_DHCP )
    { /* Network port is configured for DHCP */

      /* When DHCP client is enabled by administrator, it could be either to override 
         the existing static IP or to perform some parameter assigning function for the 
         same address binding. If the IP is set back to zero when DHCP is enabled, which
         means for new address assignment, the new address is taken into account. If not,
         i.e. DHCP is enabled for parameter assignment, then the INFORM message is sent 
         for this.This following flag 'defaultNetworkIpConfigExists' help to acheive this. */
      networkPortIpAddr = simGetSystemIPAddr();		
      if (networkPortIpAddr == L7_NULL)
      {
        defaultNetworkIpConfigExists = L7_FALSE;
      }
      if ( l7_eth1_dhcpstate != DHCPSTATE_BOUND )
      { /* Not in a bound state */
        if ( dhcp_bind_network() == L7_SUCCESS )
        {
          if ( l7_eth1_up &&
               l7_eth1_dhcpstate     == DHCPSTATE_BOUND &&
               old_eth1_dhcpstate != l7_eth1_dhcpstate &&
               networkDHCPState   == CONFIG_SET )
          {
		    L7_uchar8 ifname[20];

			sprintf(ifname, "%s0", L7_DTL_PORT_IF);

            /* Set new IP parameters */
            if ( L7_SUCCESS == getParamsFromDhcpStruct(&l7_eth1_bootp_data, &params) )
            {

			  dhcpConfigSemGet ();
              usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(USMDB_UNIT_CURRENT, &ibMode2);
              if (ibMode2 == L7_SYSCONFIG_MODE_DHCP)
              { /* Close race condition */
                if ((defaultNetworkIpConfigExists != L7_TRUE) || (simGetSystemIPAddr() == 0))
                {                
                  /*Set IP Addresss, Default Gateway, Netmask */
                  usmDbAgentIpMaskIfAddressSet(USMDB_UNIT_CURRENT,  params.ip, params.netMask);
                }
				
                usmDbAgentIpIfDefaultRouterSet(USMDB_UNIT_CURRENT, params.gateway);

                dhcpNotifyRegisteredUsers(&params);				
              }
			  dhcpConfigSemFree ();

#ifdef _L7_OS_LINUX_
              /*delete route entry for 0.0.0.0 IP address */
              osapimRouteEntryDelete (ifname, 0xffffffff, 0x0, 0xffffffff,
			    0, 0, 1);
#endif
              networkConfigStatus = L7_SYSCONFIG_MODE_COMPLETE;
              networkPortLogStatus = L7_TRUE;
            }
          }
        }

        if ( l7_eth1_dhcpstate == DHCPSTATE_FAILED )
		{
			l7_eth1_dhcpstate= 0;
		}
      }
    }

    /* Recheck after some time .....        */
    osapiSleep(10);
  }

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Create configuration protection semaphore.
*
* @comments The DHCP task is started by SIM in phase 2.
*			The configuration protection semaphore allows the task to 
*			read configuration only when the system is in phase 3.
*
* @end
*************************************************************************/
void dhcpConfigSemCreate (void)
{

   dhcpConfigSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
   if (dhcpConfigSem == L7_NULL) 
   {
	   LOG_ERROR (0);
   }
}

/**************************************************************************
* @purpose  Get the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void dhcpConfigSemGet (void)
{
  L7_RC_t rc;

   rc = osapiSemaTake (dhcpConfigSem, L7_WAIT_FOREVER);
   if (rc != L7_SUCCESS) 
   {
	   LOG_ERROR (rc);
   }
}

/**************************************************************************
* @purpose  Free the configuration semaphore.
*
* @comments 
*
* @end
*************************************************************************/
void dhcpConfigSemFree (void)
{
	L7_RC_t rc;

	rc = osapiSemaGive (dhcpConfigSem);
	if (rc != L7_SUCCESS) 
	{
		LOG_ERROR (rc);
	}
}

/**************************************************************************
* @purpose  Create notification protection semaphore.
*
* @comments The DHCP task is started by SIM in phase 2.
*			The notification protection semaphore allows the task to read
*           records in notify callback table synchronously with client tasks.
*
* @end
*************************************************************************/
void dhcpNotificationSemCreate (void)
{
  dhcpNotificationSem = osapiSemaBCreate (OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dhcpNotificationSem == L7_NULL) 
  {
    LOG_ERROR (0);
  }
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

  rc = osapiSemaTake (dhcpNotificationSem, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS) 
  {
    LOG_ERROR (rc);
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

  rc = osapiSemaGive (dhcpNotificationSem);
  if (rc != L7_SUCCESS) 
  {
    LOG_ERROR (rc);
  }
}

/**************************************************************************
* @purpose  DHCP task startup function.
*
* @comments None.
*
* @end
*************************************************************************/
void dhcpTaskStart()
{

  dhcpv4client_task_id = osapiTaskCreate( "DHCP Client Task", L7_dhcp, 0, 0,
                                     L7_DEFAULT_STACK_SIZE,
                                     L7_DEFAULT_TASK_PRIORITY,
                                     L7_DEFAULT_TASK_SLICE);
  if ( dhcpv4client_task_id == L7_ERROR )
    LOG_MSG("Failed to Create DHCP Client Task.\n");
}

/*********************************************************************
* @purpose  Notification Registration 
*
* @param    L7_uint32  component_ID   @b((input)) componant id
* @param    L7_uint32  (*notify)(L7_bootp_dhcp_t)   @b((input))  function
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
  if (component_ID >= L7_LAST_COMPONENT_ID)
  {
    LOG_MSG("DHCP registrar ID %u greater than Last Component ID\n", component_ID);
    return(L7_FAILURE);
  }

  if ((L7_uint32)dhcpNotifyList[component_ID].notifyFunction != L7_NULL)
  {
    LOG_MSG("DHCP registrar ID %u already registered\n", component_ID);
    return(L7_FAILURE);
  }
  
  dhcpNotificationSemGet();
  dhcpNotifyList[component_ID].registrar_ID = component_ID;
  dhcpNotifyList[component_ID].requestedOptionsMask = requestedOptionsMask;
  dhcpNotifyList[component_ID].notifyFunction = notifyFuncPtr;
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
    LOG_MSG("DHCP registrar ID %u greater than Last Component ID\n", componentId);
    return L7_FAILURE;
  }

  dhcpNotificationSemGet();
  dhcpNotifyList[componentId].notifyFunction = L7_NULLPTR;
  dhcpNotifyList[componentId].registrar_ID = L7_NULL;
  dhcpNotificationSemFree();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Call registered users with information 
*
* @param L7_bootp_dhcp_t networkParamsPtr @b((input)) pointer to
*        network parameters structure
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

  dhcpNotificationSemGet();

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    if (dhcpNotifyList[i].registrar_ID != L7_NULL)
    {
      /* Notify the registered component only if the requested options are present.*/
      if ((networkParamsPtr->offeredOptionsMask & dhcpNotifyList[i].requestedOptionsMask) > L7_NULL )
      {
        (void)(*dhcpNotifyList[i].notifyFunction)(networkParamsPtr);
      }
    }
  }
  
  dhcpNotificationSemFree();
  
  return;
}

/**************************************************************************
* @purpose  Release (and set state to DHCPSTATE_FAILED)for given interface
* @param    Char *intf   Interface Name String
*
* @comments None.
*
* @end
*************************************************************************/
void l7_dhcp_release( void )
{
  if ( l7_eth0_up )
  {
    do_dhcp_release(bspapiServicePortNameGet(), &l7_eth0_bootp_data, &l7_eth0_dhcpstate, &l7_eth0_lease);
  }
  if ( l7_eth1_up )
  {
    do_dhcp_release("dtl", &l7_eth1_bootp_data, &l7_eth1_dhcpstate, &l7_eth1_lease);
  }
  servPortConfigStatus = L7_SYSCONFIG_MODE_NOT_COMPLETE;
  networkConfigStatus = L7_SYSCONFIG_MODE_NOT_COMPLETE;

}
/* EOF dhcp_support.c*/
