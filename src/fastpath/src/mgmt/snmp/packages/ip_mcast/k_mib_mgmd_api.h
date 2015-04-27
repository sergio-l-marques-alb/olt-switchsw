/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_igmp_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create
*
* @author   Kumar Manish
*
* @end
*
**********************************************************************/

#ifndef _K_MIB_MGMD_API_H
#define _K_MIB_MGMD_API_H

#include "l3_mcast_commdefs.h"
#include "usmdb_mib_igmp_api.h"
#include "usmdb_igmp_api.h"
#include "l7_mgmd_api.h"
#include "snmpapi.h"
L7_RC_t
snmpMgmdInterfaceRowStatusSet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 mode,L7_uint32 familyType )
{
	L7_RC_t rc = L7_SUCCESS;

	switch (mode)
	{
	case D_mgmdRouterInterfaceStatus_active:
	case D_mgmdRouterInterfaceStatus_createAndGo:
         rc = usmDbMgmdInterfaceStatusSet(UnitIndex, familyType, intIfNum,L7_ENABLE,MGMD_ROUTER_INTERFACE);
	  break;

	case D_mgmdRouterInterfaceStatus_destroy:
         rc = usmDbMgmdInterfaceStatusSet(UnitIndex, familyType, intIfNum, L7_DISABLE,MGMD_ROUTER_INTERFACE);
	  break;

	default:
	rc = L7_FAILURE;
	}

	return rc;
}

L7_RC_t
snmpMgmdInterfaceRowStatusGet( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *mode,L7_uint32 familyType )
{
  
  L7_uint32 temp_val;

  if(usmDbMgmdInterfaceStatusGet(UnitIndex, familyType, intIfNum, &temp_val)==L7_SUCCESS)
  {
    if(temp_val==L7_ENABLE)
    {
     if (usmDbMgmdInterfaceOperationalStateGet(UnitIndex,familyType,intIfNum) == L7_TRUE)
     { 
        *mode = D_mgmdRouterInterfaceStatus_active;
        return L7_SUCCESS;
     }
    }
    *mode = D_mgmdRouterInterfaceStatus_notInService;
    return L7_SUCCESS;
 } 
 return  L7_FAILURE;
 
}


L7_RC_t
snmpMgmdCacheRowStatusGet( L7_uint32 UnitIndex, L7_uint32 ipaddr, L7_uint32 intIfNum, L7_int32 *mode )
{
  L7_RC_t rc;
  L7_inet_addr_t igmpIpAddr;

  inetAddressReset(&igmpIpAddr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipaddr, &igmpIpAddr))
    return L7_FAILURE;

  rc = usmDbMgmdCacheEntryGet(UnitIndex, L7_AF_INET,&igmpIpAddr, intIfNum);
  if(rc == L7_SUCCESS)
      *mode = D_mgmdRouterInterfaceStatus_active;
  else
  {
       *mode = D_mgmdRouterInterfaceStatus_notInService;
       rc = L7_SUCCESS;
  }
  return rc;
}

L7_RC_t
snmpMgmdInterfaceQuerierUpTimeGet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *igmpInterfaceQuerierUpTime,L7_uint32 familyType)
{

  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  if(usmDbMgmdInterfaceQuerierUpTimeGet(UnitIndex, familyType, intIfNum, &temp_val) == L7_SUCCESS)
  {
      *igmpInterfaceQuerierUpTime = temp_val * SNMP_HUNDRED;
      return L7_SUCCESS;
  }
  return rc;
}

L7_RC_t
snmpMgmdInterfaceQuerierExpiryTimeGet (L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32* igmpInterfaceQuerierExpiryTime, L7_uint32 familyType)
{

  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  if(usmDbMgmdInterfaceQuerierExpiryTimeGet(UnitIndex, familyType, intIfNum, &temp_val) == L7_SUCCESS)
  {
      *igmpInterfaceQuerierExpiryTime = temp_val * SNMP_HUNDRED;
      return L7_SUCCESS;
  }
  return rc;
}

L7_RC_t
snmpMgmdCacheCheckCacheTimeValidity(L7_uint32 UnitIndex, L7_inet_addr_t *multipaddr, L7_uint32 intIfNum, L7_uint32 family)
{
  L7_RC_t rc;
  L7_uint32 tmpVal;

  /*CacheUpTime and CacheExpiryTime are not valid only if the 
    source filter mode is include */
  rc = usmDbMgmdCacheGroupCompatModeGet(UnitIndex, family, multipaddr, 
                                        intIfNum, &tmpVal);
  if(rc == L7_SUCCESS && tmpVal ==  L7_MGMD_VERSION_3) 
  {
    rc = usmDbMgmdCacheGroupSourceFilterModeGet(UnitIndex, family, multipaddr,
                                                intIfNum, &tmpVal);
    if(rc == L7_SUCCESS && tmpVal == D_mgmdRouterCacheSourceFilterMode_include)
            rc = L7_FAILURE;
  }
  return rc;
}

L7_RC_t
snmpMgmdCacheEntryGet(L7_uint32 UnitIndex, L7_inet_addr_t *multipaddr, L7_uint32 intIfNum, L7_uint32 nominator, L7_uint32 family)
{
  L7_RC_t rc=L7_FAILURE;
  
  rc = usmDbMgmdCacheEntryGet(UnitIndex, family,multipaddr,intIfNum);
  return rc;
}

L7_RC_t
snmpMgmdCacheEntryNextGet(L7_uint32 UnitIndex, L7_inet_addr_t *multipaddr, L7_uint32 *intIfNum, L7_uint32 nominator, L7_uchar8 family)
{
  L7_RC_t rc=L7_FAILURE;
  rc = usmDbMgmdCacheEntryNextGet(UnitIndex, family, multipaddr,intIfNum);
    return rc;
}

L7_RC_t
snmpMgmdCacheUpTimeGet( L7_uint32 UnitIndex, L7_inet_addr_t *igmpIpAddr, L7_uint32 intIfNum, L7_uint32* igmpCacheUpTime, L7_uint32 family)
{

  L7_uint32 temp_val;
  L7_RC_t rc;
  L7_uint32 compatMode, mode;

  rc = L7_FAILURE;
  
   /* CompatMode == 3 indicates IGMP v3 */
   /* mode == 2 indicates FILTER_MODE_EXCLUDE */
  if ((usmDbMgmdCacheGroupCompatModeGet(UnitIndex, family, igmpIpAddr, intIfNum, &compatMode) == L7_SUCCESS) &&
      ((compatMode !=  L7_MGMD_VERSION_3) ||
       ((compatMode ==  L7_MGMD_VERSION_3) &&
        (usmDbMgmdCacheGroupSourceFilterModeGet(UnitIndex, family, igmpIpAddr, intIfNum, &mode) == L7_SUCCESS) 
        )))
  {
    if(compatMode !=  L7_MGMD_VERSION_3)
      mode = D_mgmdRouterCacheSourceFilterMode_exclude;
     
    if (mode == D_mgmdRouterCacheSourceFilterMode_include)  /*SourceFilterMode - 'include' */
    {
       
      /* This object is not relavent when filter mode is include */
      return L7_FAILURE;
    }
    else if(mode == D_mgmdRouterCacheSourceFilterMode_exclude)
    { 
      
      if(usmDbMgmdCacheUpTimeGet(UnitIndex, family, igmpIpAddr, intIfNum, &temp_val) == L7_SUCCESS)
      {
          *igmpCacheUpTime = temp_val * SNMP_HUNDRED;
          return L7_SUCCESS;
      }
    }
  }
  
  return rc;
}

L7_RC_t
snmpMgmdCacheExpiryTimeGet( L7_uint32 UnitIndex, L7_inet_addr_t *igmpIpAddr, L7_uint32 intIfNum, L7_uint32* igmpCacheExpiryTime, L7_uint32 family)
{

  L7_uint32 temp_val;
  L7_RC_t rc;
  L7_uint32 compatMode, mode;

  rc = L7_FAILURE;
  
  /* CompatMode == 3 indicates IGMP v3 */
  /* mode == 2 indicates FILTER_MODE_EXCLUDE */
  if ((usmDbMgmdCacheGroupCompatModeGet(UnitIndex, family, igmpIpAddr, intIfNum, &compatMode) == L7_SUCCESS) &&
      ((compatMode !=  L7_MGMD_VERSION_3) ||
       ((compatMode ==  L7_MGMD_VERSION_3) &&
        (usmDbMgmdCacheGroupSourceFilterModeGet(UnitIndex, family, igmpIpAddr, intIfNum, &mode) == L7_SUCCESS) 
        )))
   {
     if(compatMode !=  L7_MGMD_VERSION_3)
       mode = D_mgmdRouterCacheSourceFilterMode_exclude;

     if (mode == D_mgmdRouterCacheSourceFilterMode_include)  /*if SourceFilterMode - 'include' */
     {
      /* This object is not relavent when filter mode is include */
       

      return L7_FAILURE;
     }
     else if (mode == D_mgmdRouterCacheSourceFilterMode_exclude)
     {
         

       if(usmDbMgmdCacheExpiryTimeGet(UnitIndex, family, igmpIpAddr, intIfNum, &temp_val) == L7_SUCCESS)
       {
          *igmpCacheExpiryTime = temp_val * SNMP_HUNDRED;
          return L7_SUCCESS;
       }
     }
   }

  return rc;
}

L7_RC_t
snmpMgmdCacheVersion1HostTimerGet(L7_uint32 UnitIndex, L7_inet_addr_t *igmpIpAddr, L7_uint32 intIfNum, L7_uint32* igmpCacheVersion1HostTimer, L7_uint32 family)
{

  L7_uint32 temp_val=0;
  L7_RC_t rc= L7_FAILURE;
  
  if(usmDbMgmdCacheVersion1HostTimerGet(UnitIndex, family, igmpIpAddr, intIfNum, &temp_val) == L7_SUCCESS)
  
  { 
    *igmpCacheVersion1HostTimer = temp_val * SNMP_HUNDRED;
    return L7_SUCCESS;
  }
  
  return rc;
}

L7_RC_t
snmpMgmdCacheVersion2HostTimerGet(L7_uint32 UnitIndex, L7_inet_addr_t *igmpIpAddr, L7_uint32 intIfNum, L7_uint32* igmpCacheVersion2HostTimer, L7_uint32 family)
{

  L7_uint32 temp_val=0;
  L7_RC_t rc = L7_FAILURE;
   
  if(usmDbMgmdCacheVersion2HostTimerGet(UnitIndex, family, igmpIpAddr, intIfNum, &temp_val) == L7_SUCCESS)
        {
          *igmpCacheVersion2HostTimer = temp_val * SNMP_HUNDRED;
          return L7_SUCCESS;
        }
  
  return rc;


}


L7_RC_t
snmpMgmdProxyInterfaceRowStatusGet( L7_uint32 UnitIndex, L7_uint32 intIfNum,L7_uint32 family, L7_int32 *mode )
{
  L7_RC_t rc;
  L7_uint32 temp_val;

  rc = usmDbMgmdHostInterfaceStatusGet(UnitIndex, family, intIfNum, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *mode = D_mgmdHostInterfaceStatus_active;
      break;

    case L7_DISABLE:
      *mode = D_mgmdHostInterfaceStatus_notInService;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpMgmdHostCacheRowStatusGet( L7_uint32 UnitIndex, L7_inet_addr_t *ipaddr, L7_uint32 intIfNum, L7_int32 *mode, L7_uint32 family )
{
  L7_RC_t rc;

  rc = usmDbMgmdHostCacheStatusGet(UnitIndex, family, ipaddr, intIfNum);
  if(rc == L7_SUCCESS)
      *mode = D_mgmdRouterInterfaceStatus_active;
  else
  {
       *mode = D_mgmdRouterInterfaceStatus_notInService;
       rc = L7_SUCCESS;
  }
  return rc;
}

L7_RC_t
snmpMgmdHostInterfaceRowStatusSet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 mode,L7_uint32 family )
{
	L7_RC_t rc = L7_SUCCESS;
	L7_uint32 temp_val = 0;

	switch (mode)
	{
	case D_mgmdRouterInterfaceStatus_active:
	case D_mgmdRouterInterfaceStatus_createAndGo:
	  temp_val = L7_ENABLE;
	  break;

	case D_mgmdRouterInterfaceStatus_notInService:
	case D_mgmdRouterInterfaceStatus_destroy:
	  temp_val = L7_DISABLE;
	  break;

	default:
	rc = L7_FAILURE;
	}

	if (rc == L7_SUCCESS)
	{
      L7_MGMD_INTF_MODE_t type =  MGMD_PROXY_INTERFACE;
      rc = usmDbMgmdInterfaceStatusSet(UnitIndex, family, intIfNum, temp_val,type);
	}

	return rc;
}

#endif
