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
snmpPimSmInterfaceStatusGet (L7_uint32 pimInterfaceIfIndex, L7_int32 pimInetIPVersion, L7_int32 *val )
{
  
  L7_uint32 mode;

  if(usmDbPimsmInterfaceModeGet(USMDB_UNIT_CURRENT,pimInetIPVersion ,pimInterfaceIfIndex, &mode)==L7_SUCCESS)
  {
    if(mode==L7_ENABLE)
    {
     if(usmDbPimsmInterfaceOperationalStateGet(USMDB_UNIT_CURRENT, pimInetIPVersion,pimInterfaceIfIndex) == L7_TRUE)
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
snmpPimInterfaceStatusSet (L7_uint32 pimInterfaceIfIndex,L7_int32 pimInetIPVersion, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_pimInterfaceStatus_active:
  case D_pimInterfaceStatus_createAndGo:
       rc = usmDbPimInterfaceModeSet(USMDB_UNIT_CURRENT,pimInetIPVersion,pimInterfaceIfIndex, L7_ENABLE);
       break;

  case D_pimInterfaceStatus_destroy:
       rc = usmDbPimInterfaceModeSet(USMDB_UNIT_CURRENT, pimInetIPVersion,pimInterfaceIfIndex, L7_DISABLE);
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
snmpPimSmSsmRangeStatusSet (L7_int32 addrType, L7_inet_addr_t *pimSsmRangeAddr, L7_uint32 length, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_pimInterfaceStatus_active:
  case D_pimInterfaceStatus_createAndGo:

       rc = usmDbPimsmSsmRangeModeSet(addrType, pimSsmRangeAddr,length, L7_ENABLE);
       break;

  case D_pimInterfaceStatus_destroy:

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
     *val = D_pimStaticRPRowStatus_active;
  else
     *val = D_pimStaticRPRowStatus_notInService;

  return L7_SUCCESS;
}

L7_RC_t
snmpPimSmStaticRPStatusSet (L7_int32 addrType, L7_inet_addr_t pimStaticRPRPAddr, L7_inet_addr_t pimStaticRPGrpAddr,
                            L7_uint32 length, L7_int32 overRide, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL conflict = L7_FALSE;

  if (overRide == D_pimStaticRPOverrideDynamic_true)
  {
   conflict = L7_TRUE;
  }
  switch (val)
  {
  case D_pimStaticRPRowStatus_active:
       break;
  case D_pimStaticRPRowStatus_createAndGo:
       rc = usmDbPimsmStaticRPSet(addrType, &pimStaticRPRPAddr,
                                  &pimStaticRPGrpAddr, (L7_uchar8)length, conflict);

       break;

  case D_pimStaticRPRowStatus_destroy:
       if(usmDbPimsmStaticRPAddrGet(addrType, &pimStaticRPGrpAddr,
                         length, &pimStaticRPRPAddr) == L7_SUCCESS)
        {

           rc = usmDbPimsmStaticRPRemove(addrType, &pimStaticRPRPAddr,
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
      *val = D_pimInterfaceMode_sparse;
  else if( mode == L7_DISABLE)
      *val = D_pimInterfaceMode_dense;

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

L7_RC_t
snmpPimInterfaceStatusGet ( L7_uint32 UnitIndex, L7_uint32 version, L7_uint32 pimInterfaceIfIndex, L7_int32 *val )
{
   L7_uint32 mode;

    if(usmDbPimInterfaceModeGet(UnitIndex, version, pimInterfaceIfIndex, &mode)==L7_SUCCESS)
    {
      if(mode==L7_ENABLE)
      {
        if(usmDbPimInterfaceOperationalStateGet(UnitIndex, version, pimInterfaceIfIndex) == L7_TRUE)
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
snmpPimInterfaceEntryGet(L7_uint32 UnitIndex, L7_uint32 extIntfNum, L7_uint32 family)
{
    L7_uint32 intIfNum;


  if((usmDbIntIfNumFromExtIfNum(extIntfNum, &intIfNum) == L7_SUCCESS) && 
     (usmDbPimInterfaceEntryGet(USMDB_UNIT_CURRENT, family, intIfNum) == L7_SUCCESS))
    return L7_SUCCESS;
   
  return L7_FAILURE;
}    


L7_RC_t
snmpPimInterfaceEntryNextGet(L7_uint32 UnitIndex, L7_uint32 extIntfNum, L7_uint32 family, 
                             L7_uint32 *nextIntfNum, L7_uint32 *nextFamily)
{
  L7_uint32 intIfNum = 0;
  L7_uint32 intIfNumV6 = 0, intIfNumV4 = 0;  
  L7_RC_t rc1, rc2; 

  if(extIntfNum != 0)
  {
    if(usmDbIntIfNumFromExtIfNum(extIntfNum, &intIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    intIfNumV4 = intIfNumV6 = intIfNum;
  }

  rc1 = usmDbPimInterfaceEntryNextGet(UnitIndex, L7_AF_INET, &intIfNumV4);
  rc2 = usmDbPimInterfaceEntryNextGet(UnitIndex, L7_AF_INET6, &intIfNumV6);

  if((rc1 == L7_SUCCESS) && (rc2 == L7_SUCCESS)) 
  {
    if(intIfNumV4 > intIfNumV6)
    {
      if(usmDbExtIfNumFromIntIfNum(intIfNumV6, nextIntfNum)== L7_SUCCESS)
      {
        *nextFamily = L7_AF_INET6;
        return L7_SUCCESS;
      }
    }
    else
    {
      if(usmDbExtIfNumFromIntIfNum(intIfNumV4, nextIntfNum)== L7_SUCCESS)
      {
        *nextFamily = L7_AF_INET;
        return L7_SUCCESS;
      }
    }
  }
  else if(rc1 ==  L7_SUCCESS)
  {
    if(usmDbExtIfNumFromIntIfNum(intIfNumV4, nextIntfNum)== L7_SUCCESS)
    {
      *nextFamily = L7_AF_INET;
      return L7_SUCCESS;
    }
  }
  else if(rc2 ==  L7_SUCCESS)
  {
    if(usmDbExtIfNumFromIntIfNum(intIfNumV6, nextIntfNum)== L7_SUCCESS)
    {
      *nextFamily = L7_AF_INET6;
      return L7_SUCCESS;
    }
  }
  else
  {
    return L7_FAILURE;  
  } 

  return L7_FAILURE;
}

L7_RC_t
snmpPimNeighborEntryGet(L7_uint32 UnitIndex, L7_uint32 family , L7_uint32 extIntfNum, L7_inet_addr_t *ipaddr)
{
  L7_uint32 intIfNum;

  if((usmDbIntIfNumFromExtIfNum(extIntfNum, &intIfNum) == L7_SUCCESS) &&
     (usmDbPimNeighborEntryGet(USMDB_UNIT_CURRENT, family, intIfNum, ipaddr) == L7_SUCCESS))
    return L7_SUCCESS;

  return L7_FAILURE;
}


L7_RC_t
snmpPimNeighborEntryNextGet(L7_uint32 UnitIndex, L7_uint32 extIntfNum, L7_inet_addr_t *pimNbrAddr,
                             L7_uint32 *nextIntfNum, L7_uint32 *nextFamily)
{
  L7_uint32 intIfNum = 0;
  L7_uint32 intIfNumV6 = 0, intIfNumV4 = 0;
  L7_RC_t rc1 = L7_FAILURE , rc2 = L7_FAILURE;
  L7_inet_addr_t pimNbrAddrV4, pimNbrAddrV6;

  inetAddressZeroSet(L7_AF_INET, &pimNbrAddrV4);
  inetAddressZeroSet(L7_AF_INET6, &pimNbrAddrV6);

  if(extIntfNum == 0)
  {
    rc1 = usmDbPimNeighborEntryNextGet(UnitIndex, L7_AF_INET, &intIfNumV4, &pimNbrAddrV4);
    rc2 = usmDbPimNeighborEntryNextGet(UnitIndex, L7_AF_INET6, &intIfNumV6, &pimNbrAddrV6);


    if((rc1 == L7_SUCCESS) && (rc2 == L7_SUCCESS))
  {
    if(intIfNumV4 > intIfNumV6)
    {
      if(usmDbExtIfNumFromIntIfNum(intIfNumV6, nextIntfNum)== L7_SUCCESS)
      {
        *nextFamily = L7_AF_INET6;
        memcpy(pimNbrAddr, &pimNbrAddrV6, sizeof(L7_inet_addr_t));
        return L7_SUCCESS;
      }
    }
    else
    {
      if(usmDbExtIfNumFromIntIfNum(intIfNumV4, nextIntfNum)== L7_SUCCESS)
      {
        memcpy(pimNbrAddr, &pimNbrAddrV4, sizeof(L7_inet_addr_t));
        *nextFamily = L7_AF_INET;
        return L7_SUCCESS;
      }
    }
  }
  else if(rc1 ==  L7_SUCCESS)
  {
    if(usmDbExtIfNumFromIntIfNum(intIfNumV4, nextIntfNum)== L7_SUCCESS)
    {

      memcpy(pimNbrAddr, &pimNbrAddrV4, sizeof(L7_inet_addr_t));
      *nextFamily = L7_AF_INET;
      return L7_SUCCESS;
    }
  }
  else if(rc2 ==  L7_SUCCESS)
  {
    if(usmDbExtIfNumFromIntIfNum(intIfNumV6, nextIntfNum)== L7_SUCCESS)
    {
      memcpy(pimNbrAddr, &pimNbrAddrV6, sizeof(L7_inet_addr_t));
      *nextFamily = L7_AF_INET6;
      return L7_SUCCESS;
    }
  } 
  else
  {
    return L7_FAILURE;
  }  
    

  }


  if(extIntfNum != 0)
  {
    if(usmDbIntIfNumFromExtIfNum(extIntfNum, &intIfNum) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    intIfNumV4 = intIfNumV6 = intIfNum;
    memcpy(&pimNbrAddrV4, pimNbrAddr, sizeof(L7_inet_addr_t));
    memcpy(&pimNbrAddrV6, pimNbrAddr, sizeof(L7_inet_addr_t));
   
  }


  if(((rc1 = usmDbPimNeighborEntryNextGet(UnitIndex, L7_AF_INET, &intIfNumV4, &pimNbrAddrV4)) == L7_SUCCESS) && ( intIfNumV4 == intIfNum))
  {

    if(usmDbExtIfNumFromIntIfNum(intIfNum, nextIntfNum)== L7_SUCCESS)
    {
      memcpy(pimNbrAddr, &pimNbrAddrV4, sizeof(L7_inet_addr_t));
      *nextFamily = L7_AF_INET;
      return L7_SUCCESS;
    }
  }
  else if(((rc2 = usmDbPimNeighborEntryNextGet(UnitIndex, L7_AF_INET6, &intIfNumV6, &pimNbrAddrV6)) == L7_SUCCESS) && (intIfNumV6 == intIfNum))
  {
    if(usmDbExtIfNumFromIntIfNum(intIfNum, nextIntfNum)== L7_SUCCESS)
    {
      memcpy(pimNbrAddr, &pimNbrAddrV6, sizeof(L7_inet_addr_t));
      *nextFamily = L7_AF_INET6;
      return L7_SUCCESS;
    }
  } 
  else if((rc1 == L7_SUCCESS) && (rc2 == L7_SUCCESS))
  {
    if(intIfNumV4 > intIfNumV6)
    {
      if(usmDbExtIfNumFromIntIfNum(intIfNumV6, nextIntfNum)== L7_SUCCESS)
      {
        *nextFamily = L7_AF_INET6;
        memcpy(pimNbrAddr, &pimNbrAddrV6, sizeof(L7_inet_addr_t));
        return L7_SUCCESS;
      }
    }
    else
    {
      if(usmDbExtIfNumFromIntIfNum(intIfNumV4, nextIntfNum)== L7_SUCCESS)
      {
        memcpy(pimNbrAddr, &pimNbrAddrV4, sizeof(L7_inet_addr_t));
        *nextFamily = L7_AF_INET;
        return L7_SUCCESS;
      }
    }
  }
  else if(rc1 ==  L7_SUCCESS)
  {
    if(usmDbExtIfNumFromIntIfNum(intIfNumV4, nextIntfNum)== L7_SUCCESS)
    {
      memcpy(pimNbrAddr, &pimNbrAddrV4, sizeof(L7_inet_addr_t));
      *nextFamily = L7_AF_INET;
      return L7_SUCCESS;
    }
  }
  else if(rc2 ==  L7_SUCCESS)
  {
    if(usmDbExtIfNumFromIntIfNum(intIfNumV6, nextIntfNum)== L7_SUCCESS)
    {
      memcpy(pimNbrAddr, &pimNbrAddrV6, sizeof(L7_inet_addr_t));
      *nextFamily = L7_AF_INET6;
      return L7_SUCCESS;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_FAILURE;


}

L7_RC_t snmpPimNeighborUpTimeGet(L7_uint32 unit , L7_uint32 addressType, L7_uint32 intIfNum, L7_inet_addr_t *pimSmNbrAddr, L7_uint32 *upTime)
{
  L7_uint32 time;
  usmDbTimeSpec_t ts;
  L7_RC_t rc;

  rc = usmDbPimNeighborUpTimeGet(unit, addressType, intIfNum, pimSmNbrAddr, &time);
  osapiConvertRawUpTime(time, (L7_timespec *)&ts);

  if (rc == L7_SUCCESS)
  {
    *upTime = ts.seconds;
    *upTime += ts.minutes * SNMP_MINUTES;      /* 60 */
    *upTime += ts.hours * SNMP_HOURS;          /* 60 * 60 */
    *upTime += ts.days * SNMP_DAYS;            /* 60 * 60 * 24 */
    *upTime *= SNMP_HUNDRED;
  }

  return rc;
}


L7_RC_t snmpPimNeighborExpiryTimeGet(L7_uint32 unit , L7_uint32 addressType, L7_uint32 intIfNum, L7_inet_addr_t *pimSmNbrAddr, L7_uint32 *expiryTime)
{
  L7_uint32 time;
  usmDbTimeSpec_t ts;
  L7_RC_t rc;

  rc = usmDbPimNeighborExpiryTimeGet(unit, addressType, intIfNum, pimSmNbrAddr, &time);
  osapiConvertRawUpTime(time, (L7_timespec *)&ts);

  if (rc == L7_SUCCESS)
  {
    *expiryTime = ts.seconds;
    *expiryTime += ts.minutes * SNMP_MINUTES;      /* 60 */
    *expiryTime += ts.hours * SNMP_HOURS;          /* 60 * 60 */
    *expiryTime += ts.days * SNMP_DAYS;            /* 60 * 60 * 24 */
    *expiryTime *= SNMP_HUNDRED;
  }

  return rc;
}






#endif
