/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* Name: k_mib_tacacsclient_api.c
*
* Purpose: System-specific code for TACACS Private MIB
*
* Created by: nramesh 08/19/2005
*
* Component: SNMP
* *********************************************************************/
#include "k_mib_tacacsclient_api.h"
#include "usmdb_tacacs_api.h"
#include "osapi_support.h"
#include "prnt_lib.h" /* debug */

/********************************************************************
  Function: snmpTacacsServerGet
  Purpose:  Verifies whether the given TACACS server is configured
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
  Comment:
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsServerGet(OctetString *agentTacacsServerIpAddress, L7_uint32 *type, L7_char8 *get_ipAddress)
{
  L7_uchar8 ipAddress[255];
  L7_RC_t rc;
  OctetString *curr_address=NULL;
  L7_uint32 temp_uint;
  
  memset(ipAddress, 0, strlen(ipAddress));
  rc = usmDbTacacsHostNameServerFirstGet(type, ipAddress);

  while(rc == L7_SUCCESS)
  {
    /* only process ipv4 and dns entries */
    if ((*type == L7_IP_ADDRESS_TYPE_IPV4) || (*type == L7_IP_ADDRESS_TYPE_DNS))
    {
      /* convert address to OctetString*/
      if (*type == L7_IP_ADDRESS_TYPE_IPV4) {
        /* map dotted ip address into inetaddress string */
        temp_uint = osapiInet_addr(ipAddress);
        FreeOctetString(curr_address);
        curr_address = MakeOctetString((L7_uchar8*)&temp_uint, sizeof(temp_uint));
      }
      else if (*type == L7_IP_ADDRESS_TYPE_DNS) {
        /* dns string is already in the right format */
        FreeOctetString(curr_address);
        curr_address = MakeOctetString(ipAddress, strlen(ipAddress));
      }

      /* if equal to the requested address */
      if((agentTacacsServerIpAddress->length == curr_address->length) &&
         (CmpOctetStrings(agentTacacsServerIpAddress, curr_address) == 0))
      {
        FreeOctetString(curr_address);
        strcpy(get_ipAddress, ipAddress);
        return L7_SUCCESS;
      }
    }
    rc = usmDbTacacsHostNameServerNextGet(ipAddress, type, ipAddress);
  }
  FreeOctetString(curr_address);
  return L7_FAILURE;
}

/********************************************************************
  Function: snmpTacacsServerNextGet
  Purpose:  Retrieves the next TACACS server configured
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsServerIpAddressNext - Next TACACS Server IP Address
  Comment:
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsServerNextGet(OctetString *agentTacacsServerIpAddress,
                        OctetString **agentTacacsServerIpAddressNext, L7_uint32 *next_type, L7_char8 *next_ipAddress)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 ipAddress[255];
  OctetString *prev_address = NULL;
  L7_uint32 prev_type = 0;
  L7_uchar8 prev_ipAddress[255];
  OctetString *curr_address = NULL;
  L7_uint32 curr_type = 0;
  L7_uint32 temp_uint;

  rc = usmDbTacacsHostNameServerFirstGet(&curr_type,ipAddress);

  if (rc != L7_SUCCESS)
  {
    return rc;
  }
  rc = L7_FAILURE;
  do
  {
    /* only process ipv4 and dns entries */
    if ((curr_type == L7_IP_ADDRESS_TYPE_IPV4) || (curr_type == L7_IP_ADDRESS_TYPE_DNS)) {
      /* convert address to OctetString*/
      if (curr_type == L7_IP_ADDRESS_TYPE_IPV4) {
        /* map dotted ip address into inetaddress string */
        temp_uint = osapiInet_addr(ipAddress);
        FreeOctetString(curr_address);
        curr_address = MakeOctetString((L7_uchar8*)&temp_uint, sizeof(temp_uint));
      }
      else if (curr_type == L7_IP_ADDRESS_TYPE_DNS) {
        /* dns string is already in the right format */
        FreeOctetString(curr_address);
        curr_address = MakeOctetString(ipAddress, strlen(ipAddress));
      }

      /* if equal to the requested address */
      if(curr_address->length == agentTacacsServerIpAddress->length &&
         CmpOctetStrings(curr_address, agentTacacsServerIpAddress) == 0)
      {
        FreeOctetString(prev_address);
        prev_address = curr_address;
        prev_type = curr_type;
        strcpy(prev_ipAddress, ipAddress);
        curr_address = NULL;
        rc = L7_SUCCESS;
        break;
      }
      /* if greater than requested address, index will be greater */
      else if(curr_address->length > agentTacacsServerIpAddress->length ||
              (curr_address->length == agentTacacsServerIpAddress->length &&
               CmpOctetStrings(curr_address, agentTacacsServerIpAddress) > 0))
      {
        /* if we havent found one greater */
        if(prev_address == NULL)
        {
          prev_address = curr_address;
          prev_type = curr_type;
          strcpy(prev_ipAddress, ipAddress);
          curr_address = NULL;
          rc = L7_SUCCESS;
        }
        /* if new found address is less than last found address*/
        else if (curr_address->length < prev_address->length ||
                 (curr_address->length == prev_address->length &&
                  CmpOctetStrings(curr_address, prev_address) < 0))
        {
          FreeOctetString(prev_address);
          prev_address = curr_address;
          prev_type = curr_type;
          strcpy(prev_ipAddress, ipAddress);
          curr_address = NULL;
          rc = L7_SUCCESS;
        }
      }
    }
  } while (usmDbTacacsHostNameServerNextGet(ipAddress, &curr_type, ipAddress) == L7_SUCCESS);

  FreeOctetString(curr_address);
  
  if (rc == L7_SUCCESS)
  {
    FreeOctetString(*agentTacacsServerIpAddressNext);
    *agentTacacsServerIpAddressNext = prev_address;
    *next_type = prev_type;
    strcpy(next_ipAddress, prev_ipAddress);
  }
  else
  {
    FreeOctetString(prev_address);
  }
  return rc;
}

/********************************************************************
  Function: snmpTacacsPortNumberGet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsPortNumber - TACACS Server Port Number
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsPortNumberGet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type, 
                        L7_uint32 *agentTacacsPortNumber)
{
  L7_ushort16 port;
  if(usmDbTacacsHostNamePortNumberGet(type,agentTacacsServerIpAddress, &port) == L7_SUCCESS)
  {
    *agentTacacsPortNumber = port;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/********************************************************************
  Function: snmpTacacsPortNumberSet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsPortNumber - TACACS Server Port Number
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsPortNumberSet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                        L7_uint32 agentTacacsPortNumber)
{
  return usmDbTacacsHostNamePortNumberSet(type,agentTacacsServerIpAddress, agentTacacsPortNumber);
}
/********************************************************************
  Function: snmpTacacsTimeOutGet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsTimeOut - Timeout
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsTimeOutGet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                     L7_uchar8 *agentTacacsTimeOut)
{
  L7_BOOL useGlobal;
  if(usmDbTacacsHostNameServerUseGlobalTimeoutGet(type,agentTacacsServerIpAddress, &useGlobal) == L7_SUCCESS)
  {
    if(useGlobal)
    {
      return usmDbTacacsGblTimeOutGet(agentTacacsTimeOut);
    }
    else
    {
      return usmDbTacacsHostNameTimeOutGet(type,agentTacacsServerIpAddress, agentTacacsTimeOut);
    }
  }
  return L7_FAILURE;
}

/********************************************************************
  Function: snmpTacacsTimeOutSet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsTimeOut - Timeout
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsTimeOutSet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                     L7_uchar8 agentTacacsTimeOut)
{
  return usmDbTacacsHostNameTimeOutSet(type,agentTacacsServerIpAddress, agentTacacsTimeOut);
}

/********************************************************************
  Function: snmpTacacsKeyGet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsKey - TACACS server key
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsKeyGet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                 L7_char8 *agentTacacsKey)
{
  L7_BOOL useGlobal;
  if(usmDbTacacsHostNameServerUseGlobalKeyGet(type,agentTacacsServerIpAddress, &useGlobal) == L7_SUCCESS)
  {
    if(useGlobal)
    {
      return usmDbTacacsGblKeyGet(agentTacacsKey);
    }
    else
    {
        return usmDbTacacsHostNameKeyGet(type, agentTacacsServerIpAddress, agentTacacsKey);
    }
  }
  return L7_FAILURE;
}

/********************************************************************   
  Function: snmpTacacsKeySet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsKey - TACACS server key
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsKeySet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                 L7_char8 *agentTacacsKey)
{
  return usmDbTacacsHostNameKeySet(type,agentTacacsServerIpAddress, agentTacacsKey);
}
/********************************************************************
  Function: snmpTacacsPriorityGet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsPriority - TACACS Server priority
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsPriorityGet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                      L7_uint32 *agentTacacsPriority)
{
  L7_ushort16 priority;
  if(usmDbTacacsHostNamePriorityGet(type, agentTacacsServerIpAddress, &priority) == L7_SUCCESS)
  {
    *agentTacacsPriority = priority;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/********************************************************************
  Function: snmpTacacsPrioritySet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsPriority - TACACS Server priority
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsPrioritySet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                      L7_uint32 agentTacacsPriority)
{
  return usmDbTacacsHostNamePrioritySet(type,agentTacacsServerIpAddress, agentTacacsPriority);
}

/********************************************************************
  Function: snmpTacacsRowStatusSet
  Purpose:  Support the TACACS functionality.
  Arguments: agentTacacsServerIpAddress - TACACS Server IP Address
             agentTacacsServerStatus - TACACS Server row status
  Comment:     Support for the MIB object - Supported.
  Change Report:
*********************************************************************/
L7_RC_t
snmpTacacsRowStatusSet(L7_uchar8 *agentTacacsServerIpAddress,L7_uint32 type,
                       L7_uint32 agentTacacsServerStatus)
{
  if(agentTacacsServerStatus == D_agentTacacsServerStatus_createAndGo) 
  {
    if(usmDbTacacsHostNameServerAdd(type,agentTacacsServerIpAddress) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else if(agentTacacsServerStatus == D_agentSntpClientUcastServerRowStatus_destroy)
  {
    if(usmDbTacacsHostNameServerRemove(type,agentTacacsServerIpAddress) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else if(agentTacacsServerStatus != D_agentSntpClientUcastServerRowStatus_active)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
