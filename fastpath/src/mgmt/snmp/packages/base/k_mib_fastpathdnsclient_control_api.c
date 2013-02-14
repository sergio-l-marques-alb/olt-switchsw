/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_fastpathdnsclient_control_api.c
*
* @purpose    Wrapper functions for Fastpath DNS Client Control MIB
*
* @component  SNMP
*
* @comments
*
* @create     3/29/2005
*
* @author     I. Kiran
* @end
*
**********************************************************************/

#include "k_private_base.h"
#include "k_mib_fastpathdnsclient_control_api.h"
#include "usmdb_dns_client_api.h"

/*****************************************************************************/
L7_RC_t
snmpDnsResCtlglobalAdminModeGet(L7_int32 *adminmode)
{
  L7_BOOL enable;
  L7_RC_t rc;

  if ((rc = usmDbDNSClientAdminModeGet(&enable)) == L7_SUCCESS)
  {
    switch (enable)
    {
    case L7_TRUE:
      *adminmode = D_agentResCtlAdminMode_true;
      break;
    case L7_FALSE:
    default:
      *adminmode = D_agentResCtlAdminMode_false;
      break;
    }
  }

  return rc;

}

L7_RC_t
snmpDnsResCtlglobalAdminModeSet(L7_int32 adminmode) 
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL temp_val = L7_FALSE;
  
  switch (adminmode)
  {
    case D_agentResCtlAdminMode_true:
      temp_val = L7_TRUE;
      break;
    case D_agentResCtlAdminMode_false:
      temp_val = L7_FALSE;
      break;
    default:
      rc = L7_FAILURE;
      break;
  }
 
  if(rc == L7_SUCCESS)
  {
    rc = usmDbDNSClientAdminModeSet(temp_val);
  }

  return rc;
}
L7_RC_t
snmpDNSClientStaticHostEntryNextGet( L7_char8  *hostname,
                                     L7_inet_addr_t *inetAddr)
{
 L7_RC_t rc = L7_FAILURE;

 while ((rc =usmDbDNSClientStaticHostEntryNextGet(hostname, inetAddr))== L7_SUCCESS) 
 {
    if (inetAddr->family == L7_AF_INET)  
       break;
 }

 return rc;
}

/******************************************************************************/

