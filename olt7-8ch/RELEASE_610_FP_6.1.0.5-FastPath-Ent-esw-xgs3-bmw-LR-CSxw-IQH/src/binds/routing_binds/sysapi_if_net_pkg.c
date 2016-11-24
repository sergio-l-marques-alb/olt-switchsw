/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  sysapi_if_net_pkg.c
*
* @purpose   Provide package-dependent system-wide support routines 
*            specific to network and mbuf routines
*
* @component sysapi
*
* @create    08/08/2001
*
* @author    paulq 
*
* @end
*
*********************************************************************/

/*************************************************************
                    
*************************************************************/

#include <errno.h>
#include <stdlib.h>
#include "string.h"                /* for memcpy() etc... */
#include <l7_common.h>
#include <registry.h>
#include "osapi.h"
#include "log.h"
#include "sysapi.h"
#include "simapi.h"
#include "dtlapi.h"
#include "l7_ip_api.h"
#include "usmdb_util_api.h"
/* @p1261 start */

/* @p1261 end */


extern int inet_addr(char *);


/**************************************************************************
*
* @purpose	Determine if system has an IP address configured on any interface
*
* @param    void
*
* @returns  L7_TRUE     if system has at least one IP address configured
* @returns  L7_FALSE    otherwise
*
* @notes    none
*
* @end
*
*************************************************************************/
L7_BOOL sysapiIPConfigValid(void)
{
  L7_uint32 myHostAddr;
  L7_uint32 intIfNum;
  L7_IP_ADDR_t ipAddr;
  L7_IP_MASK_t mask;
  L7_uint32 i;  
  L7_BOOL rc;

  rc = L7_FALSE;


  /* Is the service port IP address configured? */
  if ( (myHostAddr = simGetServPortIPAddr()) != 0 )
  {
    rc = L7_TRUE;
  }
  else
    if ( (myHostAddr = simGetSystemIPAddr()) != 0 )
  {
    /* Is the system IP address configured? */
    rc = L7_TRUE;
  }
  else
  {
    /* Is any router port IP address configured? */
    for ( i = 1; i < L7_RTR_MAX_RTR_INTERFACES; i++)
    {
      if (ipMapRtrIntfToIntIfNum(i, &intIfNum) == L7_SUCCESS)
      {
        if (ipMapRtrIntfIpAddressGet(intIfNum, &ipAddr, &mask) == L7_SUCCESS)
        {
          if (ipAddr != 0)
          {
            rc = L7_TRUE;
            break;
          }
        }
      }
    }
  }
  return rc;
}

/* @p1261 start */
/*********************************************************************
* @purpose  checks IP Address and NetMask for subnet conflicts on the system
*
* @param    intIfNum      L7_uint32 internal interface index of the port ifIpAddress and ifNetMask are associated with
* @param    ifIpAddress   L7_uint32 IP Address to check for conflict
* @param    ifNetMask     L7_uint32 NetMask for ifIpAddress
* @param    errorMsg      L7_uint32 pointer to hold localized error message number if L7_TRUE is returned.
*           
*
* @returns  L7_TRUE       if there is a conflict
* @returns  L7_FALSE      if there are no conflicts
*
* @notes    use FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM for Service port interface
* @notes    use FD_CNFGR_NIM_MIN_CPU_INTF_NUM for CPU interface
*       
* @end
*********************************************************************/
L7_BOOL sysapiIPConfigConflict(L7_uint32 intIfNum, L7_uint32 ifIpAddress, L7_uint32 ifNetMask, L7_uint32 *errorMsg)
{
  L7_IP_ADDR_t ipAddress;
  L7_IP_MASK_t netMask;
  L7_BOOL rc;

  rc = L7_FALSE;
  *errorMsg = 0;

  /* does the configuration conflict with the network port? */
  if (intIfNum < FD_CNFGR_NIM_MIN_CPU_INTF_NUM || intIfNum > FD_CNFGR_NIM_MAX_CPU_INTF_NUM)
  {
    if ((ipAddress = simGetSystemIPAddr()) != 0 &&
        (netMask = simGetSystemIPNetMask()) != 0)
    {
      if (usmDbSubnetCompare(ifIpAddress, ifNetMask, ipAddress, netMask) != L7_SUCCESS)
      {
        rc = L7_TRUE;
        *errorMsg = 7101; /* Conflict with the IP Address/Netmask of the switch. */
      }
    }
  }

  /* does the configuration conflict with the service port? */
  if ((rc != L7_TRUE) && (intIfNum < FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM || intIfNum > FD_CNFGR_NIM_MAX_SERV_PORT_INTF_NUM))
  {
    if ((ipAddress = simGetServPortIPAddr()) != 0 &&
        (netMask = simGetServPortIPNetMask()) != 0)
    {
      if (usmDbSubnetCompare(ifIpAddress, ifNetMask, ipAddress, netMask) != L7_SUCCESS)
      {
        rc = L7_TRUE;
        *errorMsg = 7102; /* Conflict with the IP Address/Netmask of the service port. */
      }
    }
  }

  /* Is any router port IP address configured and conflicting? */
  if ((rc != L7_TRUE) && (ipMapRtrIntfAddressConflict(intIfNum, ifIpAddress, ifNetMask)))
  {
    rc = L7_TRUE;
    *errorMsg = 7103; /* Conflict with the IP Address/Netmask of a router interface. */
  }

  return rc;
}
/* @p1261 end */

/*********************************************************************
* @purpose  Finds first valid IP address on this system
*
* @param    ipAddress    local address
*
* @notes    returns 0.0.0.0 if no valid IP addresses are found
*       
* @end
*********************************************************************/
void sysapiIPFirstLocalAddress(L7_uint32 *ipAddress)
{
  /* Service Port */
  if (cnfgrIsComponentPresent(L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)
  {
    if ((*ipAddress = simGetServPortIPAddr()) != 0)
      return;
  }

  /* Network Port */
  if ((*ipAddress = simGetSystemIPAddr()) != 0)
    return;

  /* Routing Ports */
  if (ipMapRtrIntfFirstAddress(ipAddress) == L7_SUCCESS)
    return;

  *ipAddress = 0;
  return;
}
