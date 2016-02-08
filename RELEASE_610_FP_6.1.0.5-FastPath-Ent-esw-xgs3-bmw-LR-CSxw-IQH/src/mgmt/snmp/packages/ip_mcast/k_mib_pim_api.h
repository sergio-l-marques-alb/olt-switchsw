/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_pim_api.h
*
* @purpose SNMP specific value conversion
*
* @component SNMP
*
* @comments This file is included at the top of the k_mib_pim.c
*
* @create 04/09/2001
*
* @author cpverne
*
* @end
*
**********************************************************************/

#include "usmdb_common.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_pim_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"
#include "usmdb_mib_pimsm_api.h"
#endif



#ifndef _K_MIB_PIM_H
#define _K_MIB_PIM_H
#include "k_mib_pim_config.h"

L7_RC_t
snmpPimInterfaceStatusGet ( L7_uint32 UnitIndex, L7_uint32 pimInterfaceIfIndex, L7_int32 *val )
{
   L7_uint32 mode;

    if(usmDbPimInterfaceModeGet(UnitIndex, L7_AF_INET, pimInterfaceIfIndex, &mode)==L7_SUCCESS)
    {
      if(mode==L7_ENABLE)
      {
        if(usmDbPimInterfaceOperationalStateGet(UnitIndex, L7_AF_INET, pimInterfaceIfIndex) == L7_TRUE)
        {
         *val = D_pimInterfaceStatus_active;
         return L7_SUCCESS;
        }
      }  
    
    *val = D_pimInterfaceStatus_notInService;
     return L7_SUCCESS;
   }
  
  return L7_FAILURE;
}

L7_RC_t
snmpPimInterfaceStatusSet ( L7_uint32 UnitIndex, L7_uint32 pimInterfaceIfIndex, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_pimInterfaceStatus_active:
        if(usmDbPimInterfaceOperationalStateGet(UnitIndex, L7_AF_INET, pimInterfaceIfIndex) == L7_TRUE)
        {
         rc = usmDbPimInterfaceModeSet(UnitIndex, L7_AF_INET , pimInterfaceIfIndex,L7_ENABLE);
        }
        else
        {
         rc = L7_FAILURE;
        }
     
    break;
         
  case D_pimInterfaceStatus_createAndGo:
      rc = usmDbPimInterfaceModeSet(UnitIndex, L7_AF_INET, pimInterfaceIfIndex,L7_ENABLE);
    break;
  
  case D_pimInterfaceStatus_destroy:
      rc = usmDbPimInterfaceModeSet(UnitIndex, L7_AF_INET, pimInterfaceIfIndex,L7_DISABLE);
    break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpPimNeighborUpTimeGet(L7_uint32 UnitIndex, L7_uint32 ipNbr, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_inet_addr_t pimIpNbr;

  inetAddressReset(&pimIpNbr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipNbr, &pimIpNbr))
    return L7_FAILURE;

  if(L7_SUCCESS != usmDbPimNeighborIfIndexGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpNbr, &intIfNum))
    return L7_FAILURE;

  rc = usmDbPimNeighborUpTimeGet(UnitIndex, L7_AF_INET, intIfNum, &pimIpNbr, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}


L7_RC_t
snmpPimNeighborExpiryTimeGet(L7_uint32 UnitIndex, L7_uint32 ipNbr, L7_uint32 *val)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_inet_addr_t pimIpNbr;

  inetAddressReset(&pimIpNbr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipNbr, &pimIpNbr))
    return L7_FAILURE;

  if(L7_SUCCESS != usmDbPimNeighborIfIndexGet(USMDB_UNIT_CURRENT, L7_AF_INET, &pimIpNbr, &intIfNum))
    return L7_FAILURE;

  rc = usmDbPimNeighborExpiryTimeGet(UnitIndex, L7_AF_INET, intIfNum, &pimIpNbr, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}


L7_RC_t
snmpPimsmInterfaceModeGet ( L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 mode;

  rc = usmDbPimsmInterfaceModeGet(UnitIndex, L7_AF_INET, intIfNum, &mode);

  if(mode == L7_ENABLE)
      *val = D_pimInterfaceMode_sparse;
  else if( mode == L7_DISABLE)
      *val = D_pimInterfaceMode_dense;

  return rc;
}


L7_RC_t
snmpPimsmNeighborUpTimeGet(L7_uint32 UnitIndex, L7_uint32 ipNbr, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_inet_addr_t pimIpNbr;

  inetAddressReset(&pimIpNbr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipNbr, &pimIpNbr))
    return L7_FAILURE;
#ifdef SNMP_FP_MCAST_MIB_TODO
  rc = usmDbPimsmNeighborUpTimeGet(UnitIndex, L7_AF_INET, &pimIpNbr, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;
#else
  rc = L7_FAILURE;
#endif

  return rc;

}


L7_RC_t
snmpPimsmNeighborExpiryTimeGet(L7_uint32 UnitIndex, L7_uint32 ipNbr, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;
  L7_inet_addr_t pimIpNbr;

  inetAddressReset(&pimIpNbr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET, &ipNbr, &pimIpNbr))
    return L7_FAILURE;

#ifdef SNMP_FP_MCAST_MIB_TODO
  rc = usmDbPimsmNeighborExpiryTimeGet(UnitIndex, L7_AF_INET, &pimIpNbr, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;
#else
  rc = L7_FAILURE;
#endif

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

  rc = usmDbPimsmRPSetExpiryTimeGet(UnitIndex, L7_AF_INET, &pimIpGrp, &pimIpMask, &pimIpSrc, rpComponent, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;

  return rc;

}


L7_RC_t
snmpPimsmComponentBSRExpiryTimeGet(L7_uint32 UnitIndex, L7_uint32 componentIndx, L7_uint32 *val)
{
  L7_RC_t rc = L7_FAILURE;


#ifdef SNMP_FP_MCAST_MIB_TODO
  rc = usmDbPimsmComponentBSRExpiryTimeGet(UnitIndex, componentIndx, val);

  /* usmdb returns time in seconds, SNMP returns time in timeticks measured by hundredths of a second */
  *val = *val * 100;
#else
  rc = L7_FAILURE;
#endif
  return rc;

}



#endif
