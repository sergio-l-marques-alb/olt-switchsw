/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_mcast_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @create
*
* @author   Nitish Kumar
*
* @end
*
**********************************************************************/

#ifndef _K_MIB_MCAST_H
#define _K_MIB_MCAST_H

#include "usmdb_mib_mcast_api.h"


L7_RC_t
snmpMcastRoutingStatusSet ( L7_uint32 UnitIndex, L7_int32 mode )
{
	L7_RC_t rc = L7_SUCCESS;
	L7_uint32 temp_val = 0;

	switch (mode)
	{
	case D_ipMRouteEnable_enabled:
        temp_val = L7_ENABLE;
        break;
	case D_ipMRouteEnable_disabled:
        temp_val = L7_DISABLE;
	  break;

	default:
	rc = L7_FAILURE;
	}

	if (rc == L7_SUCCESS)
	{
		rc = usmDbMcastAdminModeSet(UnitIndex, temp_val);
	}
    
    return rc;
}


L7_RC_t
snmpMcastRoutingStatusGet ( L7_uint32 UnitIndex, L7_int32 *mode )
{
	L7_RC_t rc ;
    L7_uint32 temp_val;

    rc = usmDbMcastAdminModeGet(UnitIndex, &temp_val);

    if(rc == L7_SUCCESS)
    {
        switch(temp_val)
        {
        case L7_ENABLE:
            *mode = D_ipMRouteEnable_enabled;
            break;
        
        case L7_DISABLE:
            *mode = D_ipMRouteEnable_disabled;
            break;
        
        default:
            rc = L7_FAILURE;
        }
    }

    return rc;
}


L7_RC_t
snmpIpMRouteExpiryGet(L7_uint32 UnitIndex, L7_uint32 ipMRtGrp, L7_uint32 ipMRtSrc,
                                  L7_uint32 ipMRtSrcMask, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_inet_addr_t inetMRtGrp, inetMRtSrc, inetSrcMask;

  inetAddressReset(&inetMRtGrp);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRtGrp, &inetMRtGrp))
    return L7_FAILURE;
  inetAddressReset(&inetMRtSrc);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRtSrc, &inetMRtSrc))
    return L7_FAILURE;
  inetAddressReset(&inetSrcMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRtSrcMask, &inetSrcMask))
    return L7_FAILURE;

  rc = usmDbMcastIpMRouteExpiryGet(L7_AF_INET, UnitIndex, &inetMRtGrp, &inetMRtSrc, &inetSrcMask, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}

L7_RC_t
snmpIpMRouteUpTimeGet(L7_uint32 UnitIndex, L7_uint32 ipMRtGrp, L7_uint32 ipMRtSrc,
                                L7_uint32 ipMRtSrcMask, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_inet_addr_t inetMRtGrp, inetMRtSrc, inetSrcMask;

  inetAddressReset(&inetMRtGrp);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRtGrp, &inetMRtGrp))
    return L7_FAILURE;
  inetAddressReset(&inetMRtSrc);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRtSrc, &inetMRtSrc))
    return L7_FAILURE;
  inetAddressReset(&inetSrcMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRtSrcMask, &inetSrcMask))
    return L7_FAILURE;

  rc = usmDbMcastIpMRouteUpTimeGet(L7_AF_INET, UnitIndex, &inetMRtGrp, &inetMRtSrc, &inetSrcMask, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}

L7_RC_t
snmpIpMRouteProtocolGet(L7_uint32 UnitIndex, L7_uint32 ipMRtGrp, L7_uint32 ipMRtSrc,
                                L7_uint32 ipMRtSrcMask, L7_uint32 *val)
{
  L7_uint32 protocol;
  L7_inet_addr_t inetMRtGrp, inetMRtSrc, inetSrcMask;
  inetAddressReset(&inetMRtGrp);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRtGrp, &inetMRtGrp))
    return L7_FAILURE;
  inetAddressReset(&inetMRtSrc);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRtSrc, &inetMRtSrc))
    return L7_FAILURE;
  inetAddressReset(&inetSrcMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMRtSrcMask, &inetSrcMask))
    return L7_FAILURE;

  if(usmDbMcastIpMRouteProtocolGet(L7_AF_INET, UnitIndex, &inetMRtGrp, &inetMRtSrc, &inetSrcMask, &protocol) == L7_SUCCESS)
  {
	  switch (protocol)
	  {

                  case L7_MCAST_IANA_MROUTE_UNASSIGNED:
                          *val = D_ipMRouteProtocol_other;
                          break;

                  case L7_MCAST_IANA_MROUTE_OTHER:
			  *val = D_ipMRouteProtocol_other;
			  break;

		  case L7_MCAST_IANA_MROUTE_LOCAL:
			  *val = D_ipMRouteProtocol_local;
			  break;

		  case L7_MCAST_IANA_MROUTE_NETMGMT:
			  *val = D_ipMRouteProtocol_netmgmt;
			  break;

		  case L7_MCAST_IANA_MROUTE_DVMRP:
			  *val = D_ipMRouteProtocol_dvmrp;
			  break;

		  case L7_MCAST_IANA_MROUTE_MOSPF:
			  *val = D_ipMRouteProtocol_mospf;
			  break;

		  case L7_MCAST_IANA_MROUTE_PIM_SM_DM:
			  *val = D_ipMRouteProtocol_pimSparseDense;
			  break;

		  case L7_MCAST_IANA_MROUTE_CBT:
			  *val = D_ipMRouteProtocol_cbt;
			  break;

		  case L7_MCAST_IANA_MROUTE_PIM_SM:
			  *val = D_ipMRouteProtocol_pimSparseMode;
			  break;

		  case L7_MCAST_IANA_MROUTE_PIM_DM:
			  *val = D_ipMRouteProtocol_pimDenseMode;
			  break;

		  case L7_MCAST_IANA_MROUTE_IGMP:
			  *val = D_ipMRouteProtocol_igmpOnly;
			  break;

		  case L7_MCAST_IANA_MROUTE_BMGP:
			  *val = D_ipMRouteProtocol_bgmp;
			  break;

		  case L7_MCAST_IANA_MROUTE_MSDP:
			  *val = D_ipMRouteProtocol_msdp;
			  break;

		 default :
		  return L7_FAILURE;
	  }

	  return L7_SUCCESS;
   }
   return L7_FAILURE;
}


L7_RC_t
snmpIpMRouteInterfaceProtocolGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val)
{
  L7_uint32 protocol;
  if(usmDbMcastIpMRouteInterfaceProtocolGet(UnitIndex, intIfNum, &protocol) == L7_SUCCESS)
  {       
	  switch (protocol)
	  {      
                  case L7_MCAST_IANA_MROUTE_UNASSIGNED:
                          *val = D_ipMRouteInterfaceProtocol_other;
                          break;
		  case L7_MCAST_IANA_MROUTE_OTHER:
			  *val = D_ipMRouteInterfaceProtocol_other;
			  break;

		  case L7_MCAST_IANA_MROUTE_LOCAL:
			  *val = D_ipMRouteInterfaceProtocol_local;
			  break;

		  case L7_MCAST_IANA_MROUTE_NETMGMT:
			  *val = D_ipMRouteInterfaceProtocol_netmgmt;
			  break;

		  case L7_MCAST_IANA_MROUTE_DVMRP:
			  *val = D_ipMRouteInterfaceProtocol_dvmrp;
			  break;

		  case L7_MCAST_IANA_MROUTE_MOSPF:
			  *val = D_ipMRouteInterfaceProtocol_mospf;
			  break;

		  case L7_MCAST_IANA_MROUTE_PIM_SM_DM:
			  *val = D_ipMRouteInterfaceProtocol_pimSparseDense;
			  break;

		  case L7_MCAST_IANA_MROUTE_CBT:
			  *val = D_ipMRouteInterfaceProtocol_cbt;
			  break;

		  case L7_MCAST_IANA_MROUTE_PIM_SM:
			  *val = D_ipMRouteInterfaceProtocol_pimSparseMode;
			  break;

		  case L7_MCAST_IANA_MROUTE_PIM_DM:
			  *val = D_ipMRouteInterfaceProtocol_pimDenseMode;
			  break;

		  case L7_MCAST_IANA_MROUTE_IGMP:
			  *val = D_ipMRouteInterfaceProtocol_igmpOnly;
			  break;

		  case L7_MCAST_IANA_MROUTE_BMGP:
			  *val = D_ipMRouteInterfaceProtocol_bgmp;
			  break;

		  case L7_MCAST_IANA_MROUTE_MSDP:
			  *val = D_ipMRouteInterfaceProtocol_msdp;
			  break;

		 default :
		  return L7_FAILURE;
	  }

	  return L7_SUCCESS;
   }

   return L7_FAILURE;
}

L7_RC_t
snmpMcastMrouteBoundaryStatusSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 ipAddr, L7_uint32 ipMask, L7_uint32 val)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_inet_addr_t inetMRtSrc, inetSrcMask;

  inetAddressReset(&inetMRtSrc);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipAddr, &inetMRtSrc))
    return L7_FAILURE;
  inetAddressReset(&inetSrcMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipMask, &inetSrcMask))
    return L7_FAILURE;

    switch (val)
    {
        case D_ipMRouteBoundaryStatus_active:
        case D_ipMRouteBoundaryStatus_createAndGo:
			 rc = usmDbMcastMrouteBoundaryAdd(UnitIndex, intIfNum, &inetMRtSrc, &inetSrcMask);
             break;

        case D_ipMRouteBoundaryStatus_destroy:
             rc = usmDbMcastMrouteBoundaryDelete(UnitIndex, intIfNum, &inetMRtSrc, &inetSrcMask);
	        break;
        default:
             rc = L7_FAILURE;
    }

  return rc;
}
#endif
