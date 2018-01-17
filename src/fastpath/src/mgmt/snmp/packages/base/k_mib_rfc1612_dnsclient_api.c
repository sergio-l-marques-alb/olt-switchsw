/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   k_mib_rfc1612_dnsclient_api.c
*
* @purpose    Wrapper functions for Fastpath DNS Client (RFC 1612) MIB
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
#include "k_mib_rfc1612_dnsclient_api.h"
#include "usmdb_dns_client_api.h"
#include "usmdb_common.h"
#include "usmdb_registry_api.h"
#include "usmdb_util_api.h"
#include "usmdb_unitmgr_api.h"
#include <ctype.h>

/*****************************************************************************/

L7_RC_t
snmpdnsResConfigResetGet(L7_int32 *dnsResConfigReset)
{
  L7_RC_t rc;
  L7_BOOL enable;

  rc = usmDbDNSClientAdminModeGet(&enable);

  if( rc == L7_SUCCESS)
  {
    switch (enable)
    {
    case L7_TRUE:
      *dnsResConfigReset = D_dnsResConfigReset_running;
      break;
    case L7_FALSE:
    default:
      *dnsResConfigReset = D_dnsResConfigReset_other;
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpdnsResConfigImplementIdentGet(L7_char8 *buf)
{
  L7_RC_t rc = L7_FAILURE;
  usmDbCodeVersion_t ver;
  L7_uint32 unit;

  if (usmDbComponentPresentCheck(USMDB_UNIT_CURRENT, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
  {
    rc =  usmDbUnitMgrMgrNumberGet(&unit);
    if ( (rc == L7_SUCCESS) &&(rc = usmDbUnitMgrDetectCodeVerRunningGet(unit, &ver)) == L7_SUCCESS)
    {
      if (isalpha(ver.rel))
         sprintf(buf, "%c.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
      else
         sprintf(buf, "%d.%d.%d.%d", ver.rel, ver.ver, ver.maint_level, ver.build_num);
    }
  }
  else
     rc = usmDbSwVersionGet(USMDB_UNIT_CURRENT, buf);

   return rc;
}

L7_RC_t
snmpDNSClientCacheRRNextGet(L7_uchar8    *rrName,
                            dnsRRType_t  *rrType,
                            L7_uint32    *rrIndex,
                            dnsRRClass_t *rrClass,
                            L7_uchar8    *qname)
{
  L7_RC_t rc = L7_FAILURE;

  while((rc= usmDbDNSClientCacheRRNextGet (rrName, rrType,
         rrIndex ,rrClass, qname))== L7_SUCCESS)
  {
     if (*rrType == DNS_RR_TYPE_ADDRESS || *rrType == DNS_RR_TYPE_CNAME)
     {
       break;
     }
  }
  return rc; 
}

