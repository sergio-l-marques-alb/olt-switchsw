/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_dvmrp_api.h
*
* @component SNMP
*
* @comments
*
* @create
*
* Created by: Mahe Korukonda (BRI) 03/19/2002
* @end
*
**********************************************************************/

#include "usmdb_mib_dvmrp_api.h"
#include "l3_mcast_commdefs.h"
#include "l7_mgmd_api.h"

/********************************************************************
 *                    
 *******************************************************************/

L7_RC_t
snmpDvmrpIntfEntryGet(L7_uint32 UnitIndex, L7_uint32 *intIfNum, L7_uint32 nominator)
{
  L7_RC_t rc;
  L7_ulong32 tmpVal;

  rc = usmDbDvmrpIntfEntryGet(UnitIndex, intIfNum);

  if (rc == L7_SUCCESS)
  {
    switch (nominator)
    {
    case I_dvmrpInterfaceRcvBadPkts:
      if (usmDbDvmrpIntfRcvBadPktsGet(UnitIndex, *intIfNum, &tmpVal) != L7_SUCCESS)
        rc = L7_FAILURE;
      break;

    case I_dvmrpInterfaceRcvBadRoutes:
      if (usmDbDvmrpIntfRcvBadRoutesGet(UnitIndex, *intIfNum, &tmpVal) != L7_SUCCESS)
        rc = L7_FAILURE;
      break;

    case I_dvmrpInterfaceSentRoutes:
      if (usmDbDvmrpIntfSentRoutesGet(UnitIndex, *intIfNum, &tmpVal) != L7_SUCCESS)
        rc = L7_FAILURE;
      break;

    default:
      break;
    }
  }

  return rc;
}

L7_RC_t
snmpDvmrpInterfaceEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *intIfNum, L7_uint32 nominator)
{
  L7_RC_t rc;
  L7_ulong32 tmpVal;

  while (1)
  {
    rc = usmDbDvmrpInterfaceEntryNextGet(UnitIndex, intIfNum);

    if (rc == L7_SUCCESS)
    {
      switch (nominator)
      {
      case I_dvmrpInterfaceRcvBadPkts:
        if (usmDbDvmrpIntfRcvBadPktsGet(UnitIndex, *intIfNum, &tmpVal) != L7_SUCCESS)
          continue;
        break;

      case I_dvmrpInterfaceRcvBadRoutes:
        if (usmDbDvmrpIntfRcvBadRoutesGet(UnitIndex, *intIfNum, &tmpVal) != L7_SUCCESS)
          continue;
        break;

      case I_dvmrpInterfaceSentRoutes:
        if (usmDbDvmrpIntfSentRoutesGet(UnitIndex, *intIfNum, &tmpVal) != L7_SUCCESS)
          continue;
        break;

      default:
        break;
      }

      break;
    }
    else
    {
      break;
    }
  }

  return rc;
}



L7_RC_t
snmpDvmrpInterfaceStatusGet ( L7_uint32 UnitIndex, L7_uint32 dvmrpInterfaceIfIndex, L7_int32 *val )
{

  L7_uint32 mode;

    if(usmDbDvmrpIntfAdminModeGet(UnitIndex, dvmrpInterfaceIfIndex, &mode)==L7_SUCCESS)
    {
     if(mode==L7_ENABLE)
     {
      if(usmDbDvmrpInterfaceOperationalStateGet(UnitIndex, dvmrpInterfaceIfIndex) == L7_TRUE)
      {
        *val = D_dvmrpInterfaceStatus_active;
         return L7_SUCCESS;
      }
     }
     *val = D_dvmrpInterfaceStatus_notInService;
     return L7_SUCCESS; 
    }
  return L7_FAILURE;

}


L7_RC_t
snmpDvmrpInterfaceStatusSet ( L7_uint32 UnitIndex, L7_uint32 dvmrpInterfaceIfIndex, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_dvmrpInterfaceStatus_active:
       if(usmDbDvmrpInterfaceOperationalStateGet(UnitIndex, dvmrpInterfaceIfIndex) == L7_TRUE)
         rc = usmDbDvmrpIntfAdminModeSet(UnitIndex,dvmrpInterfaceIfIndex,L7_ENABLE);
       else
         rc = L7_FAILURE; 
  case D_dvmrpInterfaceStatus_createAndGo:
       rc = usmDbDvmrpIntfAdminModeSet(UnitIndex,dvmrpInterfaceIfIndex,L7_ENABLE);
       break;

  case D_dvmrpInterfaceStatus_destroy:
       rc = usmDbDvmrpIntfAdminModeSet(UnitIndex,dvmrpInterfaceIfIndex,L7_DISABLE);
       break;

  default:
    rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t
snmpDvmrpNeighborCapabilitiesGet ( L7_uint32 UnitIndex, L7_uint32 nbrIfIndex, L7_uint32  nbrIfAddr, L7_uint32 *val )
{

  L7_ushort16 temp_val;
  L7_inet_addr_t dvmrpNbrAddr;
  L7_RC_t rc;

  rc = L7_FAILURE;

  inetAddressReset(&dvmrpNbrAddr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&nbrIfAddr,&dvmrpNbrAddr))
    return(L7_FAILURE);

  rc = usmDbDvmrpNeighborCapabilitiesGet(UnitIndex, nbrIfIndex, &dvmrpNbrAddr, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DVMRP_CAPABILITIES_LEAF:
      *val = D_dvmrpNeighborCapabilities_leaf;
      break;

    case L7_DVMRP_CAPABILITIES_PRUNE:
      *val = D_dvmrpNeighborCapabilities_prune;
      break;

    case L7_DVMRP_CAPABILITIES_GENID:
      *val = D_dvmrpNeighborCapabilities_generationID;
      break;

    case L7_DVMRP_CAPABILITIES_MTRACE:
      *val = D_dvmrpNeighborCapabilities_mtrace;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpDvmrpNeighborStateGet ( L7_uint32 UnitIndex, L7_uint32 nbrIfIndex, L7_inet_addr_t  *nbrIfAddr, L7_uint32 *val )
{

  L7_ulong32 temp_val;
  L7_RC_t rc;

  rc = L7_FAILURE;

  rc = usmDbDvmrpNeighborStateGet(UnitIndex, nbrIfIndex, nbrIfAddr, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DVMRP_NEIGHBOR_STATE_ONEWAY:
      *val = D_dvmrpNeighborState_oneway;
      break;

    case L7_DVMRP_NEIGHBOR_STATE_ACTIVE:
      *val = D_dvmrpNeighborState_active;
      break;

    case L7_DVMRP_NEIGHBOR_STATE_IGNORING:
      *val = D_dvmrpNeighborState_ignoring;
      break;

    case L7_DVMRP_NEIGHBOR_STATE_DOWN:
      *val = D_dvmrpNeighborState_down;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpDvmrpNextHopTypeGet ( L7_uint32 UnitIndex, L7_uint32 nextHopSrc, L7_uint32 nextHopSrcMask, L7_uint32 nextHopIfIndex, L7_uint32 *val )
{

  L7_uint32 temp_val;
  L7_inet_addr_t dvmrpNbrAddr;
  L7_inet_addr_t dvmrpNbrMask;
  L7_RC_t rc;

  rc = L7_FAILURE;

  inetAddressReset(&dvmrpNbrAddr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&nextHopSrc,&dvmrpNbrAddr))
    return(L7_FAILURE);
  inetAddressReset(&dvmrpNbrMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&nextHopSrcMask,&dvmrpNbrMask))
    return(L7_FAILURE);

  rc = usmDbDvmrpNextHopTypeGet(UnitIndex, &dvmrpNbrAddr, &dvmrpNbrMask, nextHopIfIndex, &temp_val);

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_DVMRP_NEXTHOP_TYPE_LEAF:
      *val = D_dvmrpRouteNextHopType_leaf;
      break;

    case L7_DVMRP_NEXTHOP_TYPE_BRANCH:
      *val = D_dvmrpRouteNextHopType_branch;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpDvmrpRouteExpiryTimeGet ( L7_uint32 UnitIndex, L7_uint32 dvmrpRouteSource, L7_uint32 dvmrpRouteSourceMask, L7_uint32* dvmrpRouteExpiryTime )
{

  L7_ulong32 temp_val;
  L7_inet_addr_t dvmrpNbrAddr;
  L7_inet_addr_t dvmrpNbrMask;
  L7_RC_t rc;

  rc = L7_FAILURE;

  inetAddressReset(&dvmrpNbrAddr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpRouteSource,&dvmrpNbrAddr))
    return(L7_FAILURE);
  inetAddressReset(&dvmrpNbrMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpRouteSourceMask,&dvmrpNbrMask))
    return(L7_FAILURE);

  if(usmDbDvmrpRouteExpiryTimeGet(UnitIndex, &dvmrpNbrAddr, &dvmrpNbrMask, &temp_val) == L7_SUCCESS)
  {
      *dvmrpRouteExpiryTime = (L7_uint32)temp_val * 100;
      return L7_SUCCESS;
  }
  return rc;
}

L7_RC_t
snmpDvmrpRouteUpTimeGet ( L7_uint32 UnitIndex, L7_uint32 dvmrpRouteSource, L7_uint32 dvmrpRouteSourceMask, L7_uint32* dvmrpRouteUpTime )
{

  L7_ulong32 temp_val;
  L7_inet_addr_t dvmrpNbrAddr;
  L7_inet_addr_t dvmrpNbrMask;
  L7_RC_t rc;

  rc = L7_FAILURE;

  inetAddressReset(&dvmrpNbrAddr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpRouteSource,&dvmrpNbrAddr))
    return(L7_FAILURE);
  inetAddressReset(&dvmrpNbrMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpRouteSourceMask,&dvmrpNbrMask))
    return(L7_FAILURE);

  if(usmDbDvmrpRouteUpTimeGet(UnitIndex, &dvmrpNbrAddr, &dvmrpNbrMask, &temp_val) == L7_SUCCESS)
  {
      *dvmrpRouteUpTime = (L7_uint32)temp_val * 100;
      return L7_SUCCESS;
  }
  return rc;
}

L7_RC_t
snmpDvmrpPruneExpiryTimeGet ( L7_uint32 UnitIndex, L7_uint32 dvmrpPruneGroup, L7_uint32 dvmrpPruneSource, L7_uint32 dvmrpPruneSourceMask, L7_uint32 *dvmrpPruneExpiryTime)
{

  L7_ulong32 temp_val;
  L7_inet_addr_t dvmrpGrpAddr;
  L7_inet_addr_t dvmrpNbrAddr;
  L7_inet_addr_t dvmrpNbrMask;
  L7_RC_t rc;

  rc = L7_FAILURE;


  inetAddressReset(&dvmrpGrpAddr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpPruneGroup,&dvmrpGrpAddr))
    return(L7_FAILURE);
  inetAddressReset(&dvmrpNbrAddr);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpPruneSource,&dvmrpNbrAddr))
    return(L7_FAILURE);
  inetAddressReset(&dvmrpNbrMask);
  if(L7_SUCCESS != inetAddressSet(L7_AF_INET,&dvmrpPruneSourceMask,&dvmrpNbrMask))
    return(L7_FAILURE);

  if(usmDbDvmrpPruneExpiryTimeGet(UnitIndex, &dvmrpGrpAddr, &dvmrpNbrAddr, &dvmrpNbrMask, &temp_val) == L7_SUCCESS)
  {
      *dvmrpPruneExpiryTime = (L7_uint32)temp_val * 100;
      return L7_SUCCESS;
  }
  return rc;
}
