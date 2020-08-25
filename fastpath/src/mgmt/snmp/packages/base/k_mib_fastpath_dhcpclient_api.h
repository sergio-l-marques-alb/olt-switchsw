/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename k_mib_fastpath_dhcpclient_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create 08/01/2010
*
* @author ashadeep
*
* @end
*
**********************************************************************/

#include "usmdb_util_api.h"
#include "usmdb_dhcp_client.h"
#include "usmdb_iputil_api.h"
#include "usmdb_ip_api.h"

/*********************************************************************
* @purpose  Gets the valid interface number
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    extIfNum        @b{(output)} external interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientInterfaceGet(L7_uint32 UnitIndex, L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;
  L7_int32 temp_val;
  L7_INTF_IP_ADDR_METHOD_t method;
  L7_IP_ADDR_t ipAddr = 0;

  /* check to see if this is a valid routing interface */
  if(usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS &&
     usmDbValidateRtrIntf(UnitIndex, intIfNum) == L7_SUCCESS &&
     usmDbVisibleInterfaceCheck(UnitIndex, intIfNum, &temp_val) == L7_SUCCESS &&
     usmDbIpIntfExists(UnitIndex, intIfNum) == L7_TRUE &&
     usmDbIpRtrIntfIpAddressMethodGet(intIfNum, &method) == L7_SUCCESS &&
     (method == L7_INTF_IP_ADDR_METHOD_DHCP) &&
     (usmDbIpDhcpClientIPAddressGet(intIfNum, &ipAddr) == L7_SUCCESS) &&
     (ipAddr != 0))
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the next valid interface number
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    *extIfNum       @b{(output)} external interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientInterfaceNextGet(L7_uint32 UnitIndex,
                                            L7_uint32 *extIfNum)
{
  /* loop through available external interface numbers */
  while (nimGetNextExtIfNumber(*extIfNum, extIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a valid routing interface */
    if (snmpAgentDhcpClientInterfaceGet(UnitIndex, *extIfNum) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the DHCP Client IP Address on an interface
*
* @param    UnitIndex       @b{(input)} Unit for this operation
* @param    intIfIndex      @b{(input)} internal interface number
* @param    *CliAddr        @b{(output)}Pointer to the DHCP Client IP Address
*                                       of an interface
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientIPAddressGet(L7_uint32 UnitIndex,
                                        L7_uint32 intIfIndex,
                                        L7_uint32 *CliAddr)
{
  if (usmDbIpDhcpClientIPAddressGet (intIfIndex, CliAddr) != L7_SUCCESS)
  {
    *CliAddr = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DHCP Client Subnet Mask on an interface
*
* @param    UnitIndex       @b{(input)}  Unit for this operation
* @param    intIfIndex      @b((input))  Internal Interface Number
* @param    *CliMask        @b((output)) Pointer to the Subnet Mask of the
*                                        interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientNetworkMaskGet (L7_uint32 UnitIndex,
                                           L7_uint32 intIfIndex,
                                           L7_uint32 *CliMask)
{
  if (usmDbIpDhcpClientNetworkMaskGet (intIfIndex, CliMask) != L7_SUCCESS)
  {
    *CliMask = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the DHCP Server IP Address on an interface
*
* @param    UnitIndex       @b{(input)}  Unit for this operation
* @param    intIfIndex      @b((input))  Internal Interface Number
* @param    *SrvAddr        @b((output)) Pointer to the DHCP Server IP Address
*                                        on an interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientDhcpServerIPAddressGet (L7_uint32 UnitIndex,
                                                   L7_uint32 intIfIndex,
                                                   L7_uint32 *SrvAddr)
{
  if (usmDbIpDhcpClientDhcpServerIPAddressGet (intIfIndex, SrvAddr) != L7_SUCCESS)
  {
    *SrvAddr = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the current state of the DHCP Client
*
* @param    UnitIndex       @b{(input)}  Unit for this operation
* @param    intIfIndex      @b((input))  Internal Interface Number
* @param    *clientState    @b((output)) Pointer to the DHCP Client
*                                        State
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientStateGet (L7_uint32 UnitIndex,
                                     L7_uint32 intIfIndex,
                                     L7_uint32 *clientstate)
{
  if (usmDbIpDhcpClientStateGet (intIfIndex, clientstate) != L7_SUCCESS)
  {
    *clientstate = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Trasaction ID of the DHCP Client
*
* @param    UnitIndex       @b{(input)}  Unit for this operation
* @param    intIfIndex      @b((input))  Internal Interface Number
* @param    *transId        @b((output)) Pointer to the DHCP Client
*                                        Transaction ID
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientTransactionIdGet (L7_uint32 UnitIndex,
                                             L7_uint32 intIfIndex,
                                             L7_uchar8 *transId)
{
  L7_uint32 hexId = 0;
  if (usmDbIpDhcpClientTransactionIdGet (intIfIndex, &hexId) != L7_SUCCESS)
  {
    hexId = 0;
  }

  osapiSnprintf(transId, SNMP_BUFFER_LEN, "ox%x", hexId);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Lease Time information of the DHCP Client
*
* @param    UnitIndex       @b{(input)}  Unit for this operation
* @param    intIfIndex      @b((input))  Internal Interface Number
* @param    *leaseTime      @b((output)) Pointer to the DHCP Client
*                                        Lease Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientLeaseTimeInfoGet (L7_uint32 UnitIndex,
                                             L7_uint32 intIfIndex,
                                             L7_uint32 *leaseTime)
{
  L7_uint32 renewTime = 0, rebindTime = 0;
  if (usmDbIpDhcpClientLeaseTimeInfoGet (intIfIndex, leaseTime, &renewTime,
                                            &rebindTime) == L7_SUCCESS)
  {
    *leaseTime = *leaseTime * 100;
  }
  else
  {
    *leaseTime = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Lease Time information of the DHCP Client
*
* @param    UnitIndex       @b{(input)}  Unit for this operation
* @param    intIfIndex      @b((input))  Internal Interface Number
* @param    *renewalTime    @b((output)) Pointer to the DHCP Client
*                                        Renewal Time
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientRenewTimeInfoGet(L7_uint32 UnitIndex,
                                            L7_uint32 intIfIndex,
                                            L7_uint32 *renewalTime)
{
  L7_uint32 leaseTime = 0, rebindTime = 0;
  if (usmDbIpDhcpClientLeaseTimeInfoGet (intIfIndex, &leaseTime, renewalTime,
                                            &rebindTime) == L7_SUCCESS)
  {
    *renewalTime = *renewalTime * 100;
  }
  else
  {
    *renewalTime = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Lease Time information of the DHCP Client
*
* @param    UnitIndex       @b{(input)}  Unit for this operation
* @param    intIfIndex      @b((input))  Internal Interface Number
* @param    *rebindTime     @b((output)) Pointer to the DHCP Client
*                                        Rebinding Time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientRebindTimeInfoGet (L7_uint32 UnitIndex,
                                              L7_uint32 intIfIndex,
                                              L7_uint32 *rebindTime)
{
  L7_uint32 leaseTime = 0, renewTime = 0;
  if (usmDbIpDhcpClientLeaseTimeInfoGet (intIfIndex, &leaseTime, &renewTime,
                                            rebindTime) == L7_SUCCESS)
  {
    *rebindTime = *rebindTime * 100;
  }
  else
  {
    *rebindTime = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Retry count information of the DHCP Client
*
* @param    UnitIndex       @b{(input)}  Unit for this operation
* @param    intIfIndex      @b((input))  Internal Interface Number
* @param    *retryCount     @b((output)) Pointer to the DHCP Client's
*                                        Retry Count
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t snmpAgentDhcpClientRetryCountGet(L7_uint32 UnitIndex,
                                         L7_uint32 intIfIndex,
                                         L7_uint32 *retryCount)
{
  if (usmDbIpDhcpClientRetryCountGet (intIfIndex, retryCount) != L7_SUCCESS)
  {
    *retryCount = 0;
  }
  return L7_SUCCESS;
}

