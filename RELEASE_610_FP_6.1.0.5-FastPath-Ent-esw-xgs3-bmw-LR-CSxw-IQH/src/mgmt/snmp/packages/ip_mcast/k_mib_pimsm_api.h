/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_pimsm_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @comments This file is included at the top of the k_mib_pimsm.c
*
* @create 05/05/2006
*
* @author Dasoju Shashidhar
*
* @end
*
**********************************************************************/

#include "usmdb_common.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"

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
snmpPimSmInterfaceStatusGet (L7_uint32 pimSmInterfaceIfIndex, L7_int32 pimInetIPVersion, L7_int32 *val )
{
  
  L7_uint32 mode;

  if(usmDbPimsmInterfaceModeGet(USMDB_UNIT_CURRENT,pimInetIPVersion ,pimSmInterfaceIfIndex, &mode)==L7_SUCCESS)
  {
    if(mode==L7_ENABLE)
    {
     if(usmDbPimsmInterfaceOperationalStateGet(USMDB_UNIT_CURRENT, pimInetIPVersion,pimSmInterfaceIfIndex) == L7_TRUE)
     {
       *val = D_pimSmInterfaceStatus_active;
       return L7_SUCCESS;
     }
    }
    *val = D_pimSmInterfaceStatus_notInService;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
  
}

  


L7_RC_t
snmpPimSmInterfaceStatusSet (L7_uint32 pimSmInterfaceIfIndex,L7_int32 pimInetIPVersion, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_pimSmInterfaceStatus_active:
  case D_pimSmInterfaceStatus_createAndGo:
       rc = usmDbPimsmInterfaceModeSet(USMDB_UNIT_CURRENT,pimInetIPVersion,pimSmInterfaceIfIndex, L7_ENABLE);
       break;

  case D_pimSmInterfaceStatus_destroy:
       rc = usmDbPimsmInterfaceModeSet(USMDB_UNIT_CURRENT, pimInetIPVersion,pimSmInterfaceIfIndex, L7_DISABLE);
       break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpPimSmSsmRangeStatusGet (L7_int32 addrType, L7_inet_addr_t *pimSsmRangeAddr, L7_uint32 length, L7_int32 *val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 mode;

  rc = usmDbPimsmSsmRangeModeGet(addrType, pimSsmRangeAddr, length, &mode);
  if (rc == L7_SUCCESS)
  {
    switch (mode)
    {
    case L7_ENABLE:
      *val = D_pimSmInterfaceStatus_active;
      break;
      
    case L7_DISABLE:
      *val = D_pimSmInterfaceStatus_notInService;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpPimSmSsmRangeStatusSet (L7_int32 addrType, L7_inet_addr_t *pimSsmRangeAddr, L7_uint32 length, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_pimSmInterfaceStatus_active:
  case D_pimSmInterfaceStatus_createAndGo:

       rc = usmDbPimsmSsmRangeModeSet(addrType, pimSsmRangeAddr,length, L7_ENABLE);
       break;

  case D_pimSmInterfaceStatus_destroy:

       rc = usmDbPimsmSsmRangeModeSet(addrType, pimSsmRangeAddr,length, L7_DISABLE);
       break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpPimSmStaticRPStatusGet (L7_int32 addrType, L7_inet_addr_t *pimStaticRPGrpAddr, L7_uint32 length, L7_int32 *val )
{
  L7_RC_t rc = L7_FAILURE;

  rc = usmDbPimsmStaticRPEntryGet(addrType,pimStaticRPGrpAddr,length);

  if (rc == L7_SUCCESS)
     *val = D_pimSmStaticRPRowStatus_active;
  else
     *val = D_pimSmStaticRPRowStatus_notInService;

  return L7_SUCCESS;
}

L7_RC_t
snmpPimSmStaticRPStatusSet (L7_int32 addrType, L7_inet_addr_t pimSmStaticRPRPAddr, L7_inet_addr_t pimStaticRPGrpAddr,
                            L7_uint32 length, L7_int32 overRide, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL conflict = L7_FALSE;

  if (overRide == D_pimSmStaticRPOverrideDynamic_true)
  {
   conflict = L7_TRUE;
  }
  switch (val)
  {
  case D_pimSmStaticRPRowStatus_active:
       break;
  case D_pimSmStaticRPRowStatus_createAndGo:
       rc = usmDbPimsmStaticRPSet(addrType, &pimSmStaticRPRPAddr,
                                  &pimStaticRPGrpAddr, (L7_uchar8)length, conflict);

       break;

  case D_pimSmStaticRPRowStatus_destroy:
       if(usmDbPimsmStaticRPAddrGet(addrType, &pimStaticRPGrpAddr,
                         length, &pimSmStaticRPRPAddr) == L7_SUCCESS)
        {

           rc = usmDbPimsmStaticRPRemove(addrType, &pimSmStaticRPRPAddr,
                                  &pimStaticRPGrpAddr, (L7_uchar8)length);
        }
        else
        {
          return L7_FAILURE;
        }
       break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpPimSmbsrCandidateRPStatusGet (L7_int32 addrType, L7_inet_addr_t *bsrCandtRPAddr, L7_inet_addr_t *bsrCandtRPGrpAddr, L7_uint32 length, L7_int32 *val )
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 mode;

  rc = usmDbPimsmbsrCandRPModeGet(addrType, bsrCandtRPAddr, bsrCandtRPGrpAddr, length, &mode);
  if (rc == L7_SUCCESS)
  {
    switch (mode)
    {
    case L7_ENABLE:
      *val = D_pimSmInterfaceStatus_active;
      break;
      
    case L7_DISABLE:
      *val = D_pimSmInterfaceStatus_notInService;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpPimSmbsrCandidateRPStatusSet (L7_int32 addrType, L7_inet_addr_t bsrCandtRPAddr, L7_inet_addr_t bsrCandtRPGrpAddr, L7_uint32 length, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIface;
  L7_inet_addr_t bsrCandtRPGrpMask;

  inetAddressZeroSet(L7_AF_INET6,&bsrCandtRPGrpMask);

  if ( (val == D_bsrCandidateRPStatus_createAndGo) ||  (val == D_bsrCandidateRPStatus_destroy))
  {
     if (D_bsrCandidateRPAddressType_ipv4 == addrType)
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
     else if (D_bsrCandidateRPAddressType_ipv6 == addrType)
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

  switch (val)
  {
  case D_bsrCandidateRPStatus_active:
  case D_bsrCandidateRPStatus_createAndGo:

       rc = usmDbPimsmCandRPSet(addrType, intIface,
                                    &bsrCandtRPGrpAddr, &bsrCandtRPGrpMask, L7_ENABLE);
       break;

  case D_bsrCandidateRPStatus_destroy:
       rc = usmDbPimsmCandRPSet(addrType, intIface,
                                    &bsrCandtRPGrpAddr, &bsrCandtRPGrpMask, L7_DISABLE);
       break;

  default:
      rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpPimSmbsrCandidateBSRStatusGet(L7_int32 addrType, L7_int32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 mode;

  rc = usmDbPimsmCandBSRModeGet(addrType, &mode);
  if (rc == L7_SUCCESS)
  {
    switch (mode)
    {
    case L7_ENABLE:
      *val = D_bsrCandidateBSRStatus_active;
      break;
      
    case L7_DISABLE:
      *val = D_bsrCandidateBSRStatus_notInService;
      break;

    default:
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpPimSmbsrCandidateBSRSet(L7_int32 addrType, L7_int32 val, L7_inet_addr_t bsrCandtBSRAddr, 
                                  L7_uint32 priority, L7_uint32 maskLength )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIface = 0;
  if (D_pimSmStaticRPAddressType_ipv4 == addrType)
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
  else if (D_pimSmStaticRPAddressType_ipv6 == addrType)
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
  case D_bsrCandidateBSRStatus_active:
  case D_bsrCandidateBSRStatus_createAndGo:
       rc = usmDbPimsmCandBSRSet(USMDB_UNIT_CURRENT, addrType, intIface, priority, L7_ENABLE, maskLength, L7_NULL);
       break;

  case D_bsrCandidateBSRStatus_destroy:
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
     if (*origin == D_pimSmGroupMappingOrigin_bsr)
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


L7_RC_t snmpPimSmInterfaceProtocolGet(L7_uint32 extIfNum, L7_uint32 version)
{

  L7_uint32 intIfNum;
  L7_RC_t   rc = L7_FAILURE;

  /* check to see if this is a visible external interface number */
  if (usmDbExtIfNumTypeCheckValid(USMDB_UNIT_CURRENT, USM_ROUTER_INTF, 0, extIfNum) 
                                  == L7_SUCCESS)
  {
    rc = usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum);
    if (rc == L7_SUCCESS)
    {
      if (usmDbPimsmInterfaceEntryGet(USMDB_UNIT_CURRENT, version, 
                                      intIfNum) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}


L7_RC_t snmpPimSmInterfaceProtocolNextGet(L7_uint32 extIfNum, L7_uint32 version,
                                          L7_uint32 *nextExtIfNum, L7_uint32 *nextVersion)
{

  if (snmpPimSmInterfaceProtocolGet(extIfNum, version) != L7_SUCCESS)
  {
    /* Get Next Version */ 
    version++;
    if (snmpPimSmInterfaceProtocolGet(extIfNum, version) == L7_SUCCESS)
    {
      *nextExtIfNum = extIfNum;
      *nextVersion = version;
      return L7_SUCCESS;
    }
  }
  
  /* Get Next Interface */
  *nextExtIfNum = extIfNum;
  while (usmDbExtIfNumTypeNextGet(USMDB_UNIT_CURRENT,USM_ROUTER_INTF, 0, *nextExtIfNum, 
                                   nextExtIfNum) == L7_SUCCESS)
  {
    *nextVersion = L7_AF_INET;
    while (snmpPimSmInterfaceProtocolGet(*nextExtIfNum, *nextVersion) != L7_SUCCESS
           && *nextVersion <= L7_AF_INET6) 
    {
     *nextVersion = *nextVersion + 1;
    }
    if (*nextVersion <= L7_AF_INET6)
    {
       return L7_SUCCESS;
    }
  }
  *nextVersion = 0;
  *nextExtIfNum = 0;
  return L7_FAILURE;
}


  

#if 0

L7_RC_t
snmpPimdmNeighborUpTimeGet(L7_uint32 UnitIndex, L7_uint32 ipNbr, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_inet_addr_t pimIpNbr;

  inetAddressReset(&pimIpNbr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipNbr, &pimIpNbr))
    return L7_FAILURE;

  rc = usmDbPimNeighborUpTimeGet(UnitIndex, &pimIpNbr, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}


L7_RC_t
snmpPimdmNeighborExpiryTimeGet(L7_uint32 UnitIndex, L7_uint32 ipNbr, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_inet_addr_t pimIpNbr;

  inetAddressReset(&pimIpNbr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipNbr, &pimIpNbr))
    return L7_FAILURE;

  rc = usmDbPimNeighborExpiryTimeGet(UnitIndex, &pimIpNbr, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}


L7_RC_t
snmpPimsmInterfaceModeGet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 mode;

  rc = usmDbPimsmInterfaceModeGet(UnitIndex, intIfNum, &mode);

  if(mode == L7_ENABLE)
      *val = D_pimSmInterfaceMode_sparse;
  else if( mode == L7_DISABLE)
      *val = D_pimSmInterfaceMode_dense;

  return rc;
}


L7_RC_t
snmpPimsmNeighborUpTimeGet(L7_uint32 UnitIndex, L7_uint32 ipNbr, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_inet_addr_t pimIpNbr;

  inetAddressReset(&pimIpNbr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipNbr, &pimIpNbr))
    return L7_FAILURE;

  rc = usmDbPimsmNeighborUpTimeGet(UnitIndex, &pimIpNbr, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}


L7_RC_t
snmpPimsmNeighborExpiryTimeGet(L7_uint32 UnitIndex, L7_uint32 ipNbr, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_inet_addr_t pimIpNbr;

  inetAddressReset(&pimIpNbr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipNbr, &pimIpNbr))
    return L7_FAILURE;

  rc = usmDbPimsmNeighborExpiryTimeGet(UnitIndex, &pimIpNbr, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}


L7_RC_t
snmpPimsmRPSetExpiryTimeGet(L7_uint32 UnitIndex, L7_uint32 ipGrp, L7_uint32 grpMask, L7_uint32 ipRpSet, L7_uint32 rpComponent, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_inet_addr_t pimIpGrp, pimIpSrc, pimIpMask;

  inetAddressReset(&pimIpGrp);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipGrp, &pimIpGrp))
    return L7_FAILURE;
  inetAddressReset(&pimIpSrc);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipRpSet, &pimIpSrc))
    return L7_FAILURE;
  inetAddressReset(&pimIpMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &grpMask, &pimIpMask))
    return L7_FAILURE;

  rc = usmDbPimsmRPSetExpiryTimeGet(UnitIndex, &pimIpGrp, &pimIpMask, &pimIpSrc, rpComponent, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}


L7_RC_t
snmpPimsmComponentBSRExpiryTimeGet(L7_uint32 UnitIndex, L7_uint32 componentIndx, L7_uint32 *val)
{
  L7_RC_t rc;

  rc = usmDbPimsmComponentBSRExpiryTimeGet(UnitIndex, componentIndx, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
 
  *val = *val * 100;
 
  return rc;

}


#endif

#endif
