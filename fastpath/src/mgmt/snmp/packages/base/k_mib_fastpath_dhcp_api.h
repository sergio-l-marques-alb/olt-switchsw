/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\mgmt\snmp\snmp_sr\snmpd\unix\k_mib_dhcp_private_api.h
*
* @purpose  Provide interface to DHCP Private MIB
*
* @component SNMP
*
* @comments
*
* @create 10/07/2003
*
* @author Kumar Manish
* @end
*
**********************************************************************/
#include <k_private_base.h>
#include <usmdb_dhcps_api.h>
#include "dhcps_exports.h"
#include "snmp_exports.h"
#include "usmdb_common.h"
#include "usmdb_dhcps_api.h"
#include "usmdb_util_api.h"

#ifdef L7_DHCPS_PACKAGE

/*********************************************************************
* @purpose  Get the DHCP Server Admin Mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    val         @b{(input)} value of Admin Mode
*
*
* @returns  L7_SUCCESS  Admin Mode is get
* @returns  L7_FAILURE  Failed to get Admin Mode
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;


  rc = usmDbDhcpsAdminModeGet(USMDB_UNIT_CURRENT,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE :
      *val = D_agentDhcpServerAdminMode_enable;
      break;

    case L7_DISABLE :
      *val = D_agentDhcpServerAdminMode_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Set the DHCP Server Admin Mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    val         @b{(input)} value of Admin Mode to be set
*
*
* @returns  L7_SUCCESS  Admin Mode is set
* @returns  L7_FAILURE  Failed to set Admin Mode
*
* @comments
*
* @end
*********************************************************************/


L7_RC_t
snmpAgentDhcpsAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;

  switch (val)
  {
  case D_agentDhcpServerAdminMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentDhcpServerAdminMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }


  rc = usmDbDhcpsAdminModeSet(USMDB_UNIT_CURRENT, temp_val);

  return rc;
}
/*********************************************************************
* @purpose  Set the number of DHCP Server Ping Packets
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    intPingPkts @b{(input)} number of Ping Packets to be set
*
*
* @returns  L7_SUCCESS  Ping Packets is set
* @returns  L7_FAILURE  Failed to set the ping packets
*
* @comments If the number of ping-packets is 0, it calls 
*           function usmDbDhcpsNoOfPingPktReset().
*           If the number of ping-packets is within allowable-range it calls
*           function usmDbDhcpsNoOfPingPktSet().
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsNoOfPingPktSet(L7_uint32 UnitIndex, L7_uint32 intPingPkts)
{
  L7_RC_t rc;

  if ((intPingPkts > L7_DHCPS_PINGPKTNO_MAX || intPingPkts < L7_DHCPS_PINGPKTNOSET_MIN) &&
       intPingPkts != L7_DHCPS_PINGPKTNO_MIN)
  {
      rc = L7_FAILURE;
  }
  else if (intPingPkts == L7_DHCPS_PINGPKTNO_MIN)
  {
      rc = usmDbDhcpsNoOfPingPktReset(UnitIndex);
  }
  else
  {
      rc = usmDbDhcpsNoOfPingPktSet(UnitIndex, intPingPkts);
  }

  return rc;
}

/*********************************************************************
* @purpose  Check if a DHCP clear Statistics is True or not
*
* @param    val   @b{(input)} enable/disable
* @param    UnitIndex    @b{(input)} Unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP WALK operation
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsClearStatistics(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;

  switch (val)
  {
  case D_agentDhcpServerClearStatistics_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentDhcpServerClearStatistics_disable:
    return L7_SUCCESS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }


  rc = usmDbDhcpsStatisticsClear(temp_val);

  return rc;
}

/*********************************************************************
* @purpose  Check if a DHCP clear binding All is True or not
*
* @param    val   @b{(input)} enable/disable
* @param    UnitIndex    @b{(input)} Unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP WALK operation
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsBindingClearAll(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;

  switch (val)
  {
  case D_agentDhcpServerLeaseClearAllBindings_enable:
    temp_val = L7_DHCPS_ACTIVE_LEASE;
    break;

  case D_agentDhcpServerLeaseClearAllBindings_disable:
    return L7_SUCCESS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }


  rc = usmDbDhcpsBindingClearAll(USMDB_UNIT_CURRENT, temp_val);

  return rc;
}
/*********************************************************************
* @purpose  Check if a DHCP pool corresponding to the poolIndex is valid or not
*
* @param    poolIndex   @b{(input)} pool number
* @param    UnitIndex    @b{(input)} Unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP WALK operation
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpDhcpsPoolValidate(L7_uint32 UnitIndex, L7_uint32 poolIndex)
{

  if (usmDbDhcpsPoolValidate(UnitIndex, poolIndex) != L7_FAILURE)
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next valid DHCP pool number
*
* @param    UnitIndex        @b{(input)} Unit for this operation
* @param    poolIndex        @b{(input)} pool number
* @param    poolIndexNext    @b{(output)} next pool number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP WALK operation
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpDhcpsPoolGetNext(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uint32 *poolIndexNext )
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_FAILURE;
  if (usmDbDhcpsPoolGetNext(UnitIndex, poolIndex, &temp_val) != L7_FAILURE)
  {
      *poolIndexNext = temp_val;
      rc = L7_SUCCESS;

  }
  return rc;
}

/*********************************************************************
* @purpose  Get the pool name corresponding to a valid pool number
*
* @param    UnitIndex        @b{(input)} Unit for this operation
* @param    poolIndex       @b{(input)} pool Number
* @param    pPoolName        @b{(output)} pool Name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP GET / SET operation, where it is required
* @notes  to convert pool number to pool name for calling the USMDB apis
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolNameGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uchar8 *poolName )
{
  L7_RC_t rc = L7_FAILURE;
  if (usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_FAILURE)
  {
      rc = L7_SUCCESS;
  }
  return rc;
}
/*********************************************************************
* @purpose  Get DHCP host Routers for a DHCP Server
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    defRouterList  @b{(output)} String containing the list of default-routers
*
* @returns  L7_SUCCESS  Got the Routers
* @returns  L7_FAILURE  Failed to Get Routers
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolDefRouterGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uchar8 *defRouterList )
{
  L7_uint32 routerNumber = 0;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];


  L7_RC_t rc = L7_FAILURE;

  L7_uint32 intIPaddr[L7_DHCPS_DEFAULT_ROUTER_MAX];

  L7_uchar8 routerAddress [SNMP_BUFFER_LEN];

  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(routerAddress, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }


  if( usmDbDhcpsRoutersGet(UnitIndex, poolName, intIPaddr) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }


  /* By calling the above usmDb call, we get an array of default routers' IP addresses,
     which we need to convert to a string default router list
     in 10.10.1.1,157.227.10.1,192.168.36.1;(no spaces in between) format */
  while(intIPaddr[routerNumber] != L7_NULL && routerNumber < L7_DHCPS_DEFAULT_ROUTER_MAX)
  {
     usmDbInetNtoa(intIPaddr[routerNumber],routerAddress);
     if(routerNumber != 0)
         strcat(defRouterList,",");

     strcat(defRouterList,routerAddress);

     routerNumber++;
  }

  strcat( defRouterList, ";");

  if((strcmp(defRouterList,";") == 0))
     strcpy(defRouterList,"Not Set");

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Set DHCP host Routers for a DHCP Server
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    defRouterList  @b{(input)} String containing the list of default-routers
*
* @returns  L7_SUCCESS  Routers set
* @returns  L7_FAILURE  Failed to set Routers
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolDefRouterSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uchar8 *defRouterList )
{
  L7_uint32 routerNumber = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];


  L7_uchar8 strRouter [SNMP_BUFFER_LEN];


  L7_uint32 routerLen;

  L7_uint32 intIPaddr[L7_DHCPS_DEFAULT_ROUTER_MAX];

  L7_char8 *pbuf = NULL ;


  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(strRouter, SNMP_BUFFER_LEN);

  /* zero out intIpAddr array */
  bzero((char*)intIPaddr, sizeof(intIPaddr));


  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  
  
  /* If input defRouterList is 'null' or 'NULL', then the
   existing Default router list should be removed */
   if((strcmp(defRouterList,"null")==0)||(strcmp(defRouterList,"NULL")==0))
   {
     rc = usmDbDhcpsDefaultRoutersIpReset(UnitIndex,poolName);
     return rc;
   }

  /* Here it is expected that the Default Router  List
     is in 10.1.1.1,157.227.20.1,192.168.36.1;(no spaces in between) format */

  while((pbuf = strchr(defRouterList,',')) != NULL)
  {
      routerLen = strlen(defRouterList) - strlen(pbuf);
      strncpy(strRouter,defRouterList,routerLen);

      /* Convert the IP address into an integer */
      if ( usmDbInetAton(strRouter, &intIPaddr[routerNumber]) != L7_SUCCESS )
      {
        return L7_FAILURE;
      }
      routerNumber++;
      strcpy(defRouterList,pbuf+1);

      /* check that there aren't too many routers provided */
      if (routerNumber >= L7_DHCPS_DEFAULT_ROUTER_MAX)
        return L7_FAILURE;
  }

  pbuf=strchr(defRouterList,';');

  if(pbuf == NULL)  /* token separator not found */
    return L7_FAILURE;

  routerLen = strlen(defRouterList) - strlen(pbuf);
  strncpy(strRouter,defRouterList,routerLen);

  /* Convert the IP address into an integer */
  if ( usmDbInetAton(strRouter, &intIPaddr[routerNumber]) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }

  rc = usmDbDhcpsRoutersSet(UnitIndex, poolName, intIPaddr);

  return rc;
}

/*********************************************************************
* @purpose  Get DNS IP Servers list available to a DHCP client
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    dnsServerList  @b{(output)} String containing the list of DNS Server IPs
*
* @returns  L7_SUCCESS  Got the Dns Server IP addresses
* @returns  L7_FAILURE  Failed to Get Dns Server IP addresses
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolDnsServerGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uchar8 *dnsServerList )
{
  L7_uint32 dnsNumber = 0;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];


  L7_RC_t rc = L7_FAILURE;

  L7_uint32 intIPaddr[L7_DHCPS_DNS_SERVER_MAX];

  L7_uchar8 dnsIpAddress [SNMP_BUFFER_LEN];

  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(dnsIpAddress, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }


  if( usmDbDhcpsDnsServersGet(UnitIndex, poolName, intIPaddr) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }


  /* By calling the above usmDb call, we get an array of DNS Server IP addresses,
     which we need to convert to a string DNS Server IP list
     in 10.10.1.1,157.227.10.1,192.168.36.1;(no spaces in between) format */
  while(intIPaddr[dnsNumber] != L7_NULL && dnsNumber < L7_DHCPS_DNS_SERVER_MAX)
  {
     usmDbInetNtoa(intIPaddr[dnsNumber],dnsIpAddress);
     if(dnsNumber != 0)
         strcat(dnsServerList,",");

     strcat(dnsServerList,dnsIpAddress);

     dnsNumber++;
  }

  strcat( dnsServerList, ";");

  if((strcmp(dnsServerList,";") == 0))
     strcpy(dnsServerList,"Not Set");

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Set DNS IP Servers list  to a DHCP client
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    dnsServerList  @b{(input)} String containing the list of DNS Servers
*
* @returns  L7_SUCCESS  Dns Server IP address set
* @returns  L7_FAILURE  Failed to set DNS Server's IP address
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolDnsServerSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uchar8 *dnsServerList )
{
  L7_uint32 dnsNumber = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];


  L7_uchar8 strDnsServer [SNMP_BUFFER_LEN];

  L7_uint32 dnsServerLen;

  L7_uint32 intIPaddr[L7_DHCPS_DNS_SERVER_MAX];

  L7_char8 *pbuf = NULL ;

  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(strDnsServer, SNMP_BUFFER_LEN);

  /* zero out intIpAddr array */
  bzero((char*)intIPaddr, sizeof(intIPaddr));

  /* Getting the pool name corresponding to this pool Index */
  if (usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
    return L7_FAILURE;

  
  /* If input strDnsServer is 'null' or 'NULL', then the
  existing DNS server  list should be removed */
  if((strcmp(dnsServerList,"null")==0)||(strcmp(dnsServerList,"NULL")==0))
  {
    rc  = usmDbDhcpsDnsServersReset(UnitIndex,poolName);
    return rc;
  }
 

  /* Here it is expected that the DNS Server IP  List
     is in 10.1.1.1,157.227.20.1,192.168.36.1;(no spaces in between) format */

  while((pbuf = strchr(dnsServerList,',')) != NULL)
  {
      dnsServerLen = strlen(dnsServerList) - strlen(pbuf);
      strncpy(strDnsServer,dnsServerList,dnsServerLen);

      /* Convert the IP address into an integer */
      if ( usmDbInetAton(strDnsServer, &intIPaddr[dnsNumber]) != L7_SUCCESS )
      {
        return L7_FAILURE;
      }
      dnsNumber++;
      strcpy(dnsServerList,pbuf+1);

      /* check that there aren't too many DNS Servers provided */
      if (dnsNumber >= L7_DHCPS_DNS_SERVER_MAX)
        return L7_FAILURE;
  }

  pbuf=strchr(dnsServerList,';');

  if(pbuf == NULL)  /* token separator not found */
    return L7_FAILURE;

  dnsServerLen = strlen(dnsServerList) - strlen(pbuf);
  strncpy(strDnsServer,dnsServerList,dnsServerLen);

  /* Convert the IP address into an integer */
  if ( usmDbInetAton(strDnsServer, &intIPaddr[dnsNumber]) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }

  rc = usmDbDhcpsDnsServersSet(UnitIndex, poolName, intIPaddr);

  return rc;
}


/*********************************************************************
* @purpose  Get DHCP lease time
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    leaseTime   @b{(output)} lease time (in minutes)
* @param    poolIndex   @b{(input)} valid pool index
*
* @returns  L7_SUCCESS  lease time received
* @returns  L7_FAILURE  Failed to get lease time
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolLeaseTimeGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uint32 *leaseTime )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  /* The below USMDB function takes poolName as the 3rd argument,
  unlike other USMDB functions . Must inform the DHCP Server apps team*/
  if (usmDbDhcpsLeaseTimeGet(UnitIndex, leaseTime, poolName) != L7_FAILURE)
  {
      rc = L7_SUCCESS;

  }
  return rc;
}

/*********************************************************************
* @purpose  Set DHCP lease time
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    leaseTime   @b{(input)} lease time (in minutes)
* @param    poolIndex   @b{(input)} valid pool index
*
* @returns  L7_SUCCESS  lease time set
* @returns  L7_FAILURE  Failed to set lease time
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolLeaseTimeSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uint32 leaseTime )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  /* As DHCP Server Application requirement, we need to enter lease-time in minutes,
     so converting the time-ticks to minutes */
  leaseTime = leaseTime;

  /* The below USMDB function takes poolName as the 3rd argument,
  unlike other DHCP SERVER Apps USMDB functions.  */
  rc = usmDbDhcpsLeaseTimeSet(UnitIndex,  leaseTime, poolName);
  return rc;
}
/*********************************************************************
* @purpose  Get the DHCP Server pool type
*
* @param    *poolIndex   @b{(input)} valid pool index
* @param    *pPoolType   @b{(output)} pool type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolTypeGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uint32 *poolType )
{
  L7_RC_t rc = L7_FAILURE;

  L7_uint32 tempPoolType ;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
  {
      rc = L7_SUCCESS;

  }

  if (rc == L7_SUCCESS)
  {
    switch (tempPoolType)
    {
      case L7_DHCPS_INACTIVE_POOL :
        *poolType = D_agentDhcpServerPoolType_un_allocated;
        break;

      case L7_DHCPS_DYNAMIC_POOL :
        *poolType = D_agentDhcpServerPoolType_dynamic;
        break;

      case L7_DHCPS_MANUAL_POOL :
        *poolType = D_agentDhcpServerPoolType_manual;
        break;

      default:
        /* unknown value */
        rc = L7_FAILURE;
        break;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Set the Row Status field
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    dhcpPoolRowStatus   @b{(input)} Row Status
* @param    poolIndex   @b{(input)} valid pool index
*
* @returns  L7_SUCCESS  row-status is set
* @returns  L7_FAILURE  Failed to set row-status
*
* @comments  Row-status field has been partially implemented.
*            If user sets it todestroy, the pool-entry is deleted.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolRowStatusSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                               L7_uint32 dhcpPoolRowStatus)

{
    L7_RC_t rc = L7_SUCCESS;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }


    switch (dhcpPoolRowStatus)
    {
    case D_agentDhcpServerPoolRowStatus_active:

      break;

    case D_agentDhcpServerPoolRowStatus_destroy:

      rc = usmDbDhcpsPoolDelete(UnitIndex, poolName);
      break;

    default:
      rc = L7_FAILURE;
    }

  return rc;
}

/*********************************************************************
* @purpose  Get the Row Status field
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    dhcpPoolRowStatus   @b{(output)} Row Status

*
* @returns  L7_SUCCESS  row-status is get
* @returns  L7_FAILURE  Failed to get row-status
*
* @comments  If a valid pool-name exists, the row-status field 'll always show valid.
*
* @end
*********************************************************************/


L7_RC_t
snmpAgentDhcpsPoolRowStatusGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                               L7_uint32 *dhcpPoolRowStatus)

{
    L7_RC_t rc = L7_SUCCESS;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

        /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    /* As table-entry contains valid poolIndex and poolName, rowStatus should show 'active' */
    *dhcpPoolRowStatus = D_agentDhcpServerPoolRowStatus_active;

  return rc;


}
/*********************************************************************
* @purpose  Create a DHCP Pool
*
* @param    UnitIndex  @b{(input)} Unit for this operation
* @param    poolName   @b{(input)} pool name

*
* @returns  L7_SUCCESS  DHCP pool is created sucessfully
* @returns  L7_FAILURE  Failed to create DHCP pool
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/

L7_RC_t
snmpDhcpsPoolCreate(L7_uint32 UnitIndex, L7_uchar8 *poolName)

{
    L7_RC_t rc = L7_SUCCESS;

    rc = usmDbDhcpsPoolCreate(UnitIndex, poolName);

    return rc;
}


/*********************************************************************
* @purpose  Get the IP Address field
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    ipAddress   @b{(output)} IP

*
* @returns  L7_SUCCESS  IP address is get
* @returns  L7_FAILURE  Failed to get ip-address
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsServerDynamicPoolIpAddressGet(L7_uint32  UnitIndex, L7_uint32 poolIndex,
                           L7_uint32* ipAddress)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 temp_mask,tempPoolType;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
    {
      rc = L7_SUCCESS;

    }

    if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_DYNAMIC_POOL)
    {
        rc =  usmDbDhcpsNetworkGet(UnitIndex, poolName, ipAddress, &temp_mask);
    }else
    {
        *ipAddress = L7_NULL;
        rc = L7_SUCCESS;
    }

    return rc;
}

/*********************************************************************
* @purpose  Sets the IP Address field
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    ipAddress   @b{(output)} IP

*
* @returns  L7_SUCCESS  IP address is get
* @returns  L7_FAILURE  Failed to get ip-address
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsServerDynamicPoolIpAddressSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                            L7_uint32 ipAddress)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    rc =  usmDbDhcpsNetworkIpAddrSet(UnitIndex, poolName, ipAddress);

    return rc;
}

/*********************************************************************
* @purpose  Get the IP Mask field
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    ipMask   @b{(output)} IP Mask

*
* @returns  L7_SUCCESS  IP Mask is get
* @returns  L7_FAILURE  Failed to get ip-mask
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsServerDynamicPoolIpMaskGet(L7_uint32  UnitIndex, L7_uint32 poolIndex,
                           L7_uint32* ipMask)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 temp_ip,tempPoolType;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
    {
      rc = L7_SUCCESS;

    }
    if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_DYNAMIC_POOL)
    {
      rc =  usmDbDhcpsNetworkGet(UnitIndex, poolName, &temp_ip, ipMask);
    }else
    {
        *ipMask = L7_NULL;
        rc = L7_SUCCESS;
    }

    return rc;
}

/*********************************************************************
* @purpose  Set the DHCP Server network mask for a pool
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    *pPoolName   @b{(input)} pool name
* @param    ipMask       @b{(input)} network mask for a pool
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsServerDynamicPoolIpMaskSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                            L7_uint32 ipMask)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    rc =  usmDbDhcpsNetworkMaskSet(UnitIndex, poolName, ipMask);

    return rc;
}

/*********************************************************************
* @purpose  Get the IP Prefix field
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    ipMask   @b{(output)} IP Prefix-length

*
* @returns  L7_SUCCESS  IP prefix-length is get
* @returns  L7_FAILURE  Failed to get ip prefix-length
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsServerDynamicPoolIpPrefixGet(L7_uint32  UnitIndex, L7_uint32 poolIndex,
                           L7_uint32* ipPrefix)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 temp_ip,ipMask,tempPoolType;
    L7_uint32 length=0;
    L7_uint32 bitValue;

    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);
    bitValue = 0X80000000;

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }




    if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
    {
      rc = L7_SUCCESS;

    }
    if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_DYNAMIC_POOL)
    {
      rc =  usmDbDhcpsNetworkGet(UnitIndex, poolName, &temp_ip, &ipMask);

      while(ipMask & bitValue)
      {
        length ++;
        bitValue = bitValue >> 1;
      }

      *ipPrefix = length;
    }else
    {
      *ipPrefix = L7_NULL;
      rc = L7_SUCCESS;
    }


    return rc;
}

/*********************************************************************
* @purpose  Set the DHCP Server network IP prefix length for a pool
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    *pPoolName   @b{(input)} pool name
* @param    ipMask       @b{(input)} network prefix length for a pool
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsServerDynamicPoolIpPrefixSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                            L7_uint32 ipPrefix)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 poolPrefixLength = 0xffffffff;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }


    poolPrefixLength = poolPrefixLength << (32- ipPrefix);


    rc =  usmDbDhcpsNetworkMaskSet(UnitIndex, poolName, poolPrefixLength);

    return rc;
}



/*********************************************************************
* @purpose  Get DHCP client identifier
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    clientIdentifier  @b{(output)} client-identifier
*
* @returns  L7_SUCCESS  Got the client-identifier
* @returns  L7_FAILURE  Failed to get client-identifier
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpServerManualPoolClientIdentifierGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                 L7_uchar8 *clientIdentifier )
{
  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  L7_uchar8 strClientId [L7_DHCPS_CLIENT_ID_MAXLEN];
  L7_uchar8 strConvertClientId [(L7_DHCPS_CLIENT_ID_MAXLEN*3)+1];
  L7_uchar8 tempBuf[L7_DHCPS_CLIENT_ID_MAXLEN];

  L7_RC_t rc = L7_FAILURE;
  L7_uint32 tempPoolType;
  L7_uint32 intClientIdLen,counter;


  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(strClientId, L7_DHCPS_CLIENT_ID_MAXLEN);
  bzero(strConvertClientId, sizeof(strConvertClientId));
  bzero(tempBuf, L7_DHCPS_CLIENT_ID_MAXLEN);


  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
  {
      rc = L7_SUCCESS;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
      if( usmDbDhcpsClientIdGet(UnitIndex, poolName, strClientId, &intClientIdLen) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      for (counter = 0; counter < intClientIdLen; counter++ )
      {
        sprintf(tempBuf, "%02x", strClientId[counter]);
        strcat(strConvertClientId, tempBuf);

        if (counter != intClientIdLen-1)
        {
          strcat(strConvertClientId, ":");
        }
      }
      strcpy(clientIdentifier,strConvertClientId);
  }else
  {
    strcpy(clientIdentifier," unconfigured ");
    rc =L7_SUCCESS;
  }

   return rc;
}

/*********************************************************************
* @purpose  used to create a char array for client identifier
*
* @param    buf         contains ascii string "xx:xx:xx:xx:xx:xx.....:xx:xx"
* @param    clientIdLength   provides the length of the client identifier
* @param    clientId         return hex data
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/
L7_BOOL snmpConvertClientId(L7_uchar8 *buf, L7_uchar8 *clientId, L7_uint32 *clientIdBytes)
{
  L7_uint32 i,j, digit_count=0;
  L7_uint32 clientLen;
  L7_uchar8 client_identifier[L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE];

  clientLen = strlen(buf);

  if ((clientLen % 3) != 2)
  {
    return L7_FALSE;
  }

  if(((clientLen+1)/3) > *clientIdBytes)
    return L7_FALSE;

  for ( i=0,j=0;i<clientLen;i++,j++ )
  {
    digit_count++;
    switch ( buf[i] )
    {
    case '0':
      client_identifier[j]=0x0;
      break;
    case '1':
      client_identifier[j]=0x1;
      break;
    case '2':
      client_identifier[j]=0x2;
      break;
    case '3':
      client_identifier[j]=0x3;
      break;
    case '4':
      client_identifier[j]=0x4;
      break;
    case '5':
      client_identifier[j]=0x5;
      break;
    case '6':
      client_identifier[j]=0x6;
      break;
    case '7':
      client_identifier[j]=0x7;
      break;
    case '8':
      client_identifier[j]=0x8;
      break;
    case '9':
      client_identifier[j]=0x9;
      break;
    case 'a':
    case 'A':
      client_identifier[j]=0xA;
      break;
    case 'b':
    case 'B':
      client_identifier[j]=0xB;
      break;
    case 'c':
    case 'C':
      client_identifier[j]=0xC;
      break;
    case 'd':
    case 'D':
      client_identifier[j]=0xD;
      break;
    case 'e':
    case 'E':
      client_identifier[j]=0xE;
      break;
    case 'f':
    case 'F':
      client_identifier[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FALSE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FALSE;
      break;
    }
  }

  *clientIdBytes = (clientLen+1)/3;

  for ( i = 0; i < (*clientIdBytes); i++ )
  {
    clientId[i] = ( (client_identifier[(i*2)] << 4) + client_identifier[(i*2)+1] );
  }
  return L7_TRUE;
}


/*********************************************************************
* @purpose  Set DHCP client identifier
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    clientIdentifier  @b{(input)} String containing the DHCP client identifier
*
* @returns  L7_SUCCESS  client identifier is set
* @returns  L7_FAILURE  Failed to set the client identifier
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpServerManualPoolClientIdentifierSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                 L7_uchar8 *clientIdentifier )
{

  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  L7_uint32 intClientIdLen = L7_NULL;

  L7_uchar8 strClientId [SNMP_BUFFER_LEN];


  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(strClientId, SNMP_BUFFER_LEN);


  intClientIdLen = L7_DHCPS_CLIENT_ID_MAXLEN;
  if(snmpConvertClientId(clientIdentifier, strClientId, &intClientIdLen) !=L7_TRUE)
      return L7_FAILURE;

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  rc = usmDbDhcpsClientIdSet(UnitIndex, poolName, strClientId, intClientIdLen);

  return rc;
}

/*********************************************************************
* @purpose  Get DHCP client name
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    clientIdentifier  @b{(output)} client-name
*
* @returns  L7_SUCCESS  Got the client-name
* @returns  L7_FAILURE  Failed to get client-name
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpServerManualPoolClientNameGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                 L7_uchar8 *clientName )
{
  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  L7_uchar8 strClientName [L7_DHCPS_HOST_NAME_MAXLEN+1];
  L7_uint32 tempPoolType;
  L7_RC_t rc = L7_FAILURE;


  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(strClientName, sizeof(strClientName));


  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }


  if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
  {
      rc = L7_SUCCESS;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
      if( usmDbDhcpsClientNameGet(UnitIndex, poolName, strClientName) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      strcpy(clientName,strClientName);

  }else
  {
    strcpy(clientName," unconfigured ");
    rc =L7_SUCCESS;
  }


  return rc;
}
/*********************************************************************
* @purpose  Set DHCP client name
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    clientIdentifier  @b{(input)} String containing the DHCP client name
*
* @returns  L7_SUCCESS  client name is set
* @returns  L7_FAILURE  Failed to set the client name
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpServerManualPoolClientNameSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                 L7_uchar8 *clientName )
{

  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];

  bzero(poolName, SNMP_BUFFER_LEN);


  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  rc = usmDbDhcpsClientNameSet(UnitIndex, poolName, clientName);

  return rc;
}


/*********************************************************************
* @purpose  Get DHCP client HW Address
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    clientIdentifier  @b{(output)} Hardware address of DHCP client
*
* @returns  L7_SUCCESS  Got the client HW address
* @returns  L7_FAILURE  Failed to get client HW address
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpServerManualPoolClientHWAddrGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                 L7_uchar8 *clientHWAddr )
{
  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  L7_uchar8 strClientHWAddr [L7_DHCPS_HARDWARE_ADDR_MAXLEN];
  L7_uchar8 convertClientHWAddr [L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE];
  L7_uint32 hardwareType = L7_NULL;
  L7_uint32 tempPoolType;

  L7_RC_t rc = L7_FAILURE;


  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(strClientHWAddr, L7_DHCPS_HARDWARE_ADDR_MAXLEN);
  bzero(convertClientHWAddr, L7_DHCPS_HARDWARE_ADDR_MAXLEN);


  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }


  if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
  {
      rc = L7_SUCCESS;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
      if( usmDbDhcpsHwAddressGet(UnitIndex, poolName, strClientHWAddr,&hardwareType) != L7_SUCCESS)
      {
            return L7_FAILURE;
      }

      sprintf(convertClientHWAddr, "%02x:%02x:%02x:%02x:%02x:%02x",
                      strClientHWAddr[0], strClientHWAddr[1], strClientHWAddr[2],
                      strClientHWAddr[3], strClientHWAddr[4], strClientHWAddr[5]);

      strcpy(clientHWAddr,convertClientHWAddr);


  }else
  {
    strcpy(clientHWAddr," unconfigured ");
    rc =L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  Converts a MAC address string into hex format
*
* @param    buf   @b{(input)} MAC address in 1a:2b:3c:4d:5e:6f format
* @param    mac   @b{(output)} MAC address in hex format
*
* @returns  L7_SUCCESS  conversion successful
*
* @comments
*
* @end
*********************************************************************/


L7_BOOL snmpConvertMac(L7_uchar8 *buf, L7_uchar8 *mac)
{
  L7_uint32 i,j, digit_count=0;
  L7_uchar8 mac_address[L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE];

  if (strlen(buf) != 17)
  {      /* test string length */
    return L7_FALSE;
  }

  for ( i=0,j=0;i<17;i++,j++ )
  {
    digit_count++;
    switch ( buf[i] )
    {
    case '0':
      mac_address[j]=0x0;
      break;
    case '1':
      mac_address[j]=0x1;
      break;
    case '2':
      mac_address[j]=0x2;
      break;
    case '3':
      mac_address[j]=0x3;
      break;
    case '4':
      mac_address[j]=0x4;
      break;
    case '5':
      mac_address[j]=0x5;
      break;
    case '6':
      mac_address[j]=0x6;
      break;
    case '7':
      mac_address[j]=0x7;
      break;
    case '8':
      mac_address[j]=0x8;
      break;
    case '9':
      mac_address[j]=0x9;
      break;
    case 'a':
    case 'A':
      mac_address[j]=0xA;
      break;
    case 'b':
    case 'B':
      mac_address[j]=0xB;
      break;
    case 'c':
    case 'C':
      mac_address[j]=0xC;
      break;
    case 'd':
    case 'D':
      mac_address[j]=0xD;
      break;
    case 'e':
    case 'E':
      mac_address[j]=0xE;
      break;
    case 'f':
    case 'F':
      mac_address[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FALSE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FALSE;
      break;
    }
  }

  for ( i = 0; i < 6; i++ )
  {
    mac[i] = ( (mac_address[(i*2)] << 4) + mac_address[(i*2)+1] );
  }
  return L7_TRUE;
}
/*********************************************************************
* @purpose  Set DHCP client HW Address
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    clientIdentifier  @b{(input)} String containing the DHCP client HW address
*
* @returns  L7_SUCCESS  client hardware address is set
* @returns  L7_FAILURE  Failed to set the client hardware address
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpServerManualPoolClientHWAddrSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                 L7_uchar8 *clientHWAddr )
{

  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];

  L7_uchar8 strClientHWAddr [SNMP_BUFFER_LEN];


  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(strClientHWAddr, SNMP_BUFFER_LEN);

  if(snmpConvertMac(clientHWAddr, strClientHWAddr) !=L7_TRUE)
      return L7_FAILURE;

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  /* By default passing hardware-type as 'ethernet' */
  rc = usmDbDhcpsHwAddressSet(UnitIndex, poolName, strClientHWAddr,
                                D_agentDhcpServerManualPoolClientHWType_ethernet);

  return rc;
}

/*********************************************************************
* @purpose  Get the client hardware type
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    clientHardwareAddress   @b{(output)} Row Status

*
* @returns  L7_SUCCESS  row-status is get
* @returns  L7_FAILURE  Failed to get row-status
*
* @comments  If a valid pool-name exists, the row-status field 'll always show valid.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpServerManualPoolClientHWTypeGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                               L7_uint32 *clientHWType)

{
    L7_RC_t rc = L7_SUCCESS;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    L7_uchar8 strClientHWAddr [L7_DHCPS_HARDWARE_ADDR_MAXLEN+1];
    L7_uint32 tempPoolType;
    bzero(poolName, SNMP_BUFFER_LEN);
    bzero(strClientHWAddr, sizeof(strClientHWAddr));



        /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }



  if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
  {
      rc = L7_SUCCESS;
  }

  if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
  {
    if (usmDbDhcpsHwAddressGet(UnitIndex, poolName, strClientHWAddr,
                               clientHWType) != L7_FAILURE)
    {
      rc = L7_SUCCESS;
    }


  }else
  {
    *clientHWType = L7_NULL;
    rc =L7_SUCCESS;
  }


  return rc;

}


/*********************************************************************
* @purpose  Set the client Hardware type field
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    clientHWType   @b{(input)} Hardware type

*
* @returns  L7_SUCCESS  hardware type  is set
* @returns  L7_FAILURE  Failed to set hardware type
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpServerManualPoolClientHWTypeSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                               L7_uint32 clientHWType)

{
    L7_RC_t rc = L7_SUCCESS;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    rc = usmDbDhcpsSNMPHwTypeSet(UnitIndex,poolName,clientHWType);

  return rc;
}



/*********************************************************************
* @purpose  Get the Host IP Address
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    ipAddress   @b{(output)} IP

*
* @returns  L7_SUCCESS  IP address is get
* @returns  L7_FAILURE  Failed to get ip-address
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpServerManualPoolIpAddressGet(L7_uint32  UnitIndex, L7_uint32 poolIndex,
                           L7_uint32* ipAddress)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 temp_mask,tempPoolType;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }


    if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
    {
      rc = L7_SUCCESS;

    }

    if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
    {
        rc =  usmDbDhcpsHostGet(UnitIndex, poolName, ipAddress, &temp_mask);
    }else
    {
      *ipAddress = L7_NULL;
      rc =L7_SUCCESS;
    }


    return rc;
}

/*********************************************************************
* @purpose  Sets the host IP Address
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    ipAddress   @b{(output)} IP

*
* @returns  L7_SUCCESS  IP address is get
* @returns  L7_FAILURE  Failed to get ip-address
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpServerManualPoolIpAddressSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                            L7_uint32 ipAddress)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    rc =  usmDbDhcpsSNMPHostIpAddrSet(UnitIndex, poolName, ipAddress);

    return rc;
}

/*********************************************************************
* @purpose  Get the Host IP Mask
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    ipMask   @b{(output)} IP Mask

*
* @returns  L7_SUCCESS  IP Mask is get
* @returns  L7_FAILURE  Failed to get ip-mask
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpServerManualPoolIpMaskGet(L7_uint32  UnitIndex, L7_uint32 poolIndex,
                           L7_uint32* ipMask)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 temp_ip,tempPoolType;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
    {
      rc = L7_SUCCESS;

    }

    if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
    {
        rc =  usmDbDhcpsHostGet(UnitIndex, poolName, &temp_ip, ipMask);
    }else
    {
      *ipMask = L7_NULL;
      rc =L7_SUCCESS;
    }


    return rc;
}

/*********************************************************************
* @purpose  Set the network mask for a DHCP client
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    *pPoolName   @b{(input)} pool name
* @param    ipMask       @b{(input)} network mask for a client
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpServerManualPoolIpMaskSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                            L7_uint32 ipMask)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    rc =  usmDbDhcpsSNMPHostMaskSet(UnitIndex, poolName, ipMask);

    return rc;
}

/*********************************************************************
* @purpose  Get the IP Prefix of a DHCP client
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} valid pool index
* @param    ipMask   @b{(output)} IP Prefix-length

*
* @returns  L7_SUCCESS  IP prefix-length is get
* @returns  L7_FAILURE  Failed to get ip prefix-length
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpServerManualPoolIpPrefixLengthGet(L7_uint32  UnitIndex, L7_uint32 poolIndex,
                           L7_uint32* ipPrefix)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 temp_ip,ipMask,tempPoolType;
    L7_uint32 length=0;
    L7_uint32 bitValue;

    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);
    bitValue = 0X80000000;

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &tempPoolType) != L7_FAILURE)
    {
      rc = L7_SUCCESS;

    }

    if(tempPoolType == L7_DHCPS_INACTIVE_POOL || tempPoolType == L7_DHCPS_MANUAL_POOL)
    {
      rc =  usmDbDhcpsHostGet(UnitIndex, poolName, &temp_ip, &ipMask);


      while(ipMask & bitValue)
      {
        length ++;
       bitValue = bitValue >> 1;
      }

      *ipPrefix = length;
    }else
    {
      *ipPrefix = L7_NULL;
      rc =L7_SUCCESS;
    }
    return rc;
}

/*********************************************************************
* @purpose  Set the network IP prefix length for a DHCP client
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    *pPoolName   @b{(input)} pool name
* @param    ipMask       @b{(input)} network prefix length for a pool
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpServerManualPoolIpPrefixLengthSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                            L7_uint32 ipPrefix)

{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 poolPrefixLength = 0xffffffff;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }


    poolPrefixLength = poolPrefixLength << (32- ipPrefix);

    rc =  usmDbDhcpsSNMPHostMaskSet(UnitIndex, poolName, poolPrefixLength);
    return rc;
}

/*********************************************************************
* @purpose  Get excluded address range
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    excludedIPRange  @b{(output)} String containing the excluded IP address range
*
* @returns  L7_SUCCESS  Got the range
*
* @comments If user performs a get operation on excluded IP Address range, he 'll be notified
*            " Do Table-view for agentDhcpServerExcludedAddressRangeTable"
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolExcludedAddressRangeCreateGet(L7_uint32 UnitIndex, L7_uchar8 *excludedIPRange )
{
  L7_uint32 rc = L7_FAILURE;
  /* Get operation on exclude-address range returns null value.
     For viewing the configured excluded
     address range, one should perform walk-operation on 
     agentDhcpServerExcludedAddressRangeTable */   
  strcpy(excludedIPRange,"");

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Set an excluded address IP range
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    defRouterList  @b{(input)} String containing the list of default-routers
*
* @returns  L7_SUCCESS  Routers set
* @returns  L7_FAILURE  Failed to set excluded IP address range
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolExcludedAddressRangeCreateSet(L7_uint32 UnitIndex, L7_uchar8 *excludedIPRange)
{
  L7_RC_t rc = L7_FAILURE;

  L7_uchar8 strIPAddress [SNMP_BUFFER_LEN];


  L7_uint32 ipLen = L7_NULL;

  L7_uint32 startIPaddr = L7_NULL;
  L7_uint32 endIPaddr  = L7_NULL;

  L7_char8 *pbuf = NULL ;



  bzero(strIPAddress, SNMP_BUFFER_LEN);

  /* Here it is expected that the excluded address range
     is in 10.1.1.1-20.1.1.1;(no spaces in between) format */

  if((pbuf = strchr(excludedIPRange,'-')) != NULL)
  {
      ipLen = strlen(excludedIPRange) - strlen(pbuf);
      strncpy(strIPAddress,excludedIPRange,ipLen);

      /* Convert the IP address into an integer */
      if ( usmDbInetAton(strIPAddress, &startIPaddr) != L7_SUCCESS )
      {
        return L7_FAILURE;
      }

      strcpy(excludedIPRange,pbuf+1);


      pbuf=strchr(excludedIPRange,';');

      if(pbuf == NULL)  /* token separator not found */
        return L7_FAILURE;

      ipLen = strlen(excludedIPRange) - strlen(pbuf);
      strncpy(strIPAddress,excludedIPRange,ipLen);

      /* Convert the IP address into an integer */
      if ( usmDbInetAton(strIPAddress, &endIPaddr) != L7_SUCCESS )
      {
        return L7_FAILURE;
      }
  }else /* Only start IP address is entered */
  {
    pbuf=strchr(excludedIPRange,';');

    if(pbuf == NULL)  /* token separator not found */
      return L7_FAILURE;

    ipLen = strlen(excludedIPRange) - strlen(pbuf);
    strncpy(strIPAddress,excludedIPRange,ipLen);

    /* Convert the IP address into an integer */
    if ( usmDbInetAton(strIPAddress, &startIPaddr) != L7_SUCCESS )
    {
      return L7_FAILURE;
    }

    /* end IP Address will be equal to start IP address */
    endIPaddr = startIPaddr;
  }

  rc = usmDbDhcpsExcludedAddressRangeAdd(UnitIndex, startIPaddr, endIPaddr);

  return rc;
}


/*********************************************************************
* @purpose  Delete an  excluded IP address range
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    excludedIPRange  @b{(output)} String containing the excluded IP address range
*
* @returns  L7_SUCCESS  Got the range
*
* @comments If user performs a get operation on excluded IP Address range, he 'll be notified
*            " Do Table-view for agentDhcpServerExcludedAddressRangeTable"
*
* @end
*********************************************************************/
L7_RC_t
snmpDhcpsExcludedAddressRangeDelete(L7_uint32 UnitIndex, L7_uint32 rangeNumber )
{
  L7_uint32 rc = L7_FAILURE;

  L7_uint32 startIPAddress = L7_NULL;
  L7_uint32 endIPAddress = L7_NULL;

  rc = usmDbDhcpsSNMPExcludeAddressRangeGet(UnitIndex,rangeNumber, &startIPAddress,
                                                                   &endIPAddress);
  if(rc == L7_SUCCESS)
  {
    rc = usmDbDhcpsExcludedAddressRangeDelete(UnitIndex,startIPAddress,endIPAddress);
  }

  return rc;
}

/*********************************************************************
* @purpose  Get Next Lease IP address
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    leaseIpAddr @b{(input)} Lease IP address
* @param    pNextAddr   @b{(output)} Next Lease IP address
*
* @returns  L7_SUCCESS  Got the next IP 
*
*
* @end
*********************************************************************/
L7_RC_t
snmpDhcpsServerLeaseGetNext(L7_uint32 UnitIndex, L7_uint32 leaseIpAddr, L7_uint32 * pNextAddr)
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_FAILURE;

  if (usmDbDhcpsLeaseGetNext(UnitIndex, leaseIpAddr, &temp_val) != L7_FAILURE)
  {
    *pNextAddr = temp_val;
    rc = L7_SUCCESS;
  }
  return rc;
}


/*********************************************************************
* @purpose  Get Lease Entry data 
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    ipAddr      @b{(input)} Lease IP address 
* @param    pData       @b{(output)} Lease snmp data 
*
* @returns  L7_SUCCESS  Got the data 
*
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsServerLeaseEntryGet(L7_uint32 UnitIndex, 
  L7_uint32 ipAddr, agentDhcpServerLeaseEntry_t * pData)
{
  L7_uint32 ipMask;
  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  L7_uchar8 clientIdentifier[SNMP_BUFFER_LEN];
  L7_uchar8 hwAddr[SNMP_BUFFER_LEN];
  L7_uint32 remainingTime;
  L7_uint32 poolType;

  if(L7_NULLPTR == pData)
    return L7_FAILURE;

  memset(poolName, 0, SNMP_BUFFER_LEN);
  memset(clientIdentifier, 0, SNMP_BUFFER_LEN);
  memset(hwAddr, 0, SNMP_BUFFER_LEN);

  if(usmDbDhcpsLeaseDataGet(UnitIndex, ipAddr, 
      &ipMask, 
      hwAddr, 
      clientIdentifier, 
      poolName, 
      &remainingTime) != L7_SUCCESS)
    return L7_FAILURE;
  
  if(usmDbDhcpsPoolTypeGet(UnitIndex, poolName, &poolType) != L7_SUCCESS)
    return L7_FAILURE;

  /* set values */
  pData->agentDhcpServerLeaseIPAddress = ipAddr;
  pData->agentDhcpServerLeaseIPMask    = ipMask;

  /* convert remaining time to timeticks (100th of second) as it is returned in minutes */
  pData->agentDhcpServerLeaseRemainingTime = remainingTime*60*100;

  if(L7_DHCPS_MANUAL_POOL == poolType)
    pData->agentDhcpServerLeaseType = D_agentDhcpServerLeaseType_manual;
  else if(L7_DHCPS_DYNAMIC_POOL == poolType)
    pData->agentDhcpServerLeaseType = D_agentDhcpServerLeaseType_automatic; 
  else
    return L7_FAILURE;

  /* Since SNMP requires the hardware-address to be returned as an array 
   * of bytes instead of a string returned by usmDbDhcpsLeaseDataGet(), 
   * hardware-address is retrieved by the following usmDb call. 
   */
  memset(hwAddr, 0, SNMP_BUFFER_LEN);
  if(usmDbDhcpsLeaseHWAddrGet(UnitIndex, ipAddr, hwAddr) != L7_SUCCESS)
    return L7_FAILURE;

  if(SafeMakeOctetString(&pData->agentDhcpServerLeaseHWAddress, hwAddr, L7_MAC_ADDR_LEN) != L7_TRUE)
    return L7_FAILURE;

  return (L7_SUCCESS);
}


/*********************************************************************
* @purpose  Clear an IP address lease
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    leaseIpAddr @b{(input)} Lease IP address to be cleared
*
* @returns  L7_SUCCESS  Cleared the binding
*
* @comments If user performs a get operation on IP Address lease status, he 'll be notified
*            " Do Table-view for agentDhcpServerLeaseTable"
*
* @end
*********************************************************************/
L7_RC_t
snmpDhcpsServerLeaseDelete(L7_uint32 UnitIndex, L7_uint32 leaseIpAddr)
{
  if(usmDbDhcpsBindingClear(UnitIndex, leaseIpAddr, L7_DHCPS_ACTIVE_LEASE) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}



/************************************* Phase 2 *****************************/




/*********************************************************************
* @purpose  Get the bootp client allocation mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    val         @b{(input)} value of bootp client allocation mode
*
*
* @returns  L7_SUCCESS  bootp client allocation mode is get
* @returns  L7_FAILURE  Failed to get bootp client allocation mode
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsBootpAutomaticGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;


  rc = usmDbDhcpsBootpAutomaticGet(USMDB_UNIT_CURRENT,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE :
      *val = D_agentDhcpServerBootpAutomatic_enable;
      break;

    case L7_FALSE :
      *val = D_agentDhcpServerBootpAutomatic_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Set the bootp client allocation mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    val         @b{(input)} value of bootp client allocation mode to be set
*
*
* @returns  L7_SUCCESS  bootp client allocation mode is set
* @returns  L7_FAILURE  Failed to set bootp client allocation mode
*
* @comments
*
* @end
*********************************************************************/


L7_RC_t
snmpAgentDhcpsBootpAutomaticSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;

  switch (val)
  {
  case D_agentDhcpServerBootpAutomatic_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentDhcpServerBootpAutomatic_disable:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }


  rc = usmDbDhcpsBootpAutomaticSet(USMDB_UNIT_CURRENT, temp_val);

  return rc;
}

/*********************************************************************
* @purpose  Get Net BIOS Name-server list available to a DHCP client
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    dnsServerList  @b{(output)} String containing the list of Net BIOS 
*                          Name-servers
*
* @returns  L7_SUCCESS  Got the Net BIOS Name-servers' list
* @returns  L7_FAILURE  Failed to Get Net BIOS Name-servers' list
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsNetbiosNameServerAddressGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uchar8 *netBIOSNameServerList )
{
  L7_uint32 netBIOSNameServerNumber = 0;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];


  L7_RC_t rc = L7_FAILURE;

  L7_uint32 intIPaddr[L7_DHCPS_NETBIOS_NAME_SERVER_MAX];  

  L7_uchar8 nameServerAddress[SNMP_BUFFER_LEN];

  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(nameServerAddress, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }


  if( usmDbDhcpsNetbiosNameServerAddressGet(UnitIndex, poolName, intIPaddr) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }


  /* By calling the above usmDb call, we get an array of Net BIOS name-server addresses,
     which we need to convert to a string Net BIOS name-server list
     in 10.10.1.1,157.227.10.1,192.168.36.1;(no spaces in between) format */
  while(intIPaddr[netBIOSNameServerNumber] != L7_NULL && 
        netBIOSNameServerNumber < L7_DHCPS_NETBIOS_NAME_SERVER_MAX)
  {
     usmDbInetNtoa(intIPaddr[netBIOSNameServerNumber],nameServerAddress);
     if(netBIOSNameServerNumber != 0)
         strcat(netBIOSNameServerList,",");

     strcat(netBIOSNameServerList,nameServerAddress);

     netBIOSNameServerNumber++;
  }

  strcat( netBIOSNameServerList, ";");

  if((strcmp(netBIOSNameServerList,";") == 0))
     strcpy(netBIOSNameServerList,"Not Set");

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Set Net BIOS Name-server list to a DHCP client
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    dnsServerList  @b{(input)} String containing the list of Net BIOS
*
* @returns  L7_SUCCESS  Net BIOS Name-servers are set
* @returns  L7_FAILURE  Failed to set Net BIOS Name-servers' list
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsNetbiosNameServerAddressSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uchar8 *netBIOSNameServerList )
{
  L7_uint32 netBIOSNameServerNumber = 0;
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];


  L7_uchar8 strNameServer[SNMP_BUFFER_LEN];

  L7_uint32 nameServerLen;

  L7_uint32 intIPaddr[L7_DHCPS_NETBIOS_NAME_SERVER_MAX];

  L7_char8 *pbuf = NULL ;

  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(strNameServer, SNMP_BUFFER_LEN);

  /* zero out intIpAddr array */
  bzero((char*)intIPaddr, sizeof(intIPaddr));

  /* Getting the pool name corresponding to this pool Index */
  if (usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
    return L7_FAILURE;
  
  
  /* If input netBIOSNameServerList  is 'null' or 'NULL', then the
     existing Name server list should be removed */
  if((strcmp(netBIOSNameServerList,"null")==0)
   ||(strcmp(netBIOSNameServerList,"NULL")==0))
  {
    rc  = usmDbDhcpsNetbiosNameServerAddressDelete(UnitIndex,poolName);
    return rc;
  }

  /* Here it is expected that the Name Server IP  List
     is in 10.1.1.1,157.227.20.1,192.168.36.1;(no spaces in between) format */

  while((pbuf = strchr(netBIOSNameServerList,',')) != NULL)
  {
      nameServerLen = strlen(netBIOSNameServerList) - strlen(pbuf);
      strncpy(strNameServer,netBIOSNameServerList,nameServerLen);

      /* Convert the IP address into an integer */
      if ( usmDbInetAton(strNameServer, &intIPaddr[netBIOSNameServerNumber]) != L7_SUCCESS )
      {
        return L7_FAILURE;
      }
      netBIOSNameServerNumber++;
      strcpy(netBIOSNameServerList,pbuf+1);

      /* check that there aren't too many Net BIOS Name-servers provided */
      if (netBIOSNameServerNumber >= L7_DHCPS_NETBIOS_NAME_SERVER_MAX)
        return L7_FAILURE;
  }

  pbuf=strchr(netBIOSNameServerList,';');

  if(pbuf == NULL)  /* token separator not found */
    return L7_FAILURE;

  nameServerLen = strlen(netBIOSNameServerList) - strlen(pbuf);
  strncpy(strNameServer,netBIOSNameServerList,nameServerLen);

  /* Convert the IP address into an integer */
  if ( usmDbInetAton(strNameServer, &intIPaddr[netBIOSNameServerNumber]) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }

  rc = usmDbDhcpsNetbiosNameServerAddressSet(UnitIndex, poolName, intIPaddr);

  return rc;
}

/*********************************************************************
* @purpose  Get the DHCP Net BIOS node type
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    val         @b{(input)} value of DHCP Net BIOS node type
*
*
* @returns  L7_SUCCESS  DHCP Net BIOS node type is get
* @returns  L7_FAILURE  Failed to get DHCP Net BIOS node type
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsNetbiosNodeTypeGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                 L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uchar8 temp_val;

  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  rc = usmDbDhcpsNetbiosNodeTypeGet(USMDB_UNIT_CURRENT,poolName, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DHCPS_B_NODE :
      *val = D_agentDhcpServerPoolNetbiosNodeType_b_node;
      break;

    case L7_DHCPS_P_NODE :
      *val = D_agentDhcpServerPoolNetbiosNodeType_p_node;
      break;

    case L7_DHCPS_M_NODE :
      *val = D_agentDhcpServerPoolNetbiosNodeType_m_node;
      break;

    case L7_DHCPS_H_NODE :
      *val = D_agentDhcpServerPoolNetbiosNodeType_h_node;
      break;

    case 0:
      *val = D_agentDhcpServerPoolNetbiosNodeType_none;
      break;



    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Set the DHCP Net BIOS node type
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    val         @b{(input)} value of DHCP Net BIOS node type
*
*
* @returns  L7_SUCCESS  DHCP Net BIOS node type is set
* @returns  L7_FAILURE  Failed to set DHCP Net BIOS node type
*
* @comments
*
* @end
*********************************************************************/


L7_RC_t
snmpAgentDhcpsNetbiosNodeTypeSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;

  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  switch (val)
  {
  case D_agentDhcpServerPoolNetbiosNodeType_b_node:
    temp_val = L7_DHCPS_B_NODE;
    break;

  case D_agentDhcpServerPoolNetbiosNodeType_p_node:
    temp_val = L7_DHCPS_P_NODE;
    break;

  case D_agentDhcpServerPoolNetbiosNodeType_m_node:
    temp_val = L7_DHCPS_M_NODE;
    break;

  case D_agentDhcpServerPoolNetbiosNodeType_h_node:
    temp_val = L7_DHCPS_H_NODE;
    break;

  default:
    /* unknown value */
    return L7_FAILURE;
    break;
  }


  rc = usmDbDhcpsNetbiosNodeTypeSet(USMDB_UNIT_CURRENT,poolName, temp_val);

  return rc;
}

/*********************************************************************
* @purpose  Get the Next Server string corresponding to a valid pool number
*
* @param    UnitIndex        @b{(input)} Unit for this operation
* @param    poolIndex       @b{(input)} pool Number
* @param    pNextServer     @b{(output)} next server
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP GET / SET operation, where it is required
* @notes  to convert pool number to pool name for calling the USMDB apis
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsNextServerGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                          L7_uint32 *pNextServer )
{
  L7_RC_t rc = L7_FAILURE;

  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (usmDbDhcpsNextServerGet(UnitIndex, poolName, pNextServer) != L7_FAILURE)
  {
      rc = L7_SUCCESS;
  }
  return rc;
}


/*********************************************************************
* @purpose  Set DHCP Next Server  corresponding to a valid pool numbers
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    pNextServer  @b{(input)} String containing the DHCP Next Server
*
* @returns  L7_SUCCESS  Next Server is set
* @returns  L7_FAILURE  Failed to set the Next Server
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsNextServerSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                 L7_uint32 pNextServer )
{

  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];

  bzero(poolName, SNMP_BUFFER_LEN);


  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  
  
  /* If input next-server   is null, then the
  existing next-server list should be removed */
  if(0 == pNextServer)
  {
    rc  = usmDbDhcpsNextServerDelete(UnitIndex,poolName);
    return rc;
  }

  rc = usmDbDhcpsNextServerSet(UnitIndex, poolName, pNextServer);

  return rc;
}



/*********************************************************************
* @purpose  Get the Domain Name corresponding to a valid pool number
*
* @param    UnitIndex        @b{(input)} Unit for this operation
* @param    poolIndex       @b{(input)} pool Number
* @param    pDomainName     @b{(output)} domain name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP GET / SET operation, where it is required
* @notes  to convert pool number to pool name for calling the USMDB apis
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsDomainNameGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                            L7_uchar8 *pDomainName )
{
  L7_RC_t rc = L7_FAILURE;

  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  if (usmDbDhcpsDomainNameGet(UnitIndex, poolName, pDomainName) != L7_FAILURE)
  {
      rc = L7_SUCCESS;
  }
  return rc;
}


/*********************************************************************
* @purpose  Set DHCP Domain name  corresponding to a valid pool numbers
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    pDomainName  @b{(input)} String containing the DHCP domain name
*
* @returns  L7_SUCCESS  Domain name is set
* @returns  L7_FAILURE  Failed to set the Domain name
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsDomainNameSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                 L7_uchar8 *strDomainName )
{

  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];

  bzero(poolName, SNMP_BUFFER_LEN);
  
  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  
  
  /* If input strDomainName  is 'null' or 'NULL' string, then the
     existing Domain Name should be removed */
  if((strcmp(strDomainName,"null")==0)||(strcmp(strDomainName,"NULL")==0))
  {
    rc  =usmDbDhcpsDomainNameDelete(UnitIndex,poolName);
    return rc;
  }

  rc = usmDbDhcpsDomainNameSet(UnitIndex, poolName, strDomainName);

  return rc;
}


/*********************************************************************
* @purpose  Get the Bootfile name corresponding to a valid pool number
*
* @param    UnitIndex        @b{(input)} Unit for this operation
* @param    poolIndex       @b{(input)} pool Number
* @param    pBootfileName     @b{(output)} domain name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP GET / SET operation, where it is required
* @notes  to convert pool number to pool name for calling the USMDB apis
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsBootfileGet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                            L7_uchar8 *pBootfileName )
{
  L7_RC_t rc = L7_FAILURE;

  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  if (usmDbDhcpsBootfileGet(UnitIndex, poolName, pBootfileName) != L7_FAILURE)
  {
      rc = L7_SUCCESS;
  }
  return rc;
}


/*********************************************************************
* @purpose  Set DHCP server bootfile name  corresponding to a valid pool numbers
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    pBootfileName  @b{(input)} String containing the bootfile name
*
* @returns  L7_SUCCESS  Bootfile name is set
* @returns  L7_FAILURE  Failed to set the bootfile name
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsBootfileSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                 L7_uchar8 *strBootfileName )
{

  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 poolName [SNMP_BUFFER_LEN];
  
  bzero(poolName, SNMP_BUFFER_LEN);
  

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  
  
  /* If input strBootfileName  is 'null' or 'NULL', then the
     existing Bootfile name should be removed */
  if((strcmp(strBootfileName,"null")==0)||(strcmp(strBootfileName,"NULL")==0))
  {
    rc  =usmDbDhcpsBootfileDelete(UnitIndex,poolName);
    return rc;
  }

  rc = usmDbDhcpsBootfileSet(UnitIndex, poolName, strBootfileName);

  return rc;
}

/*********************************************************************
* @purpose  Get the pool option value
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolOption  @b{(output)} String containing the pool-option value
*
* @returns  L7_SUCCESS  Got the range
*
* @comments If user performs a get operation on excluded IP Address range, he 'll get
*            a null-string
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolOptionCreateGet(L7_uint32 UnitIndex, L7_uchar8 *poolOption )
{
  L7_uint32 rc = L7_FAILURE;
  /* Get operation on agentDhcpServerPoolOptionCreate returns null value.
     For viewing the configured excluded
     address range, one should perform walk-operation on 
     agentDhcpServerPoolOptionTable */   
  strcpy(poolOption,"");

  rc = L7_SUCCESS;
  return rc;
}
/*********************************************************************
* @purpose  Set a pool-option value
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    poolIndex    @b{(input)} Valid pool Index
* @param    poolOption  @b{(input)} String containing the pool-option 
*                     for an existing DHCP Server pool. Here, it is required 
*                     to enter the pool-index, for which option is to be set and 
*                     the value of the DHCP option code in string-format like -> 
*                     [1-19;], 1 is the pool-index of the existing pool and 19 is the option code.
*
* @returns  L7_SUCCESS  Routers set
* @returns  L7_FAILURE  Failed to set pool-option value
*
* @comments  Check return code to determine result.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolOptionCreateSet(L7_uint32 UnitIndex, L7_uchar8 *poolOption)
{
  L7_RC_t rc = L7_FAILURE;

  L7_uchar8 strPoolIndex [SNMP_BUFFER_LEN];
  L7_uchar8 strOptionCode [SNMP_BUFFER_LEN];


  L7_uint32 optionLen = L7_NULL;

  L7_uint32 poolIndex = L7_NULL;
  L7_uint32 option  = L7_NULL;

  L7_char8 *pbuf = NULL ;

  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);
  bzero(strPoolIndex, SNMP_BUFFER_LEN);
  bzero(strOptionCode, SNMP_BUFFER_LEN);

  
  /* Here it is expected that the pool-option
     is in 1-19;(no spaces in between) format */

  if((pbuf = strchr(poolOption,'-')) != NULL)
  {
      optionLen = strlen(poolOption) - strlen(pbuf);
      strncpy(strPoolIndex,poolOption,optionLen);

      /* Convert the pool-index string into an integer */ 
      poolIndex =  atoi(strPoolIndex);
      
      strcpy(poolOption,pbuf+1);


      pbuf=strchr(poolOption,';');

      if(pbuf == NULL)  /* token separator not found */
        return L7_FAILURE;

      optionLen = strlen(poolOption) - strlen(pbuf);
      strncpy(strOptionCode,poolOption,optionLen);

      /* Convert the option string into an integer */ 
      option =  atoi(strOptionCode);  

  }

  /* If code specified is not between 1 and 255 */
  if ((option < 1) || (option > 255))
  {
    return L7_FAILURE;
      
  }

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = usmDbDhcpsOptionCreate(UnitIndex, poolName, (L7_uchar8)option);

  if (rc == L7_ALREADY_CONFIGURED) 
  {
    rc = L7_FAILURE;
  }
  return rc;
}


/*********************************************************************
* @purpose  To validate a pool-option entry for a given combination 
*           of poolNumber-optionCode 
* 
*
* @param    poolIndex   @b{(input)} pool number
* @param    optionCode   @b{(input)} option-code
* @param    UnitIndex    @b{(input)} Unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP WALK operation
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpDhcpsPoolOptionValidate(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                            L7_uint32 optionCode)
{
  L7_uchar8 strOptionData [SNMP_BUFFER_LEN];
  L7_uint32 length = L7_NULL;
  L7_uchar8  dataFormat = L7_NULL;
  
  char status = L7_NULL;

  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  bzero(strOptionData, SNMP_BUFFER_LEN);

  if (usmDbDhcpsPoolValidate(UnitIndex, poolIndex) != L7_SUCCESS)
    return L7_FAILURE;


  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  /* If get operation for this poolName-optionCode combination is successfull, then
     this row-entry of pool-option table is validated */
  if (usmDbDhcpsOptionGet(UnitIndex, poolName, (L7_uchar8)optionCode, strOptionData,
                          &length, &dataFormat, &status) != L7_SUCCESS)
    return L7_FAILURE;


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if a DHCP pool-option corresponding to the poolIndex 
*           and option-code, is valid or not
*
* @param    UnitIndex    @b{(input)} Unit for this operation
* @param    poolIndex   @b{(input)} pool number
* @param    optionCode   @b{(input)} DHCP option code
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP WALK operation
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolOptionEntryGet(L7_uint32 UnitIndex, L7_uint32 poolIndex, L7_uint32 optionCode,
                            agentDhcpServerPoolOptionEntry_t* pOptionEntryData)
{
  L7_RC_t rc;
  L7_uchar8 strOptionData [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 strData [3*L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 strIpData [3*L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 strIpAddr [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_char8  temp[3];
  L7_uint32 optionDataLen = L7_NULL;
  L7_uchar8 dataFormat = L7_NULL;
  L7_uint32 firstChar, lastChar, ipLen;
  L7_uint32  intDataFormat, counter, noOfIpAddresses, ipAddr;
  char status= L7_NULL;

  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  bzero(strOptionData, L7_DHCPS_POOL_OPTION_LENGTH);

  bzero(strData, sizeof(strData));
  bzero(strIpData, sizeof(strIpData));
  bzero(strIpAddr, L7_DHCPS_POOL_OPTION_LENGTH);
  
    /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  /* If get operation for this poolName-optionCode combination is successfull, then
     this row-entry of pool-option table is validated */
  if (usmDbDhcpsOptionGet(UnitIndex, poolName, (L7_uchar8)optionCode, strOptionData,
                          &optionDataLen, &dataFormat, &status) != L7_SUCCESS)
    return L7_FAILURE;

  /* Getting values for agentDhcpServerPoolOptionEntry */

  /* Getting pool-name */
  if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerOptionPoolName, poolName) != L7_TRUE)
  {
    return L7_FAILURE;
  }
   

  intDataFormat = (L7_uint32)dataFormat;
  
  /* Getting option data */
  switch(intDataFormat)
  {
    
    case L7_DHCPS_NONE :
      strcpy(strOptionData, "");

      /* In this scenario, all types of option-data should show null-string */
      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionAsciiData, 
                                   strOptionData) != L7_TRUE)
      {
          return L7_FAILURE;
      }

      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionHexData, 
                                   strOptionData) != L7_TRUE)
      {
          return L7_FAILURE;
      }

      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionIpAddressData, 
                                   strOptionData) != L7_TRUE)
      {
          return L7_FAILURE;
      }
        
      break;

    case L7_DHCPS_ASCII :
      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionAsciiData, 
                                   strOptionData) != L7_TRUE)

      {
          return L7_FAILURE;
      }

      
      /* In this scenario, Hex and IP types of option-data should show null-string */

      strcpy(strOptionData, "");

      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionHexData, 
                                   strOptionData) != L7_TRUE)
      {
          return L7_FAILURE;
      }

      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionIpAddressData, 
                                   strOptionData) != L7_TRUE)
      {
          return L7_FAILURE;
      }

      break;

    
  case L7_DHCPS_HEX :


       memset(strData, 0x00, sizeof(strData));
       for(counter=0 ; counter < optionDataLen ; counter++)
       {
          memset(temp, 0x00, sizeof(temp));
          firstChar = (strOptionData[counter] & 0xF0) >> 4;
          lastChar  = strOptionData[counter] & 0x0F;
          sprintf(temp,"%x%x", firstChar, lastChar);
          if ((counter%2) == 1)
          if (counter != (optionDataLen-1))
            strcat(temp,".");
          strcat(strData, temp);
       }

       
      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionHexData, 
                                     strData) != L7_TRUE)
      {
          return L7_FAILURE;
      }

      /* In this scenario, ASCII and IP types of option-data should show null-string */

      strcpy(strOptionData, "");

      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionAsciiData, 
                                         strOptionData) != L7_TRUE)
            {
                return L7_FAILURE;
            }

      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionIpAddressData, 
                                   strOptionData) != L7_TRUE)
      {
          return L7_FAILURE;
      }


      break;

  case L7_DHCPS_IP :

       memset(strData, 0x00, sizeof(strData));
       memset(strIpData, 0x00, sizeof(strIpData));
       memset(strIpAddr, 0x00, sizeof(strIpAddr));
       counter = 0;
       noOfIpAddresses = 0;
       while (counter < optionDataLen)
       {
         ipAddr = 0;
         memcpy(&ipAddr, strOptionData + counter, 4);
         rc = usmDbInetNtoa(ipAddr,  strIpAddr);
         if (rc == L7_SUCCESS)
         {
           strcat(strIpData, strIpAddr);
           strcat(strIpData,",");
           
         }
         counter = counter + 4;
       }

       /* Removing the last "," */
       ipLen = strlen(strIpData) - 1;
       strncpy(strData,strIpData,ipLen);
       

       /* Appending ";" at the end */
       strcat(strData,";");


      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionIpAddressData, 
                                   strData) != L7_TRUE)
      {
          return L7_FAILURE;
      }

      

      /* In this scenario, ASCII and HEX types of option-data should show null-string */

      strcpy(strOptionData, "");

      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionAsciiData, 
                                   strOptionData) != L7_TRUE)
      {
          return L7_FAILURE;
      }

      if(SafeMakeOctetStringFromText(&pOptionEntryData->agentDhcpServerPoolOptionHexData, 
                                   strOptionData) != L7_TRUE)
      {
          return L7_FAILURE;
      }
      
      break;

    default:
      /* unknown value */
      return L7_FAILURE;
     break; 

  }

  if((L7_uint32)status == L7_DHCPS_NOT_READY)
  {
      pOptionEntryData->agentDhcpServerPoolOptionStatus 
                       = D_agentDhcpServerPoolOptionStatus_notReady;
  }

  else if((L7_uint32)status == L7_DHCPS_ACTIVE)
  {
      pOptionEntryData->agentDhcpServerPoolOptionStatus 
                       = D_agentDhcpServerPoolOptionStatus_active;
  }
  
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Get the next valid DHCP pool number
*
* @param    UnitIndex        @b{(input)} Unit for this operation
* @param    poolIndex        @b{(input)} pool number
* @param    poolIndexNext    @b{(output)} next pool number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  intended for the use of SNMP WALK operation
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolOptionEntryGetNext(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                                L7_uint32 optionCode,
                                                L7_uint32 *poolIndexNext,
                                                L7_uint32 *optionCodeNext )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 temp_val;
  L7_uchar8 charOptionCodeNext;
  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  
  bzero(poolName, SNMP_BUFFER_LEN);


  /* get next option in current pool */
  if(snmpDhcpsPoolValidate(UnitIndex, poolIndex) == L7_SUCCESS)
  {
    if(usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
      return L7_FAILURE;

    rc = usmDbDhcpsOptionGetNext(UnitIndex, poolName, (L7_uchar8)optionCode, 
                                 &charOptionCodeNext);
  }
  
  if(L7_FAILURE == rc)
  {
    /* get first option in next pool */
    while(usmDbDhcpsPoolGetNext(UnitIndex, poolIndex, &temp_val) == L7_SUCCESS)
    {
      poolIndex = temp_val;
                
      if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
        return L7_FAILURE;

      if( usmDbDhcpsOptionGetFirst(UnitIndex, poolName, &charOptionCodeNext)== L7_SUCCESS)
      {
        rc = L7_SUCCESS;
        break;
      }
    }
  }

  /* Next valid combination of pool and optionCode is found */
  if(L7_SUCCESS == rc)
  {
    *poolIndexNext = poolIndex;
    *optionCodeNext = (L7_uint32)charOptionCodeNext;
  }
   
  return rc;
}

 

/*********************************************************************
* @purpose  Set the DHCP Option ASCII data
*
* @param    UnitIndex         @b{(input)} Unit for this operation
* @param    poolIndex         @b{(input)} Valid pool Index
* @param    optionCode        @b{(input)} option-code
* @param    optionAsciiData   @b{(input)} value of DHCP option ASCII data
*
*
* @returns  L7_SUCCESS  DHCP option ASCII data is set
* @returns  L7_FAILURE  Failed to set DHCP option ASCII data
*
* @comments
*
* @end
*********************************************************************/


L7_RC_t
snmpAgentDhcpsPoolOptionAsciiDataSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                     L7_uint32 optionCode, L7_uchar8* optionAsciiData)
{
  L7_RC_t rc;
  
  L7_uchar8 strOptionData [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 asciiString [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 dataFormat;
  char status;
  
  L7_uint32 intAsciiLen, intDataLen;

  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  bzero(strOptionData, L7_DHCPS_POOL_OPTION_LENGTH);
  bzero(asciiString, L7_DHCPS_POOL_OPTION_LENGTH);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  /* If get operation for this poolName-optionCode combination is successful, then
     this row-entry of pool-option table is validated */
  if (usmDbDhcpsOptionGet(UnitIndex, poolName, (L7_uchar8)optionCode, strOptionData,
                          &intDataLen, &dataFormat, &status) != L7_SUCCESS)
    return L7_FAILURE;

  if(((L7_uint32)status == L7_DHCPS_NOT_READY) && ((L7_uint32)dataFormat !=  L7_DHCPS_NONE))
        return L7_FAILURE;

  if(((L7_uint32)status == L7_DHCPS_ACTIVE) && ((L7_uint32)dataFormat !=  L7_DHCPS_ASCII))
        return L7_FAILURE;

  /* Get the number of characters in the ascii string */
  intAsciiLen = strlen(optionAsciiData);
  
  /* Concatenate all the strings */
  if((intDataLen + intAsciiLen) <= L7_DHCPS_POOL_OPTION_LENGTH)
  {
    memcpy(&strOptionData[intDataLen], optionAsciiData, intAsciiLen);

    intDataLen += intAsciiLen;
  }
  else 
    return L7_FAILURE;
 
  rc = usmDbDhcpsOptionSet(USMDB_UNIT_CURRENT,poolName, 
                           (L7_uchar8)optionCode,
                           strOptionData,
                           intDataLen,
                           L7_DHCPS_ASCII);

  return rc;
}


/*********************************************************************
* @purpose  Set the DHCP Option HEX data
*
* @param    UnitIndex         @b{(input)} Unit for this operation
* @param    poolIndex         @b{(input)} Valid pool Index
* @param    optionCode        @b{(input)} option-code
* @param    optionHexData   @b{(input)} value of DHCP option Hex data
*
*
* @returns  L7_SUCCESS  DHCP option Hex data is set
* @returns  L7_FAILURE  Failed to set DHCP option Hex data
*
* @comments
*
* @end
*********************************************************************/


L7_RC_t
snmpAgentDhcpsPoolOptionHexDataSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                     L7_uint32 optionCode, L7_uchar8* strHexadecimal)
{
  L7_RC_t rc;
  
  L7_uchar8 strOptionData [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 binaryString [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 dataFormat;
  char status;
  
  L7_uint32 intHexLen, intDataLen;

  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  bzero(strOptionData, L7_DHCPS_POOL_OPTION_LENGTH);
  bzero(binaryString, L7_DHCPS_POOL_OPTION_LENGTH);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  /* If get operation for this poolName-optionCode combination is successful, then
     this row-entry of pool-option table is validated */
  if (usmDbDhcpsOptionGet(UnitIndex, poolName, (L7_uchar8)optionCode, strOptionData,
                          &intDataLen, &dataFormat, &status) != L7_SUCCESS)
    return L7_FAILURE;

  if(((L7_uint32)status == L7_DHCPS_NOT_READY) && ((L7_uint32)dataFormat !=  L7_DHCPS_NONE))
        return L7_FAILURE;

  if(((L7_uint32)status == L7_DHCPS_ACTIVE) && ((L7_uint32)dataFormat !=  L7_DHCPS_HEX))
        return L7_FAILURE;

  /* Convert the input Hex string to binary data */
  if (usmDbHexToBin(strHexadecimal, binaryString, &intHexLen) != L7_SUCCESS)
        return L7_FAILURE;
          

  /* Concatenate all the strings */
  if((intDataLen + intHexLen) <= L7_DHCPS_POOL_OPTION_LENGTH)
  {
    memcpy(&strOptionData[intDataLen], binaryString, intHexLen);

    intDataLen += intHexLen;
  }
  else 
    return L7_FAILURE;
 
  rc = usmDbDhcpsOptionSet(USMDB_UNIT_CURRENT,poolName, 
                           (L7_uchar8)optionCode,
                           strOptionData,
                           intDataLen,
                           L7_DHCPS_HEX);

  return rc;
}

/*********************************************************************
* @purpose  Set the DHCP Option IP data
*
* @param    UnitIndex         @b{(input)} Unit for this operation
* @param    poolIndex         @b{(input)} Valid pool Index
* @param    optionCode        @b{(input)} option-code
* @param    optionIPData   @b{(input)} value of DHCP option Hex data
*
*
* @returns  L7_SUCCESS  DHCP option Hex data is set
* @returns  L7_FAILURE  Failed to set DHCP option IP data
*
* @comments
*
* @end
*********************************************************************/


L7_RC_t
snmpAgentDhcpsPoolOptionIPDataSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                     L7_uint32 optionCode, L7_uchar8* strIPList)
{
  L7_RC_t rc;
  
  L7_uchar8 strOptionData [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_char8 *pbuf = NULL ;
  L7_uchar8 strIp [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 dataFormat;
  char status;
  
  L7_uint32 ipLen = L7_NULL;
  L7_uint32 intDataLen = L7_NULL;
  L7_uint32 ipAddress = L7_NULL;
  L7_uint32 noOfIpAddress = L7_NULL;

  L7_uchar8 poolName[SNMP_BUFFER_LEN];
  bzero(poolName, SNMP_BUFFER_LEN);

  bzero(strOptionData, L7_DHCPS_POOL_OPTION_LENGTH);
  bzero(strIp, L7_DHCPS_POOL_OPTION_LENGTH);

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(UnitIndex, poolIndex, poolName) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

  /* If get operation for this poolName-optionCode combination is successful, then
     this row-entry of pool-option table is validated */
  if (usmDbDhcpsOptionGet(UnitIndex, poolName, (L7_uchar8)optionCode, strOptionData,
                          &intDataLen, &dataFormat, &status) != L7_SUCCESS)
    return L7_FAILURE;

  if(((L7_uint32)status == L7_DHCPS_NOT_READY) && ((L7_uint32)dataFormat !=  L7_DHCPS_NONE))
        return L7_FAILURE;

  if(((L7_uint32)status == L7_DHCPS_ACTIVE) && ((L7_uint32)dataFormat !=  L7_DHCPS_IP))
        return L7_FAILURE;

  
  
  /* Here it is expected that the Option IP  List
     is in 10.1.1.1,157.227.20.1,192.168.36.1;(no spaces in between) format */

  while((pbuf = strchr(strIPList,',')) != NULL)
  {
      ipLen = strlen(strIPList) - strlen(pbuf);
      strncpy(strIp,strIPList,ipLen);

      /* Convert the IP address into an integer */
      if ( usmDbInetAton(strIp, &ipAddress) != L7_SUCCESS )
      {
        return L7_FAILURE;
      }
      noOfIpAddress++;

      /* Concatenate all the binary strings */
      if((intDataLen + 4) <= L7_DHCPS_POOL_OPTION_LENGTH)
      {
        memcpy(&strOptionData[intDataLen], &ipAddress, 4);
        intDataLen += 4;
      }
      
      strcpy(strIPList,pbuf+1);
      
  }

  /* Processing for the last IP address */
  pbuf=strchr(strIPList,';');

  if(pbuf == NULL)  /* token separator not found */
    return L7_FAILURE;

  ipLen = strlen(strIPList) - strlen(pbuf);
  strncpy(strIp,strIPList,ipLen);

  /* Convert the last IP address into an integer */
  if ( usmDbInetAton(strIp, &ipAddress) != L7_SUCCESS )
  {
    return L7_FAILURE;
  }
  noOfIpAddress++;

  /* Maximum no. of IP addresses entered at one time is 8 */
  if (noOfIpAddress > 8) 
  {
    return L7_FAILURE; 
  }

  if((intDataLen + 4) <= L7_DHCPS_POOL_OPTION_LENGTH)
  {
    memcpy(&strOptionData[intDataLen], &ipAddress, 4);
    intDataLen += 4;
  }
  else 
    return L7_FAILURE;

  rc = usmDbDhcpsOptionSet(USMDB_UNIT_CURRENT,poolName, 
                           (L7_uchar8)optionCode,
                           strOptionData,
                           intDataLen,
                           L7_DHCPS_IP);

  return rc;
}

/*********************************************************************
* @purpose  Set the Row Status field
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    optionCode   @b{(input)} valid pool index
* @param    dhcpPoolOptionRowStatus   @b{(input)} Row Status
* @param    poolIndex   @b{(input)} valid pool index
*
* @returns  L7_SUCCESS  row-status is set
* @returns  L7_FAILURE  Failed to set row-status
*
* @comments  Row-status field has been partially implemented.
*            If user sets it todestroy, the pool-entry is deleted.
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsPoolOptionRowStatusSet(L7_uint32 UnitIndex, L7_uint32 poolIndex,
                                     L7_uint32 optionCode,
                                     L7_uint32 dhcpPoolOptionRowStatus)

{
    L7_RC_t rc = L7_SUCCESS;
    L7_uchar8 poolName [SNMP_BUFFER_LEN];
    bzero(poolName, SNMP_BUFFER_LEN);

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(UnitIndex,poolIndex, poolName) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }


    switch (dhcpPoolOptionRowStatus)
    {
    case D_agentDhcpServerPoolOptionStatus_active:

      break;

    case D_agentDhcpServerPoolOptionStatus_destroy:

      rc = usmDbDhcpsOptionDelete(UnitIndex, poolName,
                                  (L7_uchar8)optionCode);
      break;

    default:
      rc = L7_FAILURE;
    }

  return rc;
}


/*********************************************************************
* @purpose  To clear all DHCP Address conflicts
*
* @param    val   @b{(input)} enable/disable
* @param    UnitIndex    @b{(input)} Unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
* @notes
* @notes
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsAddressConflictClearAll(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  
  switch (val)
  {
  case D_agentDhcpServerClearAllAddressConflicts_enable:
    rc = usmDbDhcpsClearAllConflict(USMDB_UNIT_CURRENT);
    break;

  case D_agentDhcpServerClearAllAddressConflicts_disable:
    return L7_SUCCESS;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }
  
 return rc;
}

/*********************************************************************
* @purpose  Get the DHCP Server Conflict Logging Mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    val         @b{(input)} value of Conflict Logging Mode
*
*
* @returns  L7_SUCCESS  Conflict Logging Mode is get
* @returns  L7_FAILURE  Failed to get Conflict Logging Mode
*
* @comments
*
* @end
*********************************************************************/

L7_RC_t
snmpAgentDhcpsConflictLoggingGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 temp_val;


  rc = usmDbDhcpsConflictLoggingGet(USMDB_UNIT_CURRENT,&temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_TRUE :
      *val = D_agentDhcpServerAddressConflictLogging_enable;
      break;

    case L7_FALSE :
      *val = D_agentDhcpServerAddressConflictLogging_disable;
      break;

    default:
      /* unknown value */
      rc = L7_FAILURE;
      break;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the DHCP Server Conflict Logging Mode
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    val         @b{(input)} value of Conflict Logging Mode to be set
*
*
* @returns  L7_SUCCESS  Conflict Logging Mode is set
* @returns  L7_FAILURE  Failed to set Conflict Logging Mode
*
* @comments
*
* @end
*********************************************************************/


L7_RC_t
snmpAgentDhcpsConflictLoggingSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_uint32 temp_val =L7_NULL;

  switch (val)
  {
  case D_agentDhcpServerAddressConflictLogging_enable:
    temp_val = L7_TRUE;
    break;

  case D_agentDhcpServerAddressConflictLogging_disable:
    temp_val = L7_FALSE;
    break;

  default:
    /* unknown value */
    rc = L7_FAILURE;
    break;
  }


  rc = usmDbDhcpsConflictLoggingSet(USMDB_UNIT_CURRENT, temp_val);

  return rc;
}


/*********************************************************************
* @purpose  Get Next logged conflict address
*
* @param    UnitIndex           @b{(input)} Unit for this operation
* @param    conflictIpAddr      @b{(input)} Conflict IP address
* @param    pNextConflictIpAddr           @b{(output)} Next Lease IP address
*
* @returns  L7_SUCCESS  Got the next IP 
*
*
* @end
*********************************************************************/
L7_RC_t
snmpDhcpsServerConflictLoggingGetNext(L7_uint32 UnitIndex, L7_uint32 conflictIpAddr, 
                                      L7_uint32 * pNextConflictIpAddr)
{
  L7_uint32 temp_val;
  L7_RC_t rc = L7_FAILURE;

  if (usmDbDhcpsConflictGetNext(UnitIndex, conflictIpAddr, &temp_val) != L7_FAILURE)
  {
    *pNextConflictIpAddr = temp_val;
    rc = L7_SUCCESS;
  }
  return rc;
}


/*********************************************************************
* @purpose  Get pool-option Entry data 
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    ipAddr      @b{(input)} Conflict IP address 
* @param    pData       @b{(output)} conflict-address snmp data 
*
* @returns  L7_SUCCESS  Got the data 
*
*
* @end
*********************************************************************/
L7_RC_t
snmpAgentDhcpsServerConflictLoggingEntryGet(L7_uint32 UnitIndex, 
  L7_uint32 ipAddr, agentDhcpServerAddressConflictEntry_t * pData)
{
  L7_uint32 detectionType;
  L7_uint32 detectionTime;

  if(L7_NULLPTR == pData)
    return L7_FAILURE;

  if(usmDbDhcpsConflictGet(UnitIndex, ipAddr,
      &detectionTime,
      &detectionType) != L7_SUCCESS)
    return L7_FAILURE;
  
  /* set values */
  pData->agentDhcpServerAddressConflictIP = ipAddr;
  

  if(L7_DHCPS_PING == detectionType) 
    pData->agentDhcpServerAddressConflictDetectionType 
      = D_agentDhcpServerAddressConflictDetectionType_ping;
  /* GratuitousArp is not supported now */
  /*
  else if(L7_DHCPS_DETECTION_TYPE_GRATUITOUSARP == detectionType)
    pData->agentDhcpServerAddressConflictDetectionType 
    = D_agentDhcpServerAddressConflictDetectionType_gratuitousArp; 
  */
  else
    return L7_FAILURE;

  /* As the Detection-time is stored in timeticks, which is 100th of a second */
  pData->agentDhcpServerAddressConflictDetectionTime =  detectionTime * 100; 
  
  return (L7_SUCCESS);
}


/*********************************************************************
* @purpose  Clear an address conflict
*
* @param    UnitIndex   @b{(input)} Unit for this operation
* @param    leaseIpAddr @b{(input)} Conflict IP address to be cleared
*
* @returns  L7_SUCCESS  Cleared the conflict IP address
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
snmpDhcpsServerAddressConflictDelete(L7_uint32 UnitIndex, L7_uint32 conflictIpAddr)
{
  if(usmDbDhcpsClearConflict(UnitIndex, conflictIpAddr) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

#endif
