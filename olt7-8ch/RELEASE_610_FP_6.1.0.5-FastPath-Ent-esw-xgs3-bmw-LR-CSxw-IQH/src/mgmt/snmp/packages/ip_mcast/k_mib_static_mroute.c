/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename k_mib_fastpathmulticast.c
*
* @purpose FASTPATH Static Multicast Route Private MIB
*
* @component SNMP
*
* @comments
*
* @create
*
* @author cpverne
*
* @end
*
**********************************************************************/
/********************************************************************
 *                    
 *******************************************************************/

#include "k_private_base.h"
#include <inst_lib.h>
#include "usmdb_mib_mcast_api.h"
#include "usmdb_util_api.h"
#include "usmdb_common.h"

agentIpStaticMRouteEntry_t *
k_agentIpStaticMRouteEntry_get(int serialNum, ContextInfo *contextInfo,
                               int nominator,
                               int searchType,
                               SR_INT32 agentIpStaticMRouteSrcAddressType,
                               OctetString * agentIpStaticMRouteSrcIpAddr)
{
#ifdef NOT_YET
  static agentIpStaticMRouteEntry_t agentIpStaticMRouteEntryData;
  L7_inet_addr_t ipStaticMRouteSrcAddr;
  L7_inet_addr_t ipStaticMRouteSrcMask;
  L7_inet_addr_t ipStaticMRouteRpfAddr;
  L7_uint32 unit = USMDB_UNIT_CURRENT;
  static L7_BOOL initializeMem = L7_TRUE;
  L7_uint32 srcV4Addr = 0;
  L7_uint32 srcV4Mask = 0;
  L7_uint32 rpfV4Addr = 0;
  L7_in_addr_t srcAddr;
  L7_in6_addr_t srcV6Addr;
  L7_in6_addr_t srcV6Mask;
  L7_in6_addr_t rpfV6Addr;
  L7_uchar8 addrFamily = 0;
  L7_uint32 intIfNum = 0;
  L7_uint32 preference = 0;

  if (initializeMem == L7_TRUE)
  {
    agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcIpAddr = MakeOctetString (NULL, 0);
    agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcNetMask = MakeOctetString (NULL, 0);
    agentIpStaticMRouteEntryData.agentIpStaticMRouteRpfIpAddr = MakeOctetString (NULL, 0);
    initializeMem = L7_FALSE;
  }

  if (usmDbComponentPresentCheck (unit, L7_FLEX_MCAST_MAP_COMPONENT_ID) != L7_TRUE)
  {
    return L7_NULLPTR;
  }

  inetAddressZeroSet (agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcAddr);
  inetAddressZeroSet (agentIpStaticMRouteSrcAddressType, &ipStaticMRouteSrcMask);
  inetAddressZeroSet (agentIpStaticMRouteSrcAddressType, &ipStaticMRouteRpfAddr);

  /* Clear all the bits*/
  ZERO_VALID (agentIpStaticMRouteEntryData.valid);

  agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType = agentIpStaticMRouteSrcAddressType;
  (void)SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcIpAddr, NULL, 0);
  (void)SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcNetMask, NULL, 0);
  (void)SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteRpfIpAddr, NULL, 0);
  agentIpStaticMRouteEntryData.agentIpStaticMRouteIfIndex = 0;
  agentIpStaticMRouteEntryData.agentIpStaticMRoutePreference = 0;
  SET_VALID (I_agentIpStaticMRouteSrcAddressType, agentIpStaticMRouteEntryData.valid);
  SET_VALID (I_agentIpStaticMRouteIfIndex, agentIpStaticMRouteEntryData.valid);
  SET_VALID (I_agentIpStaticMRoutePreference, agentIpStaticMRouteEntryData.valid);

  if (agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == 0)
  {
    agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType = D_agentIpStaticMRouteSrcAddressType_ipv4;
  }

  if (agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv4)
  {
    if (agentIpStaticMRouteSrcIpAddr->length == sizeof(L7_uint32))
    {
      srcV4Addr = OctetStringToIP (agentIpStaticMRouteSrcIpAddr);
      if (inetAddressSet (L7_AF_INET, &srcV4Addr, &ipStaticMRouteSrcAddr) != L7_SUCCESS)
      {
        return L7_NULLPTR;
      }
    }
  }
  else if (agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv6)
  {
    memset (&srcV6Addr, 0, sizeof(L7_in6_addr_t));
    if (agentIpStaticMRouteSrcIpAddr->length == sizeof(L7_in6_addr_t))
    {
      OctetStringToIP6 (agentIpStaticMRouteSrcIpAddr, srcV6Addr.in6.addr8);
      if (inetAddressSet (L7_AF_INET6, &srcV6Addr, &ipStaticMRouteSrcAddr) != L7_SUCCESS)
      {
        return L7_NULLPTR;
      }
    }
  }
  else
  {
    return L7_NULLPTR;
  }
  addrFamily = agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcAddressType;

  if (searchType == EXACT)
  {
    if (usmDbMcastStaticMRouteEntryGet (unit, addrFamily, &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcMask)
                                     != L7_SUCCESS)
    {
      return L7_NULLPTR;
    }
  }
  else
  {
    if (usmDbMcastStaticMRouteEntryGet (unit, addrFamily, &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcMask)
                                     != L7_SUCCESS) &&
       (usmDbMcastStaticMRouteEntryNextGet (unit, addrFamily, &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcAddr)
                                         != L7_SUCCESS)
    {
      return L7_NULLPTR;
    }
  }

  if (addrFamily == L7_AF_INET)
  {
    if ((inetAddressGet (addrFamily, &ipStaticMRouteSrcAddr, &srcAddr.s_addr) == L7_SUCCESS) &&
        SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcIpAddr,
                           (L7_uchar8 *)&srcAddr.s_addr, sizeof(srcAddr.s_addr)) == L7_TRUE )
    {
      SET_VALID(I_agentIpStaticMRouteSrcIpAddr, agentIpStaticMRouteEntryData.valid);
    }
    else
    {
      return L7_NULLPTR;
    }
  }
  else if (addrFamily == L7_AF_INET6)
  {
    if ((inetAddressGet (addrFamily, &ipStaticMRouteSrcAddr, &srcV6Addr) == L7_SUCCESS) &&
        SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcIpAddr,
                           (L7_uchar8 *)&srcAddr.in6.addr8, sizeof(srcAddr.in6.addr8)) == L7_TRUE )
    {
      SET_VALID(I_agentIpStaticMRouteSrcIpAddr, agentIpStaticMRouteEntryData.valid);
    }
    else
    {
      return L7_NULLPTR;
    }
  }

  SET_VALID(I_agentIpStaticMRouteSrcAddressType, agentIpStaticMRouteEntryData.valid);
  SET_VALID(I_agentIpStaticMRouteSrcIpAddr, agentIpStaticMRouteEntryData.valid);

  switch (nominator)
  {
    case -1:
    case I_agentIpStaticMRouteSrcAddressType:
      break;

    case I_agentIpStaticMRouteSrcIpAddr:
      break;

#if 0
    case I_agentIpStaticMRouteSrcNetMask:
      if (usmDbMcastStaticMRouteSourceMaskGet (unit, addrFamily, &ipStaticMRouteSrcAddr,
                                               &ipStaticMRouteSrcMask)
                                            == L7_SUCCESS)
      {
        if (addrFamily == L7_AF_INET)
        {
          if (inetAddressGet (addrFamily, &ipStaticMRouteSrcMask, &srcV4Mask) == L7_SUCCESS)
          {
            if (SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcNetMask,
                                    (L7_uchar8 *)&srcV4Mask, sizeof(L7_uint32))
                                  == L7_TRUE)
            {
              SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
            }
          }
        }
        else if (addrFamily == L7_AF_INET6)
        {
          if (inetAddressGet (addrFamily, &ipStaticMRouteSrcMask, &srcV6Mask) == L7_SUCCESS)
          {
            if (SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteSrcNetMask,
                                     (L7_uchar8 *)&srcV6Mask, sizeof(L7_in6_addr_t))
                                   == L7_TRUE)
            {
              SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
            }
          }
        }
        SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
      }
      break;
#endif
    case I_agentIpStaticMRouteRpfIpAddr:
      if (usmDbMcastStaticMRouteRpfAddressGet (unit, addrFamily, &ipStaticMRouteSrcAddr,
                                               &ipStaticMRouteSrcAddr, &ipStaticMRouteRpfAddr)
                                            == L7_SUCCESS)
      {
        if (addrFamily == L7_AF_INET)
        {
          if (inetAddressGet (addrFamily, &ipStaticMRouteRpfAddr, &rpfV4Addr) == L7_SUCCESS)
          {
            if (SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteRpfIpAddr,
                                    (L7_uchar8 *)&rpfV4Addr, sizeof(L7_uint32))
                                  == L7_TRUE)
            {
              SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
            }
          }
        }
        else if (addrFamily == L7_AF_INET6)
        {
          if (inetAddressGet (addrFamily, &ipStaticMRouteRpfAddr, &rpfV6Addr) == L7_SUCCESS)
          {
            if (SafeMakeOctetString (&agentIpStaticMRouteEntryData.agentIpStaticMRouteRpfIpAddr,
                                     (L7_uchar8 *)&rpfV6Addr, sizeof(L7_in6_addr_t))
                                   == L7_TRUE)
            {
              SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
            }
          }
        }
        SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
      }
      break;

    case I_agentIpStaticMRouteIfIndex:
      if (usmDbMcastStaticMRouteInterfaceGet (unit, addrFamily, &ipStaticMRouteSrcAddr,
                                              &ipStaticMRouteSrcAddr, &intIfNum)
                                            == L7_SUCCESS)
      {
        agentIpStaticMRouteEntryData.agentIpStaticMRouteIfIndex = intIfNum;
      }
      else
      {
        agentIpStaticMRouteEntryData.agentIpStaticMRouteIfIndex = 0;
      }
      SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
      break;

    case I_agentIpStaticMRoutePreference:
      if (usmDbMcastStaticMRoutePreferenceGet (unit, addrFamily, &ipStaticMRouteSrcAddr,
                                               &ipStaticMRouteSrcAddr, &preference)
                                            == L7_SUCCESS)
      {
        agentIpStaticMRouteEntryData.agentIpStaticMRoutePreference = preference;
      }
      SET_VALID (nominator, agentIpStaticMRouteEntryData.valid);
      break;

    case I_agentIpStaticMRouteStatus:
      break;

    default:
      return L7_NULLPTR;
  }

  if (nominator >= 0 && !VALID(nominator, agentIpStaticMRouteEntryData.valid))
  {
    return L7_NULLPTR;
  }

  return(&pimSmStaticRPEntryData);
#else /* NOT_YET */
  return L7_NULLPTR;
#endif /* NOT_YET */
}

#ifdef SETS
int
k_agentIpStaticMRouteEntry_test(ObjectInfo *object, ObjectSyntax *value,
                                doList_t *dp, ContextInfo *contextInfo)
{
  return NO_ERROR;
}

int
k_agentIpStaticMRouteEntry_ready(ObjectInfo *object, ObjectSyntax *value,
                                 doList_t *doHead, doList_t *dp)
{
  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentIpStaticMRouteEntry_set_defaults(doList_t *dp)
{
  agentIpStaticMRouteEntry_t *data = (agentIpStaticMRouteEntry_t *) (dp->data);

  if ((data->agentIpStaticMRouteSrcNetMask = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }
  if ((data->agentIpStaticMRouteRpfIpAddr = MakeOctetStringFromText("")) == 0)
  {
    return RESOURCE_UNAVAILABLE_ERROR;
  }

  SET_ALL_VALID(data->valid);
  return NO_ERROR;
}

int
k_agentIpStaticMRouteEntry_set(agentIpStaticMRouteEntry_t *data,
                               ContextInfo *contextInfo, int function)
{
  L7_inet_addr_t ipStaticMRouteSrcAddr;
  L7_inet_addr_t ipStaticMRouteSrcMask;
  L7_inet_addr_t ipStaticMRouteRpfAddr;
  L7_uint32 unit = USMDB_UNIT_CURRENT;

  if (usmDbComponentPresentCheck (unit, L7_FLEX_MCAST_MAP_COMPONENT_ID)
                               != L7_TRUE)
  {
    return COMMIT_FAILED_ERROR;
  }

  if (data->agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv4)
  {
    L7_uint32 srcV4Addr;
    L7_uint32 srcV4Mask;
    L7_uint32 rpfV4Addr;

    memset (&srcV4Addr, 0, sizeof(L7_uint32));
    srcV4Addr = OctetStringToIP (data->agentIpStaticMRouteSrcIpAddr);
    if (inetAddressSet (L7_AF_INET, &srcV4Addr, &ipStaticMRouteSrcAddr) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    memset (&srcV4Mask, 0, sizeof(L7_uint32));
    srcV4Mask = OctetStringToIP (data->agentIpStaticMRouteSrcNetMask);
    if (inetAddressSet (L7_AF_INET, &srcV4Mask, &ipStaticMRouteSrcMask) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    memset (&rpfV4Addr, 0, sizeof(L7_uint32));
    rpfV4Addr = OctetStringToIP (data->agentIpStaticMRouteRpfIpAddr);
    if (inetAddressSet (L7_AF_INET, &rpfV4Addr, &ipStaticMRouteRpfAddr) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }
  }
  else if (data->agentIpStaticMRouteSrcAddressType == D_agentIpStaticMRouteSrcAddressType_ipv6)
  {
    L7_in6_addr_t srcV6Addr;
    L7_in6_addr_t srcV6Mask;
    L7_in6_addr_t rpfV6Addr;
    L7_uint32 intIfNum = 0;

    memset (&srcV6Addr, 0, sizeof(L7_in6_addr_t));
    OctetStringToIP6 (data->agentIpStaticMRouteSrcIpAddr, (L7_char8*)&srcV6Addr);
    if (inetAddressSet (L7_AF_INET6, &srcV6Addr, &ipStaticMRouteSrcAddr) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    memset (&srcV6Mask, 0, sizeof(L7_in6_addr_t));
    OctetStringToIP6 (data->agentIpStaticMRouteSrcIpAddr, (L7_char8*)&srcV6Mask);
    if (inetAddressSet (L7_AF_INET6, &srcV6Mask, &ipStaticMRouteSrcMask) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    memset (&rpfV6Addr, 0, sizeof(L7_in6_addr_t));
    OctetStringToIP6 (data->agentIpStaticMRouteSrcIpAddr, (L7_char8*)&rpfV6Addr);
    if (inetAddressSet (L7_AF_INET6, &rpfV6Addr, &ipStaticMRouteRpfAddr) != L7_SUCCESS)
    {
      return COMMIT_FAILED_ERROR;
    }

    if (usmDbIntIfNumFromExtIfNum (data->agentIpStaticMRouteIfIndex, &intIfNum)
                                == L7_SUCCESS)
    {
      data->agentIpStaticMRouteIfIndex = intIfNum;
    }
    else
    {
      data->agentIpStaticMRouteIfIndex = 0;
    }
  }
  else
  {
    return COMMIT_FAILED_ERROR;
  }

  if (VALID (I_agentIpStaticMRouteStatus, data->valid))
  {
    switch (data->agentIpStaticMRouteStatus)
    {
      case D_agentIpStaticMRouteStatus_active:
        break;

      case D_agentIpStaticMRouteStatus_createAndGo:
        if (usmDbMcastStaticMRouteAdd (unit,
                                       data->agentIpStaticMRouteSrcAddressType,
                                       &ipStaticMRouteSrcAddr,
                                       &ipStaticMRouteSrcMask, 
                                       &ipStaticMRouteRpfAddr,
                                       data->agentIpStaticMRouteIfIndex,
                                       data->agentIpStaticMRoutePreference)
                                    != L7_SUCCESS)
        {
          CLR_VALID (I_agentIpStaticMRouteStatus, data->valid);
          return COMMIT_FAILED_ERROR;
        }
        break;

      case D_agentIpStaticMRouteStatus_destroy:
        if (usmDbMcastStaticMRouteDelete (unit,
                                          data->agentIpStaticMRouteSrcAddressType,
                                          &ipStaticMRouteSrcAddr, &ipStaticMRouteSrcAddr)
                                    != L7_SUCCESS)
        {
          CLR_VALID (I_agentIpStaticMRouteStatus, data->valid);
          return COMMIT_FAILED_ERROR;
        }
        break;

      default:
        return COMMIT_FAILED_ERROR;
    }
  }

  return NO_ERROR;
}

#ifdef SR_agentIpStaticMRouteEntry_UNDO
/* add #define SR_agentIpStaticMRouteEntry_UNDO in sitedefs.h to
 * include the undo routine for the agentIpStaticMRouteEntry family.
 */
int
agentIpStaticMRouteEntry_undo(doList_t *doHead, doList_t *doCur,
                              ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_agentIpStaticMRouteEntry_UNDO */

#endif /* SETS */
