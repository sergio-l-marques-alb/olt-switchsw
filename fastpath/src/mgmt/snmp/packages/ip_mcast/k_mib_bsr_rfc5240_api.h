/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_pimbsr_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @comments This file is included at the top of the k_mib_pimbsr.c
*
* @create 27/Apr/3010
*
* @author truchi
*
* @end
*
**********************************************************************/

#include "usmdb_common.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"
#include "snmpapi.h"

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
#include "usmdb_ip6_api.h"
#endif

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pim_rfc5060_api.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_pimsm_api.h"
#endif



#ifndef _K_MIB_PIMSM_H
#define _K_MIB_PIMSM_H

L7_RC_t
snmpPimSmpimBsrCandidateRPStatusGet (L7_int32 addrType, L7_inet_addr_t *bsrCandtRPAddr, L7_inet_addr_t *bsrCandtRPGrpAddr, L7_uint32 length, L7_int32 *val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 mode;

  rc = usmDbPimsmbsrCandRPModeGet(addrType, bsrCandtRPAddr, bsrCandtRPGrpAddr, length, &mode);
  if (rc == L7_SUCCESS)
  {
    switch (mode)
    {
    case L7_ENABLE:
      *val = D_pimInterfaceStatus_active;
      break;

    case L7_DISABLE:
      *val = D_pimInterfaceStatus_notInService;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpPimSmpimBsrCandidateRPStatusSet (L7_int32 addrType, L7_inet_addr_t bsrCandtRPAddr, L7_inet_addr_t bsrCandtRPGrpAddr, L7_uint32 length, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIface = 0;
  L7_inet_addr_t bsrCandtRPGrpMask;

  inetAddressZeroSet(L7_AF_INET6,&bsrCandtRPGrpMask);

  if ( (val == D_pimBsrCandidateRPStatus_createAndGo) ||  (val == D_pimBsrCandidateRPStatus_destroy))
  {
     if (D_pimBsrCandidateRPAddressType_ipv4 == addrType)
     {
       if (bsrCandtRPAddr.addr.ipv4.s_addr != 0)
       {
         if (usmDbIpRouterIfResolve(bsrCandtRPAddr.addr.ipv4.s_addr, &intIface) != L7_SUCCESS)
           return L7_FAILURE;
       }
       else
       {
         return L7_FAILURE;
       }
     }
     else if (D_pimBsrCandidateRPAddressType_ipv6 == addrType)
     {
#ifdef L7_IPV6_PACKAGE
         if (usmDbIp6MapRtrAddressSpecificIntfGet(&(bsrCandtRPAddr.addr.ipv6), &intIface) != L7_SUCCESS)
            return L7_FAILURE;
#else
    return L7_FAILURE;
#endif
     }
     else
     {
       return L7_FAILURE;
     }
     if (inetMaskLenToMask(addrType, length, &bsrCandtRPGrpMask) != L7_SUCCESS)
     {
       return L7_FAILURE;
     }
  }
  else
  {
    return L7_FAILURE;
  }

  switch (val)
  {
  case D_pimBsrCandidateRPStatus_active:
  case D_pimBsrCandidateRPStatus_createAndGo:

       rc = usmDbPimsmCandRPSet(addrType, intIface,
                                    &bsrCandtRPGrpAddr, &bsrCandtRPGrpMask, L7_ENABLE);
       break;

  case D_pimBsrCandidateRPStatus_destroy:
       rc = usmDbPimsmCandRPSet(addrType, intIface,
                                    &bsrCandtRPGrpAddr, &bsrCandtRPGrpMask, L7_DISABLE);
       break;


  default:
      rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpPimSmpimBsrCandidateBSRStatusGet(L7_int32 addrType, L7_int32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 mode;

  rc = usmDbPimsmCandBSRModeGet(addrType, &mode);
  if (rc == L7_SUCCESS)
  {
    switch (mode)
    {
    case L7_ENABLE:
      *val = D_pimBsrCandidateBSRStatus_active;
      break;

    case L7_DISABLE:
      *val = D_pimBsrCandidateBSRStatus_notInService;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpPimSmpimBsrCandidateBSRSet(L7_int32 addrType, L7_int32 val, L7_inet_addr_t bsrCandtBSRAddr,
                                  L7_uint32 priority, L7_uint32 maskLength )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIface = 0;
  if (D_pimStaticRPAddressType_ipv4 == addrType)
  {
     if (bsrCandtBSRAddr.addr.ipv4.s_addr != 0)
     {
        if ( usmDbIpRouterIfResolve(bsrCandtBSRAddr.addr.ipv4.s_addr, &intIface) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
     }
     else
     {
       return L7_FAILURE;
     }
  }
  else if (D_pimStaticRPAddressType_ipv6 == addrType)
  {
#ifdef L7_IPV6_PACKAGE
    if (usmDbIp6MapRtrAddressSpecificIntfGet(&bsrCandtBSRAddr.addr.ipv6, &intIface) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
#else
    return L7_FAILURE;
#endif
  }
  else
  {
    return L7_FAILURE;
  }
  switch (val)
  {
  case D_pimBsrCandidateBSRStatus_active:
  case D_pimBsrCandidateBSRStatus_createAndGo:
       rc = usmDbPimsmCandBSRSet(USMDB_UNIT_CURRENT, addrType, intIface, priority, L7_ENABLE, maskLength, L7_NULL);
       break;

  case D_pimBsrCandidateBSRStatus_destroy:
       rc = usmDbPimsmCandBSRSet(USMDB_UNIT_CURRENT, addrType, intIface, L7_PIMSM_CBSR_PRIORITY, L7_DISABLE
                                 , L7_PIMSM_CBSR_HASH_MASK_LENGTH, L7_NULL);

       break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t snmpBsrRPSetGetNext(L7_uint32 UnitIndex,
                            L7_uchar8 family,
                            L7_uchar8 *origin,
                            L7_inet_addr_t *groupAddress,
                            L7_inet_addr_t *groupMask,
                            L7_inet_addr_t *rpAddress)
{
   
  L7_RC_t rc=L7_FAILURE;
  rc = usmDbPimsmRpGrpMappingEntryNextGet(USMDB_UNIT_CURRENT,
                                            family,
                                            origin,
                                            groupAddress,
                                            groupMask,
                                            rpAddress) ;
  while (rc==L7_SUCCESS)
  {
     if (*origin == D_pimGroupMappingOrigin_bsr - 1)
     {
        break;
     }

     rc = usmDbPimsmRpGrpMappingEntryNextGet(USMDB_UNIT_CURRENT,
                                            family,
                                            origin,
                                            groupAddress,
                                            groupMask,
                                            rpAddress) ;
  }
  return rc;

}



#endif
