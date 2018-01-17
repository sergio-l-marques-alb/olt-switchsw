/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  mcast_api.c
*
* @purpose   MCAST API functions
*
* @component MCAST Mapping Layer
*
* @comments  none
*
* @create    05/04/2006
*
* @author    gkiran
*
* @end
*
**********************************************************************/

#include "l3_defaultconfig.h"
#include "l3_addrdefs.h"
#include "osapi_support.h"
#include "l7_pimsm_api.h"
#include "l7_pimdm_api.h"
#include "l7_dvmrp_api.h"
#include "l7_mcast_api.h"
#include "l7_mgmd_api.h"
#include "mcast_wrap.h"
#include "mcast_map.h"
#include "mcast_util.h"
#include "mcast_debug.h"
#include "buff_api.h" 
#include "mfc_map.h"
#include "l3_mcast_commdefs.h"

/*********************************************************************
* @purpose  Determine if the MCAST component has been initialized
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments This is often used to determine whether a configuration
*           value can be applied or not.
*
* @end
*********************************************************************/
L7_BOOL mcastMapMcastIsOperational(L7_uchar8 family)
{
  if (family == L7_AF_INET)
  {
    return mcastGblVariables_g.mcastMapInfo.mcastV4Initialized;
  }
  else if (family == L7_AF_INET6)
  {
    return mcastGblVariables_g.mcastMapInfo.mcastV6Initialized;
  }
  return L7_FALSE;
}

/*********************************************************************
*                 API FUNCTIONS  -  GLOBAL MULTICAST MODE
********************************************************************/

/**********************************************************************
 * @purpose  Get the MCAST administrative mode
 *
* @param    mode       @b{(output)} admin mode
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
* @comments
 *
 * @end
**********************************************************************/
L7_RC_t mcastMapMcastAdminModeGet(L7_uint32 *mode)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered\n");
  *mode = mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminMode;
  return L7_SUCCESS;
}


/**********************************************************************
 * @purpose  Set the MCAST administrative mode
 *
* @param    mode        @b{(input)}  admin mode
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
* @comments
 *
 * @end
 **********************************************************************/
L7_RC_t mcastMapMcastAdminModeSet(L7_uint32 mode)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
  {
     MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Invalid admin mode = %d\n", mode);
     return L7_FAILURE;
  }

  if (mode == mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminMode)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_CONFIGURED, 
                    "Admin Mode is already enabled.\n");
    return L7_SUCCESS;
  }

  mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminMode = mode;
  mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged = L7_TRUE;

  /* Send the multicast admin mode config event to mcastmap thread to notify 
     the registered components there after */

  if (mcastMapConfigMsgQueue(MCASTMAP_ADMINMODE_SET_EVENT, (L7_VOIDPTR)&mode,
                             sizeof(mode)) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Failed to send Admin mode set event to mcastMap queue ");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to send (config) mcast admin mode change "
            "event  to mcastMap task\n");
    return L7_FAILURE;
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Exit.\n");
  return L7_SUCCESS;
}

/************************************************************************
            API FUNCTIONS  -  CUURENT MROUTE PROTOCOL OPERATIONAL
*************************************************************************/

/***************************************************************
* @purpose   Gets the multicast protocol running on the router.
*
* @param    family                @b{(input)} IP address family
* @param    currentMcastProtocol  @b{(output)} mcast protocol
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
***************************************************************/
L7_RC_t mcastMapIpCurrentMcastProtocolGet(L7_uint32 family, 
                                          L7_MCAST_IANA_PROTO_ID_t *currentMcastProtocol)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  if(family == L7_AF_INET)
  {
    *currentMcastProtocol = mcastGblVariables_g.mcastMapCfgData.rtr.currentMcastProtocol;
  }
  else if (family == L7_AF_INET6)
  {
    *currentMcastProtocol = mcastGblVariables_g.mcastMapCfgData.rtr.currentMcastV6Protocol;
  }
  return L7_SUCCESS;
}


/***************************************************************
* @purpose   Gets the multicast protocol running on the router.
*
* @param    family                @b{(input)} IP address family
* @param    currentMcastProtocol  @b{(output)} mcast protocol
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
***************************************************************/
L7_BOOL mcastMapMcastProtocolIsOperational(L7_uint32 family, 
                            L7_MCAST_IANA_PROTO_ID_t currentMcastProtocol)
{
  L7_BOOL oper = L7_FALSE;
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  switch (currentMcastProtocol)
  {
    case L7_MCAST_IANA_MROUTE_PIM_SM:
        oper = pimsmMapPimsmIsOperational(family);
        break;
    case L7_MCAST_IANA_MROUTE_PIM_DM:
        oper = pimdmMapPimDmIsOperational(family);
        break;
    case L7_MCAST_IANA_MROUTE_DVMRP:
        if (family != L7_AF_INET)
        {
          MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "No support for IPv6 DVMRP.\n");
          break;
        }
        oper = dvmrpMapDvmrpIsOperational();
        break;
    case L7_MCAST_IANA_MROUTE_IGMP_PROXY:
        oper = mgmdMapProxyChkIsEnabled(family);
        break;
    default:        
        break;
  }
  return oper;
}

/***************************************************************
 * @purpose   Sets the multicast protocal running on the router.
 *
* @param     currentMcastProtocol   @b{(input)} mcast protocol
 *
 * @returns   L7_SUCCESS
*
* @comments
*
* @end
***************************************************************/
L7_RC_t mcastMapIpCurrentMcastProtocolSet(L7_uint32 family, L7_uint32 currentMcastProtocol)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if(family == L7_AF_INET)
  {
    mcastGblVariables_g.mcastMapCfgData.rtr.currentMcastProtocol = currentMcastProtocol;
  }
  else if (family == L7_AF_INET6)
  {
    mcastGblVariables_g.mcastMapCfgData.rtr.currentMcastV6Protocol = currentMcastProtocol;
  }


    return L7_SUCCESS;
}

/*********************************************************************
*                 API FUNCTIONS  -  INTERFACE CONFIG THRESHOLD
*********************************************************************/

/**********************************************************************
 * @purpose  Set the Ttl threshold of the specified interface
 *
* @param    intIfNum            @b{(input)} Internal Interface Number
* @param    ttl                 @b{(input)} TTL threshold
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
* @comments
 *
 * @end
**********************************************************************/
L7_RC_t mcastMapIntfTtlThresholdSet(L7_uint32 intIfNum, L7_uint32 ttl)
{
    L7_mcastMapIfCfgData_t   *pCfg;
  mcastMapTtlEventParms_t ttlEventParms;

    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

    if (mcastMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    {
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"MCAST_MAP: Interface = %d is not configurable.\n", intIfNum);
      MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                    "interface %d is not configurable", intIfNum);
      return L7_FAILURE;
    }

    pCfg->ipMRouteIfTtlThresh = ttl;
  mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged = L7_TRUE;
    
  /* Send the TTL config event to mcastmap thread*/

  ttlEventParms.ttl = ttl;
  ttlEventParms.intIfNum = intIfNum;

  if (mcastMapConfigMsgQueue(MCASTMAP_THRESHOLD_SET_EVENT,
                             (L7_VOIDPTR)&ttlEventParms,
                             sizeof(mcastMapTtlEventParms_t)) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Failed to send threshold set event to mcastMap queue ");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to send (config) TTL set event to mcastMap task\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Get the Ttl threshold of the specified interface
 *
* @param    intIfNum            @b{(input)} Internal Interface Number
* @param    ttl                 @b{(output)} TTL threshold
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
* @comments
 *
 * @end
**********************************************************************/
L7_RC_t mcastMapIntfTtlThresholdGet(L7_uint32 intIfNum, L7_uint32 *ttl)
{
    L7_mcastMapIfCfgData_t   *pCfg;

    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
    if (mcastMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    {
        MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Interface = %d is not configurable.\n", intIfNum);
        return L7_FAILURE;
    }

    *ttl= pCfg->ipMRouteIfTtlThresh;

    return L7_SUCCESS;
}

/*********************************************************************
*                 API FUNCTIONS  -  STATIC MULTICAST ROUTES
*********************************************************************/

/***************************************************************
*
* @purpose  Adds/Updates an entry in the Static MRoute Table
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{(input)} Source IP Address
* @param    srcMask    @b{(input)} Source IP Address  Mask
* @param    rpfAddr    @b{(input)} RPF nexthop IP Address
* @param    intIfNum   @b{(input)} Internal Interface Num
* @param    preference @b{(input)} Route preference for the prefix
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
**************************************************************/
L7_RC_t
mcastMapStaticMRouteSet (L7_uchar8 addrFamily,
                         L7_inet_addr_t *srcAddr,
                         L7_inet_addr_t *srcMask,
                         L7_inet_addr_t *rpfAddr,
                         L7_uint32 intIfNum,
                         L7_uint32 preference)
{
  L7_mcastMapCfg_t *cfgData = L7_NULLPTR;
  L7_mcastMapStaticRtsCfgData_t *staticMRouteEntry = L7_NULLPTR;
  L7_uint32 currNumStaticMrouteEntries = 0;
  L7_uint32 tableIndex = 0;
  L7_uint32 rtrIfNum = 0;
  L7_uchar8 src[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 msk[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 rpf[MCAST_MAP_MAX_DBG_ADDR_SIZE];

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if ((srcAddr == L7_NULLPTR) || (srcMask == L7_NULLPTR) ||
      (rpfAddr == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "srcAddr-%p (or) srcMask-%p (or) rpfAddr-%p "
                     " is NULL", srcAddr, srcMask, rpfAddr);
    return L7_FAILURE;
  }
  if ((preference < L7_MCAST_STATICROUTE_METRIC_MIN) ||
      (preference > L7_MCAST_STATICROUTE_METRIC_MAX))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Preference - %d is out of bounds",
                     preference);
    return L7_FAILURE;
  }
  if ((inetIsAddressZero(srcAddr) == L7_TRUE) ||
      (inetIsAddressZero(srcMask) == L7_TRUE) ||
      (inetIsAddressZero(rpfAddr) == L7_TRUE))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "srcAddr-%s (or) srcMask-%s (or) rpfAddr-%s "
                     "is Zero", inetAddrPrint(srcAddr,src),
                     inetAddrPrint(srcMask,msk), inetAddrPrint(rpfAddr,rpf));
    return L7_FAILURE;
  }
  if (inetIpAddressValidityCheck(addrFamily, srcAddr) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "srcAddr-%s"
                        " is invalid", inetAddrPrint(srcAddr,src));
    return L7_NOT_SUPPORTED;
  }
  if (addrFamily == L7_AF_INET)
  {
    if (inetIpAddressValidityCheck(addrFamily, rpfAddr) != L7_SUCCESS)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, " rpfAddr-%s "
                       " is invalid", inetAddrPrint(rpfAddr,rpf));
      return L7_NOT_SUPPORTED;
    }
  }
  else if(addrFamily == L7_AF_INET6)
  {
    if(L7_IP6_IS_ADDR_LINK_LOCAL(&(rpfAddr->addr.ipv6)) == 0)
    {
      if (inetIpAddressValidityCheck(addrFamily, rpfAddr) != L7_SUCCESS)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, " rpfAddr-%s "
                         " is invalid", inetAddrPrint(rpfAddr,rpf));
        return L7_NOT_SUPPORTED;
      }
    }
  }
  if (inetIsMaskValid(srcMask) != L7_TRUE)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "srcMask-%s is Invalid",
                     inetAddrPrint(srcMask,msk));
    return L7_FAILURE;
  }

  /* Do srcAddress and rpfAddr Validations */
  for (rtrIfNum = 1; rtrIfNum < MCAST_MAX_INTERFACES; rtrIfNum++)
  {
    L7_uint32 intIfNum = 0;

    if (ipMapRtrIntfToIntIfNum (rtrIfNum, &intIfNum) == L7_SUCCESS)
    {
      /* Check to see if the Source Address is Directly Connected */
      if (inetIsDirectlyConnected (srcAddr, rtrIfNum) == L7_TRUE)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "srcAddr-%s is a Directly Connected Address",
                         inetAddrPrint(srcAddr,src));
        return L7_ERROR;
      }
  
      /* Check to see if the RPF Address is a Local Address */
      if (inetIsLocalAddress (rpfAddr, rtrIfNum) == L7_TRUE)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "rpfAddr-%s is a Local Address",
                         inetAddrPrint(rpfAddr,rpf));
        return L7_ERROR;
      }
    }
  }

  if (addrFamily == L7_AF_INET6)
  {
    /* Do not allow Global Multicast Address for RPF Address */
    if (L7_IP6_IS_ADDR_MULTICAST (&(rpfAddr->addr.ipv6)) != 0)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Specifying Global Multicast "
                       "Address for RPF Address - %s is invalid",
                       inetAddrPrint(rpfAddr,rpf));
      return L7_FAILURE;
    }

    /* Do not allow Interface for Global RPF Addresses */
    if (L7_IP6_IS_ADDR_LINK_LOCAL (&(rpfAddr->addr.ipv6)) == 0)
    {
      if (intIfNum != 0)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Specifying Interface-%d for "
                         "a Global RPF Address - %s is invalid",
                         intIfNum, inetAddrPrint(rpfAddr,rpf));
        return L7_REQUEST_DENIED;
      }
    }
  }

  if (mcastMapCfgDataGet (addrFamily, &cfgData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData Get Failed for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }
  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData is NULL for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }

  if ((currNumStaticMrouteEntries = cfgData->rtr.numStaticMRouteEntries)
                                  == L7_RTR_MAX_STATIC_MROUTES)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Table Max Limit - "
                     "[%d] Reached", currNumStaticMrouteEntries);
    return L7_FAILURE;
  }

  if (currNumStaticMrouteEntries > 0)
  {
    for (tableIndex = 0; tableIndex < currNumStaticMrouteEntries; tableIndex++)
    {
      if ((staticMRouteEntry = &cfgData->rtr.mcastStaticRtsCfgData[tableIndex]) == L7_NULLPTR)
      {
        continue;
      }

      if ((L7_INET_IS_ADDR_EQUAL (&staticMRouteEntry->source, srcAddr) == L7_TRUE) &&
          (L7_INET_IS_ADDR_EQUAL (&staticMRouteEntry->mask, srcMask) == L7_TRUE))
      {
        if (L7_INET_IS_ADDR_EQUAL (&staticMRouteEntry->rpfAddr, rpfAddr) == L7_TRUE)
        {
          if (staticMRouteEntry->preference != preference)
          {
            MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "Static MRoute Entry "
                             "Preference Updated: srcAddr-%s, Preference-%d",
                             inetAddrPrint(srcAddr,src), preference);
            staticMRouteEntry->preference = preference;
          }
        }
        else
        {
          MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "Static MRoute Entry "
                           "RPF Address Updated: srcAddr-%s, rpfAddr-%s",
                           inetAddrPrint(srcAddr,src), inetAddrPrint(rpfAddr,rpf));

          if ((intIfNum != 0) &&
              (L7_IP6_IS_ADDR_LINK_LOCAL (&(rpfAddr->addr.ipv6)) != 0))
          {
            if (nimConfigIdGet (intIfNum, &staticMRouteEntry->ifConfigId) != L7_SUCCESS)
            {
              MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "intIfNum Get Failed");
              return L7_FAILURE;
            }
          }
          inetCopy (&(staticMRouteEntry->rpfAddr), rpfAddr);

          if (staticMRouteEntry->preference != preference)
          {
            MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "Static MRoute Entry "
                             "Preference Updated: srcAddr-%s, Preference-%d",
                             inetAddrPrint(srcAddr,src), preference);
            staticMRouteEntry->preference = preference;
          }
        }

        mcastMapStaticMRouteRegisteredUsersNotify (addrFamily, staticMRouteEntry,
                                                   RTO_CHANGE_ROUTE);
        return L7_SUCCESS;
      }
    } /* for loop */
  } /* if loop */

  /* Looks like it is a fresh MRoute entry */
  staticMRouteEntry = &cfgData->rtr.mcastStaticRtsCfgData[tableIndex];
  if (addrFamily == L7_AF_INET6)
  {
    if (L7_IP6_IS_ADDR_LINK_LOCAL (&(rpfAddr->addr.ipv6)) != 0)
    {
      if (nimConfigIdGet(intIfNum, &staticMRouteEntry->ifConfigId) != L7_SUCCESS)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "intIfNum Get Failed");
        return L7_FAILURE;
      }
    }
    else 
    {
      if (intIfNum != 0)
      {
        MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Specifying Interface-%d for "
                         "a Global RPF Address - %s is invalid",
                         intIfNum, inetAddrPrint(rpfAddr,rpf));
        return L7_REQUEST_DENIED;
      }
    }
  }
  inetCopy (&(staticMRouteEntry->source), srcAddr);
  inetCopy (&(staticMRouteEntry->mask), srcMask);
  inetCopy (&(staticMRouteEntry->rpfAddr), rpfAddr);
  staticMRouteEntry->preference = preference;

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "Static MRoute Entry Added "
                   "srcAddr-%s, srcMask-%s, rpfAddr-%s, Preference-%d",
                   inetAddrPrint(srcAddr,src), inetAddrPrint(srcMask,msk),
                   inetAddrPrint(rpfAddr,rpf), preference);

  cfgData->rtr.numStaticMRouteEntries++;  
  cfgData->cfgHdr.dataChanged = L7_TRUE;

  mcastMapStaticMRouteRegisteredUsersNotify (addrFamily, staticMRouteEntry,
                                             RTO_ADD_ROUTE);

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/***************************************************************
*
* @purpose  Removes an entry from the Static MRoute Table
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{(input)} Source IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
**************************************************************/
L7_RC_t
mcastMapStaticMRouteReset (L7_uchar8 addrFamily,
                           L7_inet_addr_t *srcAddr,
                           L7_inet_addr_t *srcMask)
{
  L7_mcastMapCfg_t *cfgData = L7_NULLPTR;
  L7_mcastMapStaticRtsCfgData_t *staticMRouteEntryCurr = L7_NULLPTR;
  L7_mcastMapStaticRtsCfgData_t *staticMRouteEntryNext = L7_NULLPTR;
  L7_uint32 currNumStaticMrouteEntries = 0;
  L7_uint32 tableIndexCurr = 0;
  L7_uint32 tableIndexNext = 0;
  L7_uchar8 src[MCAST_MAP_MAX_DBG_ADDR_SIZE];

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if ((srcAddr == L7_NULLPTR) || (srcMask == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"srcAddr-%p (or) srcMask-%p is NULL",
                     srcAddr, srcMask);
    return L7_FAILURE;
  }

  if (mcastMapCfgDataGet (addrFamily, &cfgData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData Get Failed for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }
  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData is NULL for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }

  if ((currNumStaticMrouteEntries = cfgData->rtr.numStaticMRouteEntries) == 0)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Table is Empty");
    return L7_FAILURE;
  }

  for (tableIndexCurr = 0; tableIndexCurr < currNumStaticMrouteEntries; tableIndexCurr++)
  {
    staticMRouteEntryCurr = &cfgData->rtr.mcastStaticRtsCfgData[tableIndexCurr];

    if ((L7_INET_IS_ADDR_EQUAL (&staticMRouteEntryCurr->source, srcAddr) != L7_TRUE) ||
        (L7_INET_IS_ADDR_EQUAL (&staticMRouteEntryCurr->mask, srcMask) != L7_TRUE))
    {
      continue;
    }

    mcastMapStaticMRouteRegisteredUsersNotify (addrFamily, staticMRouteEntryCurr,
                                               RTO_DELETE_ROUTE);

    /* Matching Entry found; Re-arrange the following entries */
    for (tableIndexNext = tableIndexCurr; tableIndexNext < currNumStaticMrouteEntries;
         tableIndexNext++)
    {
      staticMRouteEntryCurr = &cfgData->rtr.mcastStaticRtsCfgData[tableIndexNext];
      staticMRouteEntryNext = &cfgData->rtr.mcastStaticRtsCfgData[tableIndexNext+1]; 
      if ((tableIndexNext+1) == currNumStaticMrouteEntries)
      { 
        inetAddressZeroSet(addrFamily, &(staticMRouteEntryCurr->source));
        inetAddressZeroSet (addrFamily, &(cfgData->rtr.mcastStaticRtsCfgData[tableIndexNext].mask));
        memset (&(staticMRouteEntryCurr->ifConfigId), 0, sizeof(nimConfigID_t));
        inetAddressZeroSet (addrFamily,&(staticMRouteEntryCurr->rpfAddr));
        staticMRouteEntryCurr->preference = 0;                
      }
      else
      {      
        inetCopy (&(staticMRouteEntryCurr->source), &(staticMRouteEntryNext->source));
        inetCopy (&(staticMRouteEntryCurr->mask), &(staticMRouteEntryNext->mask));
        inetCopy (&(staticMRouteEntryCurr->rpfAddr), &(staticMRouteEntryNext->rpfAddr));
        NIM_CONFIG_ID_COPY (&staticMRouteEntryCurr->ifConfigId, 
                            &staticMRouteEntryNext->ifConfigId);
        staticMRouteEntryCurr->preference = staticMRouteEntryNext->preference;
      }
    }

    cfgData->rtr.numStaticMRouteEntries--;
    cfgData->cfgHdr.dataChanged = L7_TRUE;

    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_CONFIGURED, "Static MRoute Entry with "
                     "srcAddr - %s is Removed", inetAddrPrint(srcAddr,src));
    return L7_SUCCESS;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_FAILURE;
}

/************************************************************************
* @purpose  finds the best matching static mroute entry for a given source.
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{(inout)} source address
* @param    srcMask    @b{(output)} source mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteMatchingEntryGet (L7_uchar8 addrFamily,
                                      L7_inet_addr_t *srcAddr,
                                      L7_inet_addr_t *srcMask)
{
  L7_mcastMapCfg_t *cfgData = L7_NULLPTR;
  L7_mcastMapStaticRtsCfgData_t *staticMRouteEntry = L7_NULLPTR;
  L7_uint32 tableIndex = 0;
  L7_inet_addr_t tmpAddr1;
  L7_inet_addr_t tmpAddr2;
  L7_uint32 currNumStaticMrouteEntries = 0;
  L7_uchar8 src[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 maskLen = 0, bestMaskLen = 0, bestTableIndex = 0;
  L7_BOOL foundEntry = L7_FALSE;

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if (srcAddr == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"srcAddr-%p (or) srcMask-%p is NULL",
                     srcAddr, srcMask);
    return L7_FAILURE;
  }

  if (mcastMapCfgDataGet (addrFamily, &cfgData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData Get Failed for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }
  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData is NULL for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }

  if ((currNumStaticMrouteEntries = cfgData->rtr.numStaticMRouteEntries) == 0)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Table is Empty");
    inetAddressZeroSet (addrFamily,srcAddr);
    return L7_FAILURE;
  }

  for (tableIndex = 0; tableIndex < currNumStaticMrouteEntries; tableIndex++)
  {
    staticMRouteEntry = &cfgData->rtr.mcastStaticRtsCfgData[tableIndex];

    inetAddressAnd(&staticMRouteEntry->source, &staticMRouteEntry->mask, &tmpAddr1);
    inetAddressAnd(srcAddr, &staticMRouteEntry->mask, &tmpAddr2);

    if (L7_INET_IS_ADDR_EQUAL(&tmpAddr1, &tmpAddr2) == L7_TRUE)
    {
      inetMaskToMaskLen(&staticMRouteEntry->mask, &maskLen);

      if (maskLen > bestMaskLen)
      {
        bestMaskLen = maskLen;
        bestTableIndex = tableIndex;
        foundEntry = L7_TRUE;
      }
    }
  }

  if (foundEntry == L7_TRUE)
  {
    staticMRouteEntry = &cfgData->rtr.mcastStaticRtsCfgData[bestTableIndex];
    inetCopy (srcAddr, &(staticMRouteEntry->source));
    inetCopy (srcMask, &(staticMRouteEntry->mask));
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Entry with "
                     "srcAddr - %s; Found", inetAddrPrint(srcAddr,src));
    return L7_SUCCESS;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Entry with "
                   "srcAddr - %s; Not Found", inetAddrPrint(srcAddr,src));

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Verifies the entry corresponding to given source address is
*           in the static route table.
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{(input)} source address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteEntryGet (L7_uchar8 addrFamily,
                              L7_inet_addr_t *srcAddr,
                              L7_inet_addr_t *srcMask)
{
  L7_mcastMapCfg_t *cfgData = L7_NULLPTR;
  L7_mcastMapStaticRtsCfgData_t *staticMRouteEntry = L7_NULLPTR;
  L7_uint32 tableIndex = 0;
  L7_uint32 currNumStaticMrouteEntries = 0;
  L7_uchar8 src[MCAST_MAP_MAX_DBG_ADDR_SIZE];

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if ((srcAddr == L7_NULLPTR) || (srcMask == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"srcAddr-%p (or) srcMask-%p is NULL",
                     srcAddr, srcMask);
    return L7_FAILURE;
  }

  if (mcastMapCfgDataGet (addrFamily, &cfgData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData Get Failed for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }
  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData is NULL for addrFamily "
                     "-%d", L7_INET_GET_FAMILY (srcAddr));
    return L7_FAILURE;
  }

  if ((currNumStaticMrouteEntries = cfgData->rtr.numStaticMRouteEntries) == 0)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Table is Empty");
    inetAddressZeroSet (addrFamily,srcAddr);
    return L7_FAILURE;
  }

  for (tableIndex = 0; tableIndex < currNumStaticMrouteEntries; tableIndex++)
  {
    staticMRouteEntry = &cfgData->rtr.mcastStaticRtsCfgData[tableIndex];

    if ((L7_INET_ADDR_COMPARE(&staticMRouteEntry->source, srcAddr) == L7_NULL) &&
        (L7_INET_ADDR_COMPARE(&staticMRouteEntry->mask, srcMask) == L7_NULL))
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Entry with "
                       "srcAddr - %s; Found", inetAddrPrint(srcAddr,src));
      inetCopy (srcAddr, &(staticMRouteEntry->source));
      inetCopy (srcMask, &(staticMRouteEntry->mask));
      return L7_SUCCESS;
    }
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Entry with "
                   "srcAddr - %s; Not Found", inetAddrPrint(srcAddr,src));

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns next entry's source address in the static route
*           table for the given source address.
*
* @param    addrFamily @b{(input)} Address Family Identifier
* @param    srcAddr    @b{inout)}  source address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteEntryNextGet (L7_uchar8 addrFamily,
                                  L7_inet_addr_t *srcAddr,
                                  L7_inet_addr_t *srcMask)
{
  L7_mcastMapCfg_t *cfgData = L7_NULLPTR;
  L7_mcastMapStaticRtsCfgData_t *staticMRouteEntryCurr = L7_NULLPTR;
  L7_uint32 tableIndexCurr = 0;
  L7_uint32 currNumStaticMrouteEntries = 0;
  L7_uchar8 src[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_uchar8 msk[MCAST_MAP_MAX_DBG_ADDR_SIZE];
  L7_inet_addr_t   srcAddrFind,srcAddrLocal;
  L7_uchar8        srcPrefixLenLocal,srcPrefixLenFind, maskLen = 0;
  L7_BOOL          foundEntry = L7_FALSE; 

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if ((srcAddr == L7_NULLPTR) || (srcMask == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"srcAddr-%p (or) srcMask-%p is NULL",
                     srcAddr, srcMask);
    return L7_FAILURE;
  }

  if (mcastMapCfgDataGet (addrFamily, &cfgData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData Get Failed for addrFamily "
                     "-%d", addrFamily);
    return L7_FAILURE;
  }
  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData is NULL for addrFamily "
                     "-%d", addrFamily);
    return L7_FAILURE;
  }

  if ((currNumStaticMrouteEntries = cfgData->rtr.numStaticMRouteEntries) == 0)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Table is Empty");
    return L7_FAILURE;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Entry GetNext Input with "
                   "srcAddr-%s, srcMask-%s", inetAddrPrint(srcAddr,src),
                   inetAddrPrint(srcMask,msk));

  inetMaskToMaskLen(srcMask, &maskLen);

  for (tableIndexCurr = 0; tableIndexCurr < currNumStaticMrouteEntries; tableIndexCurr++)
  {
    staticMRouteEntryCurr = &cfgData->rtr.mcastStaticRtsCfgData[tableIndexCurr];

    inetCopy(&srcAddrLocal, &staticMRouteEntryCurr->source);
    inetMaskToMaskLen(&staticMRouteEntryCurr->mask,&srcPrefixLenLocal);

    if (inetIsAddressZero(&srcAddrLocal) == L7_TRUE)
    {
      continue;
    }

    if (foundEntry == L7_FALSE &&
        (inetAddrCompareAddrWithMaskIndividual(&srcAddrLocal,srcPrefixLenLocal,srcAddr, 
                                            maskLen) > L7_NULL))
    {
      inetCopy(&srcAddrFind, &srcAddrLocal);
      srcPrefixLenFind = srcPrefixLenLocal;
      foundEntry = L7_TRUE;
    }
    else if (foundEntry == L7_TRUE &&
             (inetAddrCompareAddrWithMaskIndividual(&srcAddrLocal,srcPrefixLenLocal,&srcAddrFind, 
                                            srcPrefixLenFind) < L7_NULL) &&
             (inetAddrCompareAddrWithMaskIndividual(&srcAddrLocal,srcPrefixLenLocal,srcAddr, 
                                            maskLen) > L7_NULL))
    {
      inetCopy(&srcAddrFind, &srcAddrLocal);
      srcPrefixLenFind = srcPrefixLenLocal;
    }
  }

  if (foundEntry == L7_TRUE)
  {
    inetCopy(srcAddr, &srcAddrFind);
    inetMaskLenToMask(addrFamily, srcPrefixLenFind, srcMask);
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Next Entry "
                   "with srcAddr-%s, srcMask-%s; Found", inetAddrPrint(srcAddr,src),
                   inetAddrPrint(srcMask,msk));
    return L7_SUCCESS;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Static MRoute Next Entry "
                   "with srcAddr-%s, srcMask-%s; Not Found", inetAddrPrint(srcAddr,src),
                   inetAddrPrint(srcMask,msk));

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets RPF address of interface corresponding to the given
*           source address in the static route table
*
* @param    addrFamily @b{(input)}  Address Family Identifier
* @param    srcAddr    @b{(input)}  source address
* @param    rpfAddr    @b{(output)} RPF address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteRpfAddrGet (L7_uchar8 addrFamily,
                                L7_inet_addr_t *srcAddr,
                                L7_inet_addr_t *srcMask,
                                L7_inet_addr_t *rpfAddr)
{
  L7_uint32 intIfNum = 0;
  L7_uint32 preference = 0;

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if ((srcAddr == L7_NULLPTR) || (srcMask == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"srcAddr-%p (or) srcMask-%p is NULL",
                     srcAddr, srcMask);
    inetAddressZeroSet (addrFamily,rpfAddr);
    return L7_FAILURE;
  }

  if (mcastMapStaticMRouteLookup (addrFamily, srcAddr, srcMask, rpfAddr,
                                  &intIfNum, &preference)
                               != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/***************************************************************
*
* @purpose  Gets the metric value corresponding to given source
*           address in the static route table
*
* @param    addrFamily @b{(input)}  Address Family Identifier
* @param    ipSrc      @b{(input)}  Source IP Address
* @param    preference @b{(output)} Preference for this route
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*
***************************************************************/
L7_RC_t
mcastMapStaticMRoutePreferenceGet (L7_uchar8 addrFamily,
                                   L7_inet_addr_t *srcAddr,
                                   L7_inet_addr_t *srcMask,
                                   L7_uint32 *preference)
{
  L7_inet_addr_t rpfAddr;
  L7_uint32 intIfNum = 0;

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if ((srcAddr == L7_NULLPTR) || (srcMask == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"srcAddr-%p (or) srcMask-%p is NULL",
                     srcAddr, srcMask);
    *preference = 0;
    return L7_FAILURE;
  }

  if (mcastMapStaticMRouteLookup (addrFamily, srcAddr, srcMask, &rpfAddr,
                                  &intIfNum, preference)
                               != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/***************************************************************
*
* @purpose  Gets the RPF interface id corresponding to given source
*           address in the static route table
*
* @param    addrFamily @b{(input)}  Address Family Identifier
* @param    ipSrc      @b{(input)}  Source IP Address
* @param    intIfNum   @b{(output)} Internal Interface Number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*
***************************************************************/
L7_RC_t
mcastMapStaticMRouteRpfInterfaceGet (L7_uchar8 addrFamily,
                                     L7_inet_addr_t *srcAddr,
                                     L7_inet_addr_t *srcMask,
                                     L7_uint32 *intIfNum)
{
  L7_inet_addr_t rpfAddr;
  L7_uint32 preference = 0;

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if (addrFamily == L7_AF_INET)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Interface is Not supported for "
                     " IPv4 Address Family");
    *intIfNum = 0;
    return L7_SUCCESS;
  }
  if (srcAddr == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"srcAddr is NULL");
    *intIfNum = 0;
    return L7_FAILURE;
  }

  if (mcastMapStaticMRouteLookup (addrFamily, srcAddr, srcMask, &rpfAddr,
                                  intIfNum, &preference)
                               != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the total number of entries in the Static MRoute
*           table.
*
* @param    addrFamily @b{(input)}  Address Family Identifier
* @param    numEntries @b{(output)} Total Entry Count
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t
mcastMapStaticMRouteEntryCountGet (L7_uchar8 addrFamily,
                                   L7_uint32 *numEntries)
{
  L7_mcastMapCfg_t *cfgData = L7_NULLPTR;

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Entry");

  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid Address Family Identifier"
                     " -%d", addrFamily);
    return L7_FAILURE;
  }
  if (numEntries == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES,"numEntries is NULL");
    return L7_FAILURE;
  }

  if (mcastMapCfgDataGet (addrFamily, &cfgData) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData Get Failed for addrFamily "
                     "-%d", addrFamily);
    return L7_FAILURE;
  }
  if (cfgData == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "cfgData is NULL for addrFamily "
                     "-%d", addrFamily);
    return L7_FAILURE;
  }

  *numEntries = cfgData->rtr.numStaticMRouteEntries;

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*                 API FUNCTIONS  -  ADMINSCOPE BOUNDARY
*********************************************************************/
              
/**********************************************************************
 * @purpose    Adds an entry to the Admin Scope Boundary List for the
 *             given interface
 *
 * @param      intIfNum        @b{(input)}     Internal Interface Number
* @param      grpAddr      @b{(input)}  Group IP Address
* @param      grpMask      @b{(input)}  Group IP Address Mask
 *
 * @returns    L7_SUCCESS
 * @returns    L7_FAILURE
 * @returns    L7_ERROR
* @returns   L7_ALREADY_CONFIGURED
 * @returns    L7_TABLE_IS_FULL  
 *
* @comments
 *
 * @end
 **********************************************************************/
L7_RC_t mcastMapAdminScopeBoundarySet(L7_uint32       intIfNum,
                                      L7_inet_addr_t *grpAddr,
                                      L7_inet_addr_t *grpMask)
{
  L7_uint32 useIndex = 0;
  L7_uint32 itr, tmpIfNum;
  nimConfigID_t ifConfigId;
  L7_uint32 entriesChecked = 0;
  L7_inet_addr_t notMask, chkAddr;
  L7_uchar8      addrBuffStr[IPV6_DISP_ADDR_LEN];
  mcastMapAdminScopeEventParms_t adminScopeEventParms;

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if ((inetIsAddressZero(grpAddr) == L7_TRUE) || (inetIsAddressZero(grpMask)))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: NULL grp address and mask for admin scope set\n");
    return L7_FAILURE;
  }
  /* This validation is already done for CLI and Web and appropriate 
     error messages are thrown, but added here to validate for SNMP  */
  /* validation of admin scope IPv4 multicast space */
  if (mcastIsAddrAdminScopeRange(grpAddr) != L7_TRUE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: configured group Address = %s is not admin scoped range\n",
            inetAddrPrint(grpAddr,addrBuffStr));
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                    "Group address is not under admin scope range");      
    return L7_FAILURE;
  }
  inetAddressNot(grpMask, &notMask);
  inetAddressAnd(grpAddr, &notMask, &chkAddr);
  if (inetIsAddressZero(&chkAddr) != L7_TRUE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: configured boundary address has host bits set.\n");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"Group range has host bits set");      
    return L7_ERROR;
  }

  if (osapiSemaTake(mcastGblVariables_g.mcastMapAdminScopeSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\n MCASTMAP : Failed to take admin-scope semaphore \n");
    return L7_FAILURE;
  }
  
  if (mcastGblVariables_g.mcastMapCfgData.rtr.numAdminScopeEntries > 0)
  {
    useIndex = L7_MCAST_MAX_ADMINSCOPE_ENTRIES;
    for (itr = 0; itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES; itr++)
    {
      if (mcastGblVariables_g.mcastMapCfgData.rtr.
                           mcastAdminScopeCfgData[itr].inUse == L7_TRUE)
      {
        entriesChecked++;
        if ((nimIntIfFromConfigIDGet(&mcastGblVariables_g.
                                     mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr].
             ifConfigId, &tmpIfNum) == L7_SUCCESS) && 
            (tmpIfNum == intIfNum))
        {
          /* if boundary already exists on interface then return */
          if ((L7_INET_IS_ADDR_EQUAL(&(mcastGblVariables_g.mcastMapCfgData.rtr.
                                       mcastAdminScopeCfgData[itr].groupIpAddr), grpAddr) &&
               L7_INET_IS_ADDR_EQUAL(&(mcastGblVariables_g.mcastMapCfgData.rtr.
                                       mcastAdminScopeCfgData[itr].groupIpMask), grpMask)) == L7_TRUE)
          {
            MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, 
                            "Boundary address already exists.\n");

            osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
            return L7_ALREADY_CONFIGURED;
          }
        }
      }
      else
      {
        /* remember the first not in use entry in the array */
        useIndex = (useIndex == L7_MCAST_MAX_ADMINSCOPE_ENTRIES)? itr: useIndex;
      }

      /*  If all configured entries were checked 
       * for already existing boundaries then break.
       * Reduce the no. of iterations.  */

      if (entriesChecked == mcastGblVariables_g.mcastMapCfgData.rtr.
                                                    numAdminScopeEntries)
      {
        if (useIndex == L7_MCAST_MAX_ADMINSCOPE_ENTRIES)
        {
          useIndex = itr+1;   /* next index is not in use */
        }
        break;
      }
    }
  }

  if ((mcastGblVariables_g.mcastMapCfgData.rtr.numAdminScopeEntries >= L7_MCAST_MAX_ADMINSCOPE_ENTRIES) ||
      (useIndex >= L7_MCAST_MAX_ADMINSCOPE_ENTRIES))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Max number of admin boundaries added.\n");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"Maximum adminscope entries reached");     
    osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
    return L7_TABLE_IS_FULL;
  }

  inetCopy(&(mcastGblVariables_g.mcastMapCfgData.rtr.
             mcastAdminScopeCfgData[useIndex].groupIpAddr), grpAddr);
  inetCopy(&(mcastGblVariables_g.mcastMapCfgData.rtr.
             mcastAdminScopeCfgData[useIndex].groupIpMask), grpMask);

  if (nimConfigIdGet(intIfNum, &ifConfigId) == L7_SUCCESS)
  {
    NIM_CONFIG_ID_COPY(&mcastGblVariables_g.mcastMapCfgData.rtr.
              mcastAdminScopeCfgData[useIndex].ifConfigId , &ifConfigId);
  }

  mcastGblVariables_g.mcastMapCfgData.rtr.
                    mcastAdminScopeCfgData[useIndex].inUse = L7_TRUE;
  mcastGblVariables_g.mcastMapCfgData.rtr.numAdminScopeEntries++;
  mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged = L7_TRUE;

  memset(&adminScopeEventParms, 0, sizeof(mcastMapAdminScopeEventParms_t));

  inetCopy(&adminScopeEventParms.grpAddr, grpAddr);
  inetCopy(&adminScopeEventParms.grpMask, grpMask);
  adminScopeEventParms.intIfNum = intIfNum;
  adminScopeEventParms.mode = L7_TRUE;

  if (mcastMapConfigMsgQueue(MCASTMAP_ADMIN_SCOPE_BOUNDARY_SET_EVENT,
                             (L7_VOIDPTR)&adminScopeEventParms,
                             sizeof(mcastMapAdminScopeEventParms_t)) 
      != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Failed to send admin scope boundary event to mcastMap queue ");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to send (config) admin scope boundary change "
             "event to mcastMap task\n");
    osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
    return L7_FAILURE;
  }
  osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
  return L7_SUCCESS;
}

/**********************************************************************
 * @purpose  Deletes an entry from the Admin Scope Boundary List for
 *           the given interface
 *
 * @param    intIfNum        @b{(input)}     Internal Interface Number
* @param    grpAddr         @b{(input)}  Group IP Address
* @param    grpMask         @b{(input)}  Group IP Address Mask
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
* @comments
 *
 * @end
 **********************************************************************/
L7_RC_t mcastMapAdminScopeBoundaryReset(L7_uint32 intIfNum,
                                        L7_inet_addr_t *grpAddr,
                                        L7_inet_addr_t *grpMask)
{
  L7_uint32 itr, tmpIfNum, entriesChecked = 0;
  mcastMapAdminScopeEventParms_t adminScopeEventParms;
  L7_uchar8   addrBuffStr[IPV6_DISP_ADDR_LEN];

  if ((inetIsAddressZero(grpAddr) == L7_TRUE) || 
      (inetIsAddressZero(grpMask) == L7_TRUE))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: NULL grp address and mask for admin scope reset\n");
    return L7_FAILURE;
  }

  if (osapiSemaTake(mcastGblVariables_g.mcastMapAdminScopeSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\n MCASTMAP : Failed to take admin-scope semaphore \n");
    return L7_FAILURE;
  }
  for (itr = 0; 
      (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.
       numAdminScopeEntries) &&
      (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES); 
      itr++)
  {
    if (mcastGblVariables_g.mcastMapCfgData.rtr.
        mcastAdminScopeCfgData[itr].inUse == L7_TRUE)
    {
      entriesChecked++;
      if ((nimIntIfFromConfigIDGet(&mcastGblVariables_g.mcastMapCfgData.rtr.
                                   mcastAdminScopeCfgData[itr].ifConfigId, &tmpIfNum) == L7_SUCCESS) && 
          (tmpIfNum == intIfNum))
      {
        /* configured boundary found */
        if (L7_INET_IS_ADDR_EQUAL(&(mcastGblVariables_g.mcastMapCfgData.rtr.
                                    mcastAdminScopeCfgData[itr].groupIpAddr), grpAddr) &&
            L7_INET_IS_ADDR_EQUAL(&(mcastGblVariables_g.mcastMapCfgData.rtr.
                                    mcastAdminScopeCfgData[itr].groupIpMask), grpMask))
        {
          inetAddressZeroSet(L7_AF_INET, &(mcastGblVariables_g.mcastMapCfgData.rtr.
                             mcastAdminScopeCfgData[itr].groupIpAddr));
          inetAddressZeroSet(L7_AF_INET, &(mcastGblVariables_g.mcastMapCfgData.rtr.
                            mcastAdminScopeCfgData[itr].groupIpMask));
          memset((void *)&(mcastGblVariables_g.mcastMapCfgData.rtr.
              mcastStaticRtsCfgData[itr].ifConfigId), 0, sizeof(nimConfigID_t));
          mcastGblVariables_g.mcastMapCfgData.rtr.
                            mcastAdminScopeCfgData[itr].inUse = L7_FALSE;
          mcastGblVariables_g.mcastMapCfgData.rtr.numAdminScopeEntries--;
          mcastGblVariables_g.mcastMapCfgData.cfgHdr.dataChanged = L7_TRUE;
          osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
          return mcastMapAdminScopeBoundaryDeleteApply(intIfNum, grpAddr, grpMask);
        }
      }
    }
  }
  osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: boundary entry not found for intIfNum = %d ,grp = %s ,"
          "mask = %s \n",intIfNum,
          inetAddrPrint(grpAddr, addrBuffStr), inetAddrPrint(grpMask,addrBuffStr)); 
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                  " adminscope entry doesnot exist for intIfNum = %d" ,intIfNum);  
  MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FAILURES," group Addr : ",grpAddr);  
  MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FAILURES," group Mask : ",grpMask); 


  memset(&adminScopeEventParms, 0, sizeof(mcastMapAdminScopeEventParms_t));

  inetCopy(&adminScopeEventParms.grpAddr, grpAddr);
  inetCopy(&adminScopeEventParms.grpMask, grpMask);
  adminScopeEventParms.intIfNum = intIfNum;
  adminScopeEventParms.mode = L7_FALSE;

  if (mcastMapConfigMsgQueue(MCASTMAP_ADMIN_SCOPE_BOUNDARY_SET_EVENT,
                             (L7_VOIDPTR)&adminScopeEventParms,
                             sizeof(mcastMapAdminScopeEventParms_t)) 
      != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Failed to send admin scope boundary event to mcastMap queue ");
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to send (config) admin scope change event"
            "to mcastMap task\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Checks whether the admin scope entry exists for the
 *           given interface
 *
 * @param    intIfNum        @b{(input)}     Internal Interface Number
* @param    grpAddr         @b{(input)}  Group IP Address
* @param    grpMask         @b{(input)}  Group IP Address Mask
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
* @comments    
 *
 * @end
 **********************************************************************/
L7_RC_t mcastMapAdminScopeBoundaryEntryGet(L7_uint32 intIfNum,
                                       L7_inet_addr_t *grpAddr,
                                       L7_inet_addr_t *grpMask)
{
  L7_uint32 itr, tmpIfNum, entriesChecked = 0;
  L7_uchar8 addrBuffStr[IPV6_DISP_ADDR_LEN];
  L7_mcastMapAdminScopeCfgData_t *scope;

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (osapiSemaTake(mcastGblVariables_g.mcastMapAdminScopeSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\n MCASTMAP : Failed to take admin-scope semaphore \n");
    return L7_FAILURE;
  }
  for (itr = 0; 
      (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.
       numAdminScopeEntries) &&
      (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES); 
      itr++)
  {
    scope = &mcastGblVariables_g.mcastMapCfgData.rtr.
                                              mcastAdminScopeCfgData[itr];
    if (scope->inUse == L7_TRUE)
    {
      entriesChecked++;
      if ((nimIntIfFromConfigIDGet(&scope->ifConfigId, 
                                   &tmpIfNum)== L7_SUCCESS) &&
          (tmpIfNum == intIfNum))
      {
        if (L7_INET_IS_ADDR_EQUAL(&(scope->groupIpAddr), grpAddr) &&
            L7_INET_IS_ADDR_EQUAL(&(scope->groupIpMask), grpMask))
        {
          /* configured boundary found */
          osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
          return L7_SUCCESS;
        }
      }
    }
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: boundary entry not found for intIfNum = %d ,grp = %s ,"
          "mask = %s \n",intIfNum,
          inetAddrPrint(grpAddr, addrBuffStr), inetAddrPrint(grpMask,addrBuffStr));       
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                  " adminscope entry doesnot exist for intIfNum = %d" ,intIfNum);  
  MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FAILURES," group Addr : ",grpAddr);  
  MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_FAILURES," group Mask : ",grpMask);
  osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets difference between two admin scope boundary entries
*
* @param    scope1         @b{(input)}  Admin scope entry
* @param    scope2         @b{(input)}  Admin scope entry
*
* @returns   L7_int32
*
* @comments  The difference between two admin scope boundary entries in order 
*            of (interface, group-range, group-mask)
*
* @end
**********************************************************************/
L7_int32 mcastMapIsScopeLess(L7_mcastMapAdminScopeCfgData_t *scope1,
                    L7_mcastMapAdminScopeCfgData_t *scope2)
{
  L7_uint32 intIfNum1 = 0, intIfNum2 = 0;

  if ((nimIntIfFromConfigIDGet(&scope1->ifConfigId, 
                                   &intIfNum1) == L7_SUCCESS))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                    "Failed to get intIfNum1 for configId = %d",
                     scope1->ifConfigId);
  }

  if ((nimIntIfFromConfigIDGet(&scope2->ifConfigId, 
                                   &intIfNum2) == L7_SUCCESS))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                    "Failed to get intIfNum1 for configId = %d",
                    scope2->ifConfigId);
  }  

  if (intIfNum1 < intIfNum2)
  {
    return (intIfNum1 - intIfNum2);
  }
  if ((intIfNum1 == intIfNum2) &&
      (L7_INET_ADDR_COMPARE(&scope1->groupIpAddr, &scope2->groupIpAddr) != 0))
  {
    return L7_INET_ADDR_COMPARE(&scope1->groupIpAddr, &scope2->groupIpAddr);
  }
  if ((intIfNum1 == intIfNum2) &&
      (L7_INET_ADDR_COMPARE(&scope1->groupIpAddr, &scope2->groupIpAddr) == 0))
      
  {
    return L7_INET_ADDR_COMPARE(&scope1->groupIpMask, &scope2->groupIpMask) ;
  }
  return 1;
}
/*********************************************************************
 * @purpose  Gets the next entry in the Admin Scope Boundary list
 *           for the given interface
 *
* @param    intIfNum        @b{(inout)}  Internal Interface Number
* @param    grpAddr         @b{(inout)}  Group IP Address
* @param    grpMask         @b{(inout)}  Group IP Address Mask
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
* @comments    
 *
 * @end
 **********************************************************************/
L7_RC_t mcastMapAdminScopeBoundaryEntryNextGet(L7_uint32 *intIfNum,
                                           L7_inet_addr_t *grpAddr,
                                           L7_inet_addr_t *grpMask)
{
  L7_uint32 itr,entriesChecked=0, tmpIntIfNum;
  L7_mcastMapAdminScopeCfgData_t givenScope, *itrScope, nextLessScope;
  L7_RC_t rc = L7_FAILURE;

  memset(&givenScope, 0, sizeof(L7_mcastMapAdminScopeCfgData_t));
  memset(&nextLessScope, 0, sizeof(L7_mcastMapAdminScopeCfgData_t));
  inetCopy(&givenScope.groupIpAddr, grpAddr);
  inetCopy(&givenScope.groupIpMask, grpMask);
  if ((nimConfigIdGet(*intIfNum, &givenScope.ifConfigId) != L7_SUCCESS))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                  " Failed to get config Id for given intIfNum = %d", intIfNum);
  }

  if (osapiSemaTake(mcastGblVariables_g.mcastMapAdminScopeSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\n MCASTMAP : Failed to take admin-scope semaphore \n");
    return L7_FAILURE;
  }
  
  for (itr = 0; (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.
                                                    numAdminScopeEntries) &&
      (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES); itr++)
  {
    if (mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr].inUse == L7_TRUE)
    {
      nextLessScope = mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr];
      break;
    }
  }
  

  for (itr = 0; (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.
                                                    numAdminScopeEntries) &&
      (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES); itr++)
  {
    if (mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr].inUse == L7_FALSE)
    {
      continue;
    }
    entriesChecked++;
    itrScope = &mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr];
    if (mcastMapIsScopeLess(&givenScope, &nextLessScope) >= 0)
    {
      nextLessScope = mcastGblVariables_g.mcastMapCfgData.rtr.mcastAdminScopeCfgData[itr];
    }

    if ( (mcastMapIsScopeLess(&givenScope, itrScope) < 0))
    {
      if(mcastMapIsScopeLess(itrScope,&nextLessScope) <= 0)
      {
        nextLessScope = *itrScope;
        rc = L7_SUCCESS;
      }  
    }    
  }

  if (rc == L7_SUCCESS)
  {
    inetCopy(grpAddr, &nextLessScope.groupIpAddr);
    inetCopy(grpMask, &nextLessScope.groupIpMask);
    if ((nimIntIfFromConfigIDGet(&nextLessScope.ifConfigId, 
                                   &tmpIntIfNum) == L7_SUCCESS))
    {
      *intIfNum = tmpIntIfNum;
    }
    osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
    return L7_SUCCESS;
  }
  osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieve group information learned by MGMD
*
* @param    familyType  @b{(input)} Address Family type
* @param    rtrIfNum    @b{(input)} Router Interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This API is used by MRPs to get the group information learned 
*           by MGMD whenever MRP is disabled and enabled at run-time
*
* @end
*********************************************************************/
L7_RC_t mcastMapMRPAdminScopeInfoGet(L7_uchar8 familyType, L7_uint32 rtrIfNum)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "\nFunction Entered\n");

  if (familyType != L7_AF_INET)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Invalid FamilyType - %d", familyType);
    return L7_FAILURE;
  }
  if ((rtrIfNum <= 0) || (rtrIfNum > MCAST_MAX_INTERFACES))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid rtrIfNum - %d", rtrIfNum);
    return L7_FAILURE;
  }

  if (mcastMapMRPEventMsgSend (MCAST_EVENT_ADMIN_SCOPE_INFO_GET,
                              (void*) &rtrIfNum, sizeof (L7_uint32))
                             != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Mcast Map Event Post Failed for "
                    "eventType - %d.", MCAST_EVENT_ADMIN_SCOPE_INFO_GET);
    return L7_FAILURE;
  }

  MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_APIS, "Function Exit.");
  return L7_SUCCESS;
}


/*********************************************************************
*                 API FUNCTIONS  -  PARAMS FROM MROUTE TABLE TO USER
*********************************************************************/

/**********************************************************************
* @purpose  Gets PIM specific flags related to a multicast state entry
*
* @param    ipMRtGrp       @b{(input)} mroute group
* @param    ipMRtSrc       @b{(input)} mroute source
* @param    ipMRtSrcMask   @b{(input)} mroute mask
* @param    flag           @b{(output)} flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  Flag is '0' for RPT (Shared tree) and '1' for SPT(Source Tree)
*
* @end
*********************************************************************/
L7_RC_t mcastMapIpMRouteFlagsGet(L7_uchar8 family,
                                 L7_inet_addr_t *ipMRtGrp, 
                                 L7_inet_addr_t *ipMRtSrc,
                                 L7_inet_addr_t *ipMRtSrcMask, 
                                 L7_uint32 *flag)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    return pimdmMapIpMRouteFlagsGet(family, ipMRtGrp, ipMRtSrc, 
                                    ipMRtSrcMask, flag);
  }
  if (pimsmMapPimsmIsOperational(family) == L7_TRUE)
  {
    return pimsmMapIpMRouteFlagsGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, flag);
  }
  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    if (family != L7_AF_INET)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "No support for IPv6 DVMRP.\n");
      return L7_FAILURE;
    }
    return dvmrpMapIpMRouteFlagsGet(ipMRtGrp, ipMRtSrc,
                                    ipMRtSrcMask, flag);
  }

  if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get Mroute flags.\n");
  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Gets the address of the upstream neighbor from which
*           IP datagrams from these sources to this multicast
*           address are received.
*
* @param    ipMRtGrp     @b{(input)} mroute group
* @param    ipMRtSrc     @b{(input)} mroute source
* @param    inMRtSrcMask @b{(input)} mroute mask
* @param    upstmNbr     @b{(output)} upstream neighbor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments returns 0 if upstream neighbor is unknown.Both upstream and
*           Rpf interface are same.
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteUpstreamNbrGet(L7_uchar8 family,
                                       L7_inet_addr_t *ipMRtGrp,
                                       L7_inet_addr_t *ipMRtSrc,
                                       L7_inet_addr_t *ipMRtSrcMask,
                                       L7_inet_addr_t *upstmNbr)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    return pimdmMapIpMRouteRpfAddrGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, upstmNbr);
  }

  if (pimsmMapPimsmIsOperational(family) == L7_TRUE)
  {
    return pimsmMapIpMRouteRpfAddrGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, upstmNbr);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    return dvmrpMapIpMRouteRpfAddrGet(ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, upstmNbr);
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute upstream neighbor\n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the interface index on which IP datagrams sent by
*           these sources to this multicast address are received.
*           corresponding to the index received.
*
* @param    ipMRtGrp       @b{(input)}  Multicast Group address
* @param    ipMRouteSrc    @b{(input)}  Source address
* @param    ipMRtSrcMask   @b{(input)}  Mask Address
* @param    intIfNum       @b{(output)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments A value of 0 indicates that datagrams are not subject
*           to an incoming interface check, but may be accepted on
*           multiple interfaces.
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteIfIndexGet(L7_uchar8 family,
                                   L7_inet_addr_t *ipMRtGrp,
                                   L7_inet_addr_t *ipMRtSrc,
                                   L7_inet_addr_t *ipMRtSrcMask,
                                   L7_uint32      *intIfNum)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    return pimdmMapRouteIfIndexGet(family, ipMRtGrp, 
                                   ipMRtSrc, ipMRtSrcMask, intIfNum);
  }

  if (pimsmMapPimsmIsOperational(family) == L7_TRUE)
  {
    return pimsmMapIpMRouteIfIndexGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, intIfNum);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    if (family != L7_AF_INET)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "No support for IPv6 DVMRP.\n");
      return L7_FAILURE;
    }
    return dvmrpMapRouteIfIndexGet(ipMRtSrc, ipMRtSrcMask, intIfNum);
  }

  if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
  {
    return mgmdMapProxyIpMRouteIfIndexGet(family, ipMRtGrp, 
                                         ipMRtSrc, ipMRtSrcMask, intIfNum);
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute if-index.\n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the time since the multicast routing information
*           represented by this entry was learned by the router.
*
* @param    ipMRtGrp      @b{(input)} Multicast Group address
* @param    ipMRouteSrc   @b{(input)} Source address
* @param    ipMRtSrcMask  @b{(input)} Mask Address
* @param    upTime        @b{(output)} uptime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteUpTimeGet(L7_uchar8 family,
                                  L7_inet_addr_t *ipMRtGrp,
                                  L7_inet_addr_t *ipMRtSrc,
                                  L7_inet_addr_t *ipMRtSrcMask,
                                  L7_uint32      *upTime)
{
  L7_ulong32 dvmrpUpTime = L7_NULL;
  L7_RC_t rc = L7_FAILURE;

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    return pimdmMapRouteUptimeGet(family, ipMRtGrp, 
                                  ipMRtSrc, ipMRtSrcMask, upTime);
  }
  if (pimsmMapPimsmIsOperational(family) == L7_TRUE)
  {
    return pimsmMapIpMRouteUpTimeGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, upTime);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    if (family != L7_AF_INET)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "No support for IPv6 DVMRP.\n");
      return rc;
    }
    rc = dvmrpMapRouteUptimeGet(ipMRtSrc, ipMRtSrcMask, &dvmrpUpTime);
    *upTime = (L7_uint32) dvmrpUpTime;
    return rc;
  }

  if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
  {
    return mgmdMapProxyIpMRouteUpTimeGet(family, ipMRtGrp, 
                                         ipMRtSrc, ipMRtSrcMask, upTime);
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute up-time\n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets the minimum amount of time remaining before  this
*           entry will be aged out.
*
* @param    ipMRtGrp       @b{(input)} Multicast Group address
* @param    ipMRouteSrc    @b{(input)} Source address
* @param    ipMRtSrcMask   @b{(input)} Mask Address
* @param    expire         @b{(output)} expire time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The value of 0 indicates that the entry is not
*           subject to aging.
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteExpiryTimeGet(L7_uchar8 family,
                                      L7_inet_addr_t *ipMRtGrp,
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask,
                                      L7_uint32       *expire)
{
  L7_RC_t rc = L7_FAILURE;

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    return pimdmMapRouteExpiryTimeGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, expire);
  }
  if (pimsmMapPimsmIsOperational(family) == L7_TRUE)
  {
    return pimsmMapIpMRouteExpiryTimeGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, expire);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    if (family != L7_AF_INET)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "No support for IPv6 DVMRP.\n");
      return L7_FAILURE;
    }
    rc =  dvmrpMapIpMRouteExpiryTimeGet(ipMRtGrp,ipMRtSrc, ipMRtSrcMask, expire);            
    return rc;
  }

  if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
  {
    return mgmdMapProxyIpMRouteExpiryTimeGet(family, ipMRtGrp, 
                                         ipMRtSrc, ipMRtSrcMask, expire);
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to egt mroute expiry time\n");
  return L7_FAILURE;
}

/**********************************************************************
* @purpose  Gets multicast protocol routing the mroute entry
*
* @param    ipMRtGrp       @b{(input)} Multicast Group address
* @param    ipMRouteSrc    @b{(input)} Source address
* @param    ipMRtSrcMask   @b{(input)} Mask Address
* @param    protocol       @b{(output)} protocol value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The multicast routing protocol via which this multicast
*           forwarding entry was learned.
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteProtocolGet(L7_uchar8 family,
                                    L7_inet_addr_t *ipMRtGrp,
                                    L7_inet_addr_t *ipMRouteSrc,
                                    L7_inet_addr_t *ipMRtSrcMask,
                                    L7_uint32      *protocol)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (pimsmMapPimsmIsOperational(family) == L7_TRUE)
  {
    *protocol = L7_MCAST_IANA_MROUTE_PIM_SM;
    return L7_SUCCESS;
  }

  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    *protocol = L7_MCAST_IANA_MROUTE_PIM_DM;
    return L7_SUCCESS;
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    *protocol = L7_MCAST_IANA_MROUTE_DVMRP;
    return L7_SUCCESS;
  }

  if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
  {
    *protocol = L7_MCAST_IANA_MROUTE_IGMP_PROXY;
    return L7_SUCCESS;
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute protocol\n");
  return L7_FAILURE;
}

/**********************************************************************
* @purpose  Gets the routing protocol used to find the upstream or
*           parent interface.
*
* @param    ipMRtGrp        @b{(input)} mcast group
* @param    ipMRtSrc        @b{(input)} mcast source 
* @param    inMRtSrcMask    @b{(input)} mcast mask
* @param    protocol        @b{(output)} protocol value
*
* @returns     L7_SUCCESS
* @returns     L7_FAILURE
*
* @comments
*
* @end
********************************************************************/
L7_RC_t mcastMapIpMRouteRtProtoGet(L7_inet_addr_t *ipMRtGrp,
                                   L7_inet_addr_t *ipMRtSrc,
                                   L7_inet_addr_t *ipMRtSrcMask,
                                   L7_uint32      *protocol)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
   if(dvmrpMapIpMRouteEntryGet(ipMRtGrp,ipMRtSrc,ipMRtSrcMask)==L7_SUCCESS)
     {
       *protocol= L7_MCAST_MROUTE_RTPROTO_DVMRP;
       return L7_SUCCESS;
     }
  }

  if (pimsmMapPimsmIsOperational(L7_AF_INET) == L7_TRUE)
  {
    return pimsmMapIpMRouteRtProtoGet(L7_AF_INET, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, protocol);
  }

  if (pimdmMapPimDmIsOperational(L7_AF_INET) == L7_TRUE)
  {
    return pimdmMapIpMRouteRtProtoGet(L7_AF_INET, ipMRtGrp, 
                                     ipMRtSrc, ipMRtSrcMask, protocol);
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute protocol type\n");
  return L7_FAILURE;
}

/**********************************************************************
* @purpose  Gets the address field of the route used to find the
*           upstream or parent interface
*
* @param    ipMRtGrp         @b{(input)} mcast group
* @param    ipMRtSrc         @b{(input)} mcast source
* @param    ipMRtSrcMask     @b{(input)} mcast mask
* @param    rtAddr           @b{(output)} route address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteRtAddressGet(L7_inet_addr_t *ipMRtGrp,
                                     L7_inet_addr_t *ipMRtSrc,
                                     L7_inet_addr_t *ipMRtSrcMask,
                                     L7_inet_addr_t *rtAddr)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (pimdmMapPimDmIsOperational(L7_AF_INET) == L7_TRUE)
  {
    return pimdmMapIpMRouteRtAddressGet(L7_AF_INET, ipMRtGrp, 
                                        ipMRtSrc, ipMRtSrcMask, rtAddr);
  }
  if (pimsmMapPimsmIsOperational(L7_AF_INET) == L7_TRUE)
  {
    return pimsmMapIpMRouteRtAddressGet(L7_AF_INET, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, rtAddr);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    return dvmrpMapIpMRouteRtAddressGet(ipMRtGrp, 
                                        ipMRtSrc, ipMRtSrcMask, rtAddr);
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute route-address \n");
  return L7_FAILURE;
}

/**********************************************************************
* @purpose  Gets the mask associated with the route used to find the
*           upstream or parent interface.
*
* @param    ipMRtGrp        @b{(input)} mcast group address
* @param    ipMRtSrc        @b{(input)} mcast source
* @param    ipMRtSrcMask    @b{(input)} mcast source mask
* @param    rtMask          @b{(output)} mroute mask
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments    none
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteRtMaskGet(L7_inet_addr_t *ipMRtGrp,
                                  L7_inet_addr_t *ipMRtSrc,
                                  L7_inet_addr_t *ipMRtSrcMask,
                                  L7_inet_addr_t *rtMask)
{
  if (pimdmMapPimDmIsOperational(L7_AF_INET) == L7_TRUE)
  {
    return pimdmMapIpMRouteRtMaskGet(L7_AF_INET, ipMRtGrp, 
                                     ipMRtSrc, ipMRtSrcMask, rtMask);
  }
  if (pimsmMapPimsmIsOperational(L7_AF_INET) == L7_TRUE)
  {
    return pimsmMapIpMRouteRtMaskGet(L7_AF_INET, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, rtMask);
  }
  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    return dvmrpMapIpMRouteRtMaskGet(ipMRtGrp,
                                     ipMRtSrc, ipMRtSrcMask, rtMask);
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute route-mask\n");
  return L7_FAILURE;
}

/**********************************************************************
* @purpose  Gets the type of route used (unicast or multicast)
*
* @param    ipMRtGrp        @b{(input)} mcast group address
* @param    ipMRtSrc        @b{(input)} mcast source
* @param    ipMRtSrcMask    @b{(input)} source mask
* @param    rtType          @b{(output)} route type
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments       The reason the given route was placed in the MRIB.
*
* @end
*********************************************************************/
L7_RC_t mcastMapIpMRouteRtTypeGet(L7_inet_addr_t *ipMRtGrp,
                                  L7_inet_addr_t *ipMRtSrc,
                                  L7_inet_addr_t *ipMRtSrcMask,
                                  L7_uint32      *rtType)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  if (pimdmMapPimDmIsOperational(L7_AF_INET) == L7_TRUE)
  {
    return pimdmMapIpMRouteRtTypeGet(L7_AF_INET, ipMRtGrp, 
                                     ipMRtSrc, ipMRtSrcMask, rtType);
  }

  if (pimsmMapPimsmIsOperational(L7_AF_INET) == L7_TRUE)
  {
    return pimsmMapIpMRouteRtTypeGet(L7_AF_INET, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, rtType);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    return dvmrpMapIpMRouteRtTypeGet(ipMRtGrp,
                                     ipMRtSrc, ipMRtSrcMask, rtType);
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute route-type\n");
  return L7_FAILURE;
}
/**********************************************************************
* @purpose  Gets the Rpf address for the given index
*
* @param    ipMRtGrp        @b{(input)} mcast group address
* @param    ipMRtSrc        @b{(input)} mcast source
* @param    ipMRtSrcMask    @b{(input)} source mask
* @param    rpfAddr         @b{(output)} RPF address
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapIpMRouteRpfAddressGet(L7_uchar8 family,
                                      L7_inet_addr_t *ipMRtGrp,
                                      L7_inet_addr_t *ipMRtSrc,
                                      L7_inet_addr_t *ipMRtSrcMask,
                                      L7_inet_addr_t *rpfAddr)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    return pimdmMapIpMRouteRpfAddrGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, rpfAddr);
  }

  if (pimsmMapPimsmIsOperational(family) == L7_TRUE)
  {
    return pimsmMapIpMRouteRpfAddrGet(family, ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, rpfAddr);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    if (family != L7_AF_INET)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "No support for IPv6 DVMRP.\n");
      return L7_FAILURE;
    }
    return dvmrpMapIpMRouteRpfAddrGet(ipMRtGrp, 
                                      ipMRtSrc, ipMRtSrcMask, rpfAddr);
  }

  if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
  {
    return mgmdMapProxyIpMRouteRpfAddrGet(family, ipMRtGrp, 
                                         ipMRtSrc, ipMRtSrcMask, rpfAddr);
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute RPF address\n");
  return L7_FAILURE;
}

/**********************************************************************
* @purpose  Get the IP Multicast route table  entry count
*
* @param    count      @b{(output)} Number of Entires in Mroute Table
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
**********************************************************************/
L7_RC_t mcastMapIpMRouteEntryCountGet(L7_uchar8 family, L7_uint32 *count)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if(mfcEntryCountGet(count) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP:Failed to get multicast forwarding cache count\n");
    return L7_FAILURE;
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute entry count\n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the IP Multicast route table  Highest entry count
*
* @param    count          @b{(output)} highest entry count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteHighestEntryCountGet(L7_uint32 *count)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  if (pimdmMapPimDmIsOperational(L7_AF_INET) == L7_TRUE)
  {
    return pimdmMapIpMRouteHighestEntryCountGet(L7_AF_INET, count);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    return dvmrpMapIpMRouteHighestEntryCountGet(count);
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute highest entry count\n");
  return L7_FAILURE;
}

/**********************************************************************
* @purpose  Get the IP Multicast route table  entry
*
* @param    ipMRtGrp      @b{(input)} Multicast Group address
* @param    ipMRtSrc     @b{(input)} Source address
* @param    ipMRtSrcMask @b{(input)} Mask Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteEntryGet(L7_uchar8 family,
                                 L7_inet_addr_t *ipMRtGrp,
                                 L7_inet_addr_t *ipMRtSrc,
                                 L7_inet_addr_t *ipMRtSrcMask)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if ((ipMRtSrcMask != L7_NULL) &&
      (inetIsMaskValid(ipMRtSrcMask) != L7_TRUE))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Invalid Input parameters.\n");
    return L7_FAILURE;
  }

  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    return pimdmMapIpMRouteEntryGet(family, 
                                    ipMRtGrp, ipMRtSrc, ipMRtSrcMask);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    return dvmrpMapIpMRouteEntryGet(ipMRtGrp, ipMRtSrc, ipMRtSrcMask);
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute entry\n");
  return L7_FAILURE;
}

/**********************************************************************
* @purpose  Get the IP Multicast route table Next entry
*
* @param    ipMRtGrp           @b{(inout)} mcast group
* @param    ipMRtSrc           @b{(inout)} mcast source
* @param    ipMRtSrcMask       @b{(inout)} source mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteEntryNextGet(L7_uchar8 family,
                                     L7_inet_addr_t *ipMRtGrp,
                                     L7_inet_addr_t *ipMRtSrc,
                                     L7_inet_addr_t *ipMRtSrcMask)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    return pimdmMapIpMRouteEntryNextGet(family, 
                                        ipMRtGrp, ipMRtSrc, ipMRtSrcMask);
  }
  if (pimsmMapPimsmIsOperational(family) == L7_TRUE)
  {
    return pimsmMapIpMRouteEntryNextGet(family,
                                        ipMRtGrp, ipMRtSrc, ipMRtSrcMask);

  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    if (family != L7_AF_INET)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "No support for IPv6 DVMRP.\n");
      return L7_FAILURE;
    }
    return dvmrpMapIpMRouteEntryNextGet(ipMRtGrp, ipMRtSrc, ipMRtSrcMask);
  }

  if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
  {
    return mgmdMapProxyIpMRouteEntryNextGet(family, 
                                            ipMRtGrp, ipMRtSrc, ipMRtSrcMask);
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute next entry\n");
  return L7_FAILURE;
}
/**********************************************************************
* @purpose  Get the IP Multicast route table  Maximum Size
*
* @param    size       @b{(output)} route table size
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteTableMaxSizeGet(L7_uint32 *size)
{
  L7_MCAST_IANA_PROTO_ID_t currentMcastProtocol = L7_MCAST_IANA_MROUTE_UNASSIGNED;

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (mcastMapIpCurrentMcastProtocolGet (L7_AF_INET, &currentMcastProtocol)
                                      != L7_SUCCESS)
  {
    *size = L7_MULTICAST_FIB_MAX_ENTRIES;
    return L7_SUCCESS;
  }

  switch (currentMcastProtocol)
  {
    case L7_MCAST_IANA_MROUTE_DVMRP:
      *size = L7_DVMRP_MAX_MRT_IPV4_TABLE_SIZE;
      break;

    default:
      *size = L7_MULTICAST_FIB_MAX_ENTRIES;
      break;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the multicast protocol running on the interface
*
* @param    intfNum      @b{(input)} interface number
* @param    protocol     @b{(output)} protocol value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteInterfaceProtocolGet(L7_uint32 intIfNum,
                                             L7_uint32 *protocol)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (pimsmMapIntfIsOperational(L7_AF_INET, intIfNum) == L7_TRUE)
  {
    *protocol = L7_MCAST_IANA_MROUTE_PIM_SM;
  }

  else if (pimdmMapInterfaceIsOperational(L7_AF_INET, intIfNum) == L7_TRUE)
  {
    *protocol = L7_MCAST_IANA_MROUTE_PIM_DM;
  }

  else if (dvmrpMapIntfIsOperational(intIfNum) == L7_TRUE)
  {
    *protocol = L7_MCAST_IANA_MROUTE_DVMRP;
  }
  else
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute protocol for"
            "intIfNum = %d \n", intIfNum);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the interface entry in the routing table.
*
* @param    intfNum       @b{(input)} interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapIpMRouteIntfEntryGet(L7_uint32 intIfNum)
{
  L7_uint32 rtrIfNum = L7_NULL;

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (ipMapIntIfNumToRtrIntf(intIfNum, &rtrIfNum) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  else
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to get interface entry as conversion to "
            "rtrIfNum for intIfNum = %d failed \n", intIfNum);
    return L7_FAILURE;
  }
}

/******************************************************************
* @purpose  Gets the next interface entry in the routing table.
*
* @param    intfNum   @b{(inout)} interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*****************************************************************/
L7_RC_t mcastMapIpMRouteIntfEntryNextGet(L7_uint32 *intIfNum)
{
  L7_uint32 rtrIfNum = L7_NULL;
  L7_uint32 tmpIntIfNum = L7_NULL;

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  for (tmpIntIfNum = *intIfNum+1; 
      tmpIntIfNum <= L7_MAX_INTERFACE_COUNT; tmpIntIfNum++)
  {
    if (ipMapIntIfNumToRtrIntf(tmpIntIfNum, &rtrIfNum) == L7_SUCCESS)
    {
      *intIfNum = tmpIntIfNum;
      return L7_SUCCESS;
    }
  }
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Failed to get next interface entry as conversion to "
            "rtrIfNum for intIfNum = %d failed \n", intIfNum);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    ipMRtGrp           @b{(inout)} mcast group
* @param    ipMRtSrc           @b{(inout)} mcast source
* @param    ipMRtSrcMask       @b{(inout)} source mask
* @param    intIfNum           @b{(inout)} interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*****************************************************************/
L7_RC_t mcastMapIpMRouteOutIntfEntryNextGet(L7_uchar8 family,
                                            L7_inet_addr_t *ipMRtGrp,
                                            L7_inet_addr_t *ipMRtSrc,
                                            L7_inet_addr_t *ipMRtSrcMask,
                                            L7_uint32 *intIfNum)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  if ((ipMRtGrp == L7_NULLPTR) && (ipMRtSrc == L7_NULLPTR) &&
      (ipMRtSrcMask == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Invalid Input parameters.\n");
    return L7_FAILURE;
  }

  if (pimdmMapPimDmIsOperational(family) == L7_TRUE)
  {
    return pimdmMapRouteOutIntfEntryNextGet(family, ipMRtGrp, 
                                            ipMRtSrc, ipMRtSrcMask, intIfNum);
  }

  if (dvmrpMapDvmrpIsOperational() == L7_TRUE)
  {
    if (family != L7_AF_INET)
    {
      MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "No support for IPv6 DVMRP.\n");
      return L7_FAILURE;
    }
    return dvmrpMapRouteOutIntfEntryNextGet(ipMRtGrp, ipMRtSrc, 
                                            ipMRtSrcMask, intIfNum);
  }

  if (mgmdMapProxyChkIsEnabled(family) == L7_TRUE)
  {
    return mgmdMapProxyRouteOutIntfEntryNextGet(family, ipMRtGrp, 
                                                   ipMRtSrc, ipMRtSrcMask, intIfNum);
  }

  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: No MRP is operational to get mroute outgoinfg interface list\n");
  return L7_FAILURE;
}

/*********************************************************************
*                 API FUNCTIONS  -  NEXTHOP MROUTES
*********************************************************************/

/*********************************************************************
* @purpose  Gets the indication of whether the outgoing interface and
*           next hop represented by the given entry is currently being
*           used to forward IP datagrams.
*
* @param    ipMRtNextHopGrp       @b{(input)} next hop group
* @param    ipMRtNextHoptSrc      @b{(input)} next hop source
* @param    ipMRtNextHopSrcMask   @b{(input)} next hop source mask
* @param    ipMRtNextHopIfIndex   @b{(input)} next hop index
* @param    ipMRtNextHopAddr      @b{(input)} next hop address
* @param    state                 @b{(output)} next hop state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    If state is forwarding indicates it is currently being
*           used and if the value 'pruned' indicates it is not
*
* @end
**********************************************************************/
L7_RC_t mcastMapNextHopMRouteStateGet(L7_inet_addr_t *ipMRtNextHopGrp,
                                      L7_inet_addr_t *ipMRtNextHopSrc,
                                      L7_inet_addr_t *ipMRtNextHopSrcMask,
                                      L7_uint32 ipMRtNextHopIfIndex,
                                      L7_inet_addr_t *ipMRtNextHopAddr,
                                      L7_uint32 *state)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Checks whether the entry corresponding to the given source
*           group, and source mask is there in the next hop routing 
*           table
*
* @param    ipMRtNextHopGrp       @b{(input)} next hop group
* @param    ipMRtNextHoptSrc      @b{(input)} next hop source
* @param    ipMRtNextHopSrcMask   @b{(input)} next hop source mask
* @param    ipMRtNextHopIfIndex   @b{(input)} next hop index
* @param    ipMRtNextHopAddr      @b{(input)} next hop address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapNextHopMRouteEntryGet(L7_inet_addr_t *ipMRtNextHopGrp,
                                      L7_inet_addr_t *ipMRtNextHopSrc,
                                      L7_inet_addr_t *ipMRtNextHopSrcMask,
                                      L7_uint32 ipMRtNextHopIfIndex,
                                      L7_inet_addr_t *ipMRtNextHopAddr)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Gets the next entry in the next hop routing table for the
*           given source, group and source mask.
*
* @param    ipMRtNextHopIfIndex  @b{(input)} next hop index
* @param    ipMRtNextHopGrp      @b{(inout)} next hop group
* @param    ipMRtNextHoptSrc     @b{(inout)} next hop source
* @param    ipMRtNextHopSrcMask  @b{(inout)} next hop source mask
* @param    ipMRtNextHopAddr     @b{(inout)} next hop address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
**********************************************************************/
L7_RC_t mcastMapNextHopMRouteEntryNextGet(L7_uint32 ipMRtNextHopIfIndex,
                                          L7_inet_addr_t *ipMRtNextHopGrp,
                                          L7_inet_addr_t *ipMRtNextHopSrc,
                                          L7_inet_addr_t *ipMRtNextHopSrcMask,
                                          L7_inet_addr_t *ipMRtNextHopAddr)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  return L7_NOT_SUPPORTED;
}

/*************************************************************************
*                 API FUNCTIONS  -  REGISTRATION FOR ADMIN MODE CHANGE
*************************************************************************/

/**********************************************************************
* @purpose Register a callback function to be called when CAST interface event 
*          occurs.
*
* @param   protocolId    @b{(input)} Protocol ID  
* @param   notify        @b{(input)} pointer to the registered routine to be 
*                                    called when mcast admin mode changes. 
*
* @returns   L7_SUCCESS   
* @returns   L7_FAILURE   
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t mcastMapRegisterAdminModeEventChange(L7_MRP_TYPE_t protocolId, 
                                             L7_RC_t (*notify)(
                                        L7_uint32 event, 
                                        void *pdata,
                                        ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo))
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (protocolId >= L7_MRP_MAXIMUM)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Routing Protocol Id (%d) > L7_MRP_MAXIMUM.\n", 
                    protocolId);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Registering for mcast admin mode change with"
            "invalid protocol Id = %d\n",protocolId);
    return L7_FAILURE;
  }

  if (notify == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Invalid notify function for peotocol Id = %d \n", 
                    protocolId);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Registering for mcast admin mode change with "
            "NULL notify function \n");
    return L7_FAILURE;
  }

  osapiSemaTake(mcastGblVariables_g.mcastMapInfo.eventHandler.registered_rwlock,
                L7_WAIT_FOREVER);

  mcastGblVariables_g.notifyMcastModeChange[protocolId] = notify;
  COMPONENT_SETMASKBIT(mcastGblVariables_g.mcastMapInfo.registeredComponents,
                       protocolId);
  osapiSemaGive(mcastGblVariables_g.mcastMapInfo.eventHandler.
                registered_rwlock);

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose Unregister a callback function to be called when and MCAST
*          interface event occurs 
* 
* @param    protocolId    @b{(input)} Protocol ID  
*
* @returns   L7_SUCCESS   
* @returns   L7_FAILURE   
*
* @comments    
*
* @end
*********************************************************************/
L7_RC_t mcastMapDeregisterAdminModeEventChange(L7_MRP_TYPE_t protocolId)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");

  if (protocolId >= L7_MRP_MAXIMUM)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Routing Protocol Id (%d) > L7_LAST_ROUTER_PROTOCOL.\n", 
                    protocolId);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Deregistering for mcast admin mode change with"
            "invalid protocol Id = %d\n",protocolId);
    return L7_FAILURE;
  }

  osapiSemaTake(mcastGblVariables_g.mcastMapInfo.eventHandler.
                registered_rwlock, L7_WAIT_FOREVER);

  mcastGblVariables_g.notifyMcastModeChange[protocolId] = L7_NULLPTR;
  COMPONENT_CLRMASKBIT(mcastGblVariables_g.
                       mcastMapInfo.registeredComponents, protocolId);

  osapiSemaGive(mcastGblVariables_g.mcastMapInfo.eventHandler.
                registered_rwlock);

  return L7_SUCCESS;
}

/*************************************************************************
*                 API FUNCTIONS  -  REGISTRATION FOR ADMIN SCOPE CHANGE
*************************************************************************/

/*********************************************************************
* @purpose  Register a routine to be called when admin scope changes.
*
* @param    registrar_ID        @b{(input)} routine registrar id
* @param    pNotifyFn           @b{(input)} ptr to a routine to be invoked for 
*                                           admin scope changes.  
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapRegisterAdminScopeEventChange(L7_MRP_TYPE_t protocolId,
                                              L7_RC_t (*pNotifyFn)
                                   (L7_uint32 event_type,
                                    L7_uint32 msg_len,
                                    void *adminInfo))
{
  if (protocolId >= L7_MRP_MAXIMUM)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Routing Protocol Id (%d) > L7_MRP_MAXIMUM.\n", 
                    protocolId);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Registering for admin scope changes with"
            "invalid protocol Id = %d\n",protocolId);    return L7_FAILURE;
  }

  if (pNotifyFn == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Invalid notify function for peotocol Id = %d \n", 
                    protocolId);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Registering for admin scope changes with"
            "NULL notify function \n");
    return L7_FAILURE;
  }

  mcastGblVariables_g.notifyAdminScopeChange[protocolId] 
  = pNotifyFn;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  De-Register a routine to be called when admin scope changes
*           occur.
*
* @param    protocolId   @b{(input)} protocol to be deregistered.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastMapDeregisterAdminScopeEventChange(L7_MRP_TYPE_t protocolId)
{
  if (protocolId >= L7_MRP_MAXIMUM)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, 
                    "Routing Protocol Id (%d) > L7_MRP_MAXIMUM.\n", 
                    protocolId);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Deregistering for mcast admin mode change with"
            "invalid protocol Id = %d\n",protocolId);
    return L7_FAILURE;
  }

  mcastGblVariables_g.notifyAdminScopeChange[protocolId] = L7_NULLPTR;

  return L7_SUCCESS;
}

/*********************************************************************
*                 API FUNCTIONS  -  CALLS FROM PROTOCOL
*********************************************************************/

/******************************************************************************
* @purpose    To check if the mcast address is admin scoped
*
* @param    family      @b{(input)} family
* @param    rtrIfNum    @b{(input)} router interface number
* @param    pGrpAddr    @b{(input)} group address
*
* @returns   L7_TRUE
* @returns   L7_FALSE
*
* @comments 	 
*		
* @end
******************************************************************************/
L7_BOOL mcastMapProtocolIsAdminScopedAddress(L7_uchar8 family, 
                                             L7_uint32 rtrIfNum, 
                                             L7_inet_addr_t *pGrpAddr)
{
  L7_uint32 intIfNum = L7_NULL;
  L7_uint32 itr, tmpIfNum, entriesChecked = 0;
  L7_uchar8 grpMaskLen;
  L7_inet_addr_t grpAddr;

  if (pGrpAddr == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Input Parameters not initialized.\n");
    return L7_FALSE;
  }

  MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_APIS," Entered, group Addr : ",pGrpAddr);

  if (mcastIpMapRtrIntfToIntIfNum(family, rtrIfNum, &intIfNum) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,
                    "Failed to convert to intIfNum for rtrIfNum = %d" ,intIfNum);  
    return L7_FALSE;
  }

  if (inetIsAddressZero(pGrpAddr) == L7_TRUE)
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Invalid group to check for admin scope\n");
    return L7_FALSE;
  }

  if (osapiSemaTake(mcastGblVariables_g.mcastMapAdminScopeSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\n MCASTMAP : Failed to take admin-scope semaphore \n");
    return L7_FALSE;
  }
  
  for (itr = 0; 
      (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.
       numAdminScopeEntries) &&
      (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES); itr++)
  {

    if (mcastGblVariables_g.mcastMapCfgData.rtr.
        mcastAdminScopeCfgData[itr].inUse == L7_TRUE)
    {
      entriesChecked++;
      if ((nimIntIfFromConfigIDGet(&mcastGblVariables_g.mcastMapCfgData.rtr.
                                   mcastAdminScopeCfgData[itr].ifConfigId, &tmpIfNum) == L7_SUCCESS) && 
          (tmpIfNum == intIfNum))
      {
        inetCopy(&grpAddr, &(mcastGblVariables_g.mcastMapCfgData.rtr.
                             mcastAdminScopeCfgData[itr].groupIpAddr));
        inetMaskToMaskLen(&(mcastGblVariables_g.mcastMapCfgData.rtr.
                            mcastAdminScopeCfgData[itr].groupIpMask),
                          &grpMaskLen);
        if (inetAddrCompareAddrWithMask(&grpAddr,grpMaskLen,
                                        pGrpAddr,grpMaskLen) == 0)
        {
          MCAST_MAP_DEBUG_ADDR(MCAST_MAP_DEBUG_CONFIGURED,
                               "Configured admin scope for group Addr:",pGrpAddr);          
          osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
          return L7_TRUE;
        }
      }
    }
  }
  osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
  return L7_FALSE;
}
/******************************************************************************
* @purpose    To check if the mcast address is admin scoped
*
* @param    pGrpAddr    @b{(input)} group address
* @param    pOif        @b{(input)} interface mask with scoped interfaces 
*                                   for the given group.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments 	 
*		
* @end
******************************************************************************/
L7_RC_t mcastMapGroupScopedInterfacesGet(L7_inet_addr_t *pGrpAddr,
                                         L7_uchar8  *pOlifList)
{
  L7_uint32 rtrIfNum = L7_NULL;
  L7_uint32 itr, tmpIfNum, entriesChecked = 0;
  L7_uchar8 grpMaskLen;
  L7_inet_addr_t grpAddr;
  interface_bitset_t  *pOif = (interface_bitset_t  *)pOlifList;
  if ((pGrpAddr == L7_NULLPTR) || (pOif == L7_NULLPTR))
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Input Parameters not initialized.\n");
    return L7_FAILURE;
  }

  if (inetIsAddressZero(pGrpAddr) == L7_TRUE) 
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"\nMCAST_MAP: Invalid group to check for admin scope\n");
    return L7_FAILURE;
  }

  BITX_RESET_ALL(pOif);

  if (osapiSemaTake(mcastGblVariables_g.mcastMapAdminScopeSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "\n MCASTMAP : Failed to take admin-scope semaphore \n");
    return L7_FAILURE;
  }
  for (itr = 0; 
      (entriesChecked < mcastGblVariables_g.mcastMapCfgData.rtr.
       numAdminScopeEntries) &&
      (itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES); itr++)
  {

    if (mcastGblVariables_g.mcastMapCfgData.rtr.
        mcastAdminScopeCfgData[itr].inUse == L7_TRUE)
    {
      entriesChecked++;
      if (nimIntIfFromConfigIDGet(&mcastGblVariables_g.mcastMapCfgData.rtr.
                                   mcastAdminScopeCfgData[itr].ifConfigId, &tmpIfNum) == L7_SUCCESS)
      {
        inetCopy(&grpAddr, &(mcastGblVariables_g.mcastMapCfgData.rtr.
                             mcastAdminScopeCfgData[itr].groupIpAddr));
        inetMaskToMaskLen(&(mcastGblVariables_g.mcastMapCfgData.rtr.
                            mcastAdminScopeCfgData[itr].groupIpMask),
                          &grpMaskLen);
        if (inetAddrCompareAddrWithMask(&grpAddr,grpMaskLen,
                                        pGrpAddr,grpMaskLen) == 0)
        {
          if (mcastIpMapIntIfNumToRtrIntf(L7_AF_INET, tmpIfNum, &rtrIfNum) == L7_SUCCESS)
          {
            BITX_SET( pOif, rtrIfNum);
          }
        }
      }
    }
  }
  osapiSemaGive(mcastGblVariables_g.mcastMapAdminScopeSema);
  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  To send the protocol(MFC)-related events to mcastMap thread.
*
* @param    eventyType  @b{(input)} Event Type
* @param    msgLen      @b{(input)} Message Length.
* @param    pMsg        @b{(input)} Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mcastMapProtocolMsgSend(mfcEventType_t eventType,L7_VOIDPTR pMsg,
                                 L7_uint32 msgLen)
{
  if ((eventType != MFC_UPCALL_EXPIRY_TIMER_EVENT) &&
      (eventType != MFC_ENTRY_EXPIRY_TIMER_EVENT) &&
      (eventType != MFC_WARM_RESTART_TIMER_EVENT) &&
      (eventType != MFC_IPV6_DATA_PKT_RECV_EVENT) )
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                    "valid for eventType:%d.\n", eventType);
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES,"MCASTMAP : protocol Event = %d is not handled.\n",eventType);
    return L7_FAILURE;
  }
  return  mcastMapProtocolMsgQueue(eventType,  pMsg,  msgLen);
}

/*********************************************************************
* @purpose  Free mcast control pkt Buffer Pool allocated.
*
* @param    family      @b{(input)} family type
* @param    buffer      @b{(input)} Buffer pointer.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastCtrlPktBufferPoolFree(L7_uchar8 family, L7_uchar8 *buffer)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  if (family == L7_AF_INET)
  {
    bufferPoolFree(mcastGblVariables_g.mcastV4CtrlPktPoolId, buffer);
  }
  else if (family == L7_AF_INET6)
  {
    bufferPoolFree(mcastGblVariables_g.mcastV6CtrlPktPoolId, buffer);
  }
  else
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "Invalid family = %d\n", family);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the mcast V6 control pkt Buffer Pool allocated.
*
* @param    none 
*
* @returns  Buffer Pool Id 
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 mcastCtrlPktBufferPoolIdGet(L7_uchar8 family)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  if (family == L7_AF_INET)
  {
    return(mcastGblVariables_g.mcastV4CtrlPktPoolId);
  }
  else if (family == L7_AF_INET6)
  {
    return(mcastGblVariables_g.mcastV6CtrlPktPoolId);
  }
  else
  {
    MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_FAILURES, "Invalid family = %d\n", family);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the MCAST MGMD Events Buffer Pool ID
*
* @param    none 
*
* @returns  Buffer Pool Id 
*
* @comments
*
* @end
*********************************************************************/
L7_uint32
mcastMgmdEventsBufferPoolIdGet(L7_uchar8 family)
{
  if (family == L7_AF_INET)
  {
    return (mcastGblVariables_g.mcastV4MgmdEventsPoolId);
  }
  else if (family == L7_AF_INET6)
  {
    return(mcastGblVariables_g.mcastV6MgmdEventsPoolId);
  }
  else
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid family = %d", family);
    return 0;
  }
}

/*********************************************************************
* @purpose  Free MCAST MGMD Events Buffer Pool Buffer.
*
* @param    family         @b{(input)} family type
* @param    mgmdGroupInfo  @b{(input)} Buffer pointer.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
mcastMgmdEventsBufferPoolFree (L7_uchar8 family,
                               mgmdMrpEventInfo_t *mgmdGroupInfo)
{
  L7_uint32 mcastMgmdEventsPoolId = 0;
  L7_uint32 srcIndex = 0;

  if (mgmdGroupInfo == L7_NULLPTR)
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "mgmdGroupInfo is NULL");
    return L7_FAILURE;
  }
  if (mgmdGroupInfo->numSrcs <= 0)
  {
    return L7_FAILURE;
  }

  if (family == L7_AF_INET)
  {
    mcastMgmdEventsPoolId = mcastGblVariables_g.mcastV4MgmdEventsPoolId;
  }
  else if (family == L7_AF_INET6)
  {
    mcastMgmdEventsPoolId = mcastGblVariables_g.mcastV6MgmdEventsPoolId;
  }
  else
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Invalid family = %d", family);
    return L7_FAILURE;
  }

  /* Free all the Allocated Inner sourceList Pointers */
  for (srcIndex = 0; srcIndex < mgmdGroupInfo->numSrcs; srcIndex++)
  {
    if (mgmdGroupInfo->sourceList[srcIndex] != L7_NULLPTR)
    {
      bufferPoolFree (mcastMgmdEventsPoolId,
                      (L7_uchar8*) mgmdGroupInfo->sourceList[srcIndex]);
      mgmdGroupInfo->sourceList[srcIndex] = L7_NULLPTR;
    }
  }

  /* Free the Allocated Outer sourceList Pointer */
  if (mgmdGroupInfo->sourceList != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, mgmdGroupInfo->sourceList);
    mgmdGroupInfo->sourceList = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Free mcast data pkt Buffer Pool allocated.
*
* @param    buffer      @b{(input)} Buffer pointer.
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mcastV6DataBufferPoolFree(L7_uchar8 *buffer)
{
  MCAST_MAP_DEBUG(MCAST_MAP_DEBUG_APIS, "Function Entered.\n");
  bufferPoolFree(mcastGblVariables_g.mcastV6DataPktPoolId, buffer);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the mcast data pkt Buffer Pool allocated.
*
* @param    none
*
* @returns  Buffer Pool Id 
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 mcastV6DataBufferPoolIdGet()
{
  return (mcastGblVariables_g.mcastV6DataPktPoolId);
}

/*********************************************************************
* @purpose  Checks if any contradictory configuration made for 
*           unnumbered interface.
*
* @param    intIfNum      @b{(input)} Internal interface number
*
* @returns   L7_TRUE
* @returns   L7_FALSE
*
* @comments  An interface should not be configured as unnumbered for 
*            an DVMRP/IGMP/PIMDM configured interface.
*
* NOTE: This is IPv4-specific function. So don't use for Ipv6 family
* @end
*********************************************************************/
L7_BOOL mcastIntfIsConfigured(L7_uint32 intIfNum)
{
  L7_uint32 intfMode = L7_DISABLE;

  if ((dvmrpMapIntfAdminModeGet(intIfNum, &intfMode) == L7_SUCCESS) 
      && (intfMode == L7_ENABLE))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Failed to configure intf(%d) as unnumbered on DVMRP configured interface ", intIfNum);
    return L7_TRUE;
  }
  else if (((mgmdMapInterfaceModeGet(L7_AF_INET,intIfNum,&intfMode) == L7_SUCCESS) ||
            (mgmdMapProxyInterfaceModeGet(L7_AF_INET,intIfNum,&intfMode) == L7_SUCCESS)) &&
           (intfMode == L7_ENABLE))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Failed to configure intf(%d) as unnumbered on IGMP configured interface ",intIfNum);
    return L7_TRUE;
  }
  else if ((pimdmMapIntfAdminModeGet(L7_AF_INET,intIfNum, &intfMode) == L7_SUCCESS) 
          && (intfMode == L7_ENABLE))
  {
    MCAST_MAP_DEBUG (MCAST_MAP_DEBUG_FAILURES, "Failed to configure intf(%d) as unnumbered on PIMDM configured interface ",intIfNum);
    return L7_TRUE;
  }
  return L7_FALSE;
}

