/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ipmap_static.c
*
* @purpose   Functions relatedx to static and default IP routes.
*
* @component IP Mapping Layer
*
* @comments  none
*
* @create    5/12/2004
*
* @author    rrice
*
* @end
*
**********************************************************************/   

#include "l7_ipinclude.h"
#include "ip_util.h"
#include "l3_defaultconfig.h"


extern L7_rtrStaticRouteCfg_t  *route;
extern L7_ipMapCfg_t            *ipMapCfg;

/*********************************************************************
* @purpose  Compares a nexthop in static route with given nexthop IP and u/s/p
*
* @param    nh       @b{(input)} configured nh
* @param    nhIpAddr @b{(input)} Next hop IP address to compare with
* @param    intfConfigId @b{(input)} Contains outgoing interface for the route
*
* @returns  L7_TRUE if match
* @returns  L7_FALSE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL _ipMapSrCmpNh(L7_rtrStaticRouteNhCfgData_t *nh, L7_uint32 nhIpAddr,
    nimConfigID_t *intfConfigId)
{
  if (nh->nextHopRtr != nhIpAddr)
  {
    return L7_FALSE;
  }

  return NIM_CONFIG_ID_IS_EQUAL(&nh->intfConfigId, intfConfigId);
}

/*********************************************************************
* @purpose  Determine whether a static route includes more than one next hop.
*
* @param    staticRoute @b{(input)}  Configuration of a static route 
*
* @returns  L7_TRUE if static route includes more than one next hop
* @returns  L7_FALSE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL ipMapSrMultipleNextHops(L7_rtrStaticRouteCfgData_t *staticRoute)
{
    /* since next hop array has no holes, simply check if second element
     * is nonzero. */
    if ((platRtrRouteMaxEqualCostEntriesGet() > 1) && (staticRoute->nextHops[1].nextHopRtr != 0))
        return L7_TRUE;
    else
        return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine whether a default route is configured
*
* @param    void
*
* @returns  L7_TRUE if a default route is configured
* @returns  L7_FALSE otherwise
*
* @notes    Does not check if the default route is usable (next hop on
*           a local subnet and able to resolve next hop MAC address.)
*
* @end
*********************************************************************/
L7_BOOL ipMapDefaultRouteConfigured(void)
{
    L7_uint32 i;  

    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        if ((route->rtrStaticRouteCfgData[i].inUse == L7_TRUE) &&
            (route->rtrStaticRouteCfgData[i].ipAddr == 0) &&
            (route->rtrStaticRouteCfgData[i].ipMask == 0))

            return L7_TRUE;
    }
    return L7_FALSE;
}
                                      
/*********************************************************************
* @purpose  Set the preference of a specific next hop of a static route.
*
* @param    staticRoute @b{(input)}  Existing static route whose preference is changing
*
* @param    nextHop @b{(input)} Next hop whose preference has changed
* @param    intfConfigId @b{(input)} Contains outgoing interface for the route
*
* @param    pref @b{(input)}    Preference of static route with the next hop given
*
* @returns  L7_SUCCESS          If the preference is successfully updated.
* @returns  L7_FAILURE          
* @returns  L7_ERROR            If the maximum number of next hops for the specified 
*                               network and route preference has been exceeded
* @returns  L7_TABLE_IS_FULL    If the maximum number of static routes has been 
*                               exceeded
*
*
* @notes    The CLI command is "ip route <dest> <mask> <nh> <pref> [intf u/s/p]". The command
*           specifies only one next hop. So the preference change applies to only
*           one next hop. If staticRoute includes a single next hop, the preference
*           of the static route is simply updated. If staticRoute includes multiple
*           next hops, the next hop given is removed from this static route, 
*           leaving the remaining next hops at their previous preference. The 
*           next hop whose preference changed is either added to other next hops at
*           the new preference or added as a new static route at its own 
*           preference level.
*           
*
* @end
*********************************************************************/                                          
L7_RC_t ipMapSrNextHopPrefSet(L7_rtrStaticRouteCfgData_t *staticRoute, 
                              L7_uint32 nextHop, nimConfigID_t *intfConfigId,
                              L7_uint32 pref)
{
    L7_uint32 nh;

    /* static route with new preference of next hop */
    L7_rtrStaticRouteCfgData_t *srNewPref;  

    if (staticRoute->preference == pref)
        return L7_SUCCESS;

    if (ipMapSrMultipleNextHops(staticRoute) == L7_FALSE)
    {
        return ipMapSrPrefSet(staticRoute, pref);
    }
    /* Find our next hop */
    for (nh = 0; (nh < platRtrRouteMaxEqualCostEntriesGet()) && 
          (staticRoute->nextHops[nh].nextHopRtr != 0); nh++)
    {
        if (_ipMapSrCmpNh(&staticRoute->nextHops[nh], nextHop, intfConfigId) == L7_TRUE)
        {
            /* Remove the next hop from the route with the old preference */
            ipMapSrNextHopRemove(staticRoute, nextHop, intfConfigId);

            /* Find a static route with the new preference */
            srNewPref = ipMapSrPrefFind(staticRoute->ipAddr, staticRoute->ipMask, pref);
            if (srNewPref)
            {
                /* Add next hop to existing static route */
                ipMapSrNextHopAdd(srNewPref, nextHop, intfConfigId);
            }
            else
            {
                /* Add a new static route */
                return ipMapSrAdd(staticRoute->ipAddr, staticRoute->ipMask, nextHop, intfConfigId, pref );
            }
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the preference of a static route.
*
* @param    staticRoute @b{(input)}  Existing static route whose preference 
*                                    is changing
*
* @param    pref @b{(input)}    Preference of static route with the next hop given
*
* @returns  L7_SUCCESS          If the preference is successfully set.
* @returns  L7_FAILURE          
*
*
* @notes    The new preference is applied to all next hops in the route.         
*
* @end
*********************************************************************/                      
L7_RC_t ipMapSrPrefSet(L7_rtrStaticRouteCfgData_t *staticRoute, L7_uint32 pref)
{   
    L7_RTO_PROTOCOL_INDICES_t protocol;
    L7_uint32 oldPref = staticRoute->preference;

    /* update the configuration */
    staticRoute->preference = pref;
    route->cfgHdr.dataChanged = L7_TRUE;

    /* update RTO */
    protocol = (staticRoute->ipAddr ? RTO_STATIC : RTO_DEFAULT);
    rtoRoutePrefChange(staticRoute->ipAddr, staticRoute->ipMask,
                       protocol, oldPref, pref);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a next hop to a static route.
*
* @param    staticRoute @b{(input)} existing static route where next hop 
*                                   will be added
* @param    nextHop @b{(input)}  new next hop IP address
* @param    intfConfigId @b{(input)} Contains outgoing interface for the route
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE if next hop is in the same subnet as an existing
*                      next hop.
* @returns  L7_ERROR if the static route already has the max number
*                    of next hops.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ipMapSrNextHopAdd(L7_rtrStaticRouteCfgData_t *staticRoute, 
                          L7_uint32 nextHop, nimConfigID_t *intfConfigId)
{
    L7_uint32 nh;  /* array indices */
    L7_routeEntry_t routeEntry;

    for (nh = 0; nh < platRtrRouteMaxEqualCostEntriesGet(); nh++)
    {
        /* check for duplicate */
        if (_ipMapSrCmpNh(&staticRoute->nextHops[nh], nextHop, 
              intfConfigId) == L7_TRUE)
        {
          return L7_SUCCESS;
        }

        if (staticRoute->nextHops[nh].nextHopRtr == 0)
        {
            route->cfgHdr.dataChanged = L7_TRUE;  /* note config change */

            /* Delete existing route from RTO */
            ipMapSrEntryCreate(staticRoute, &routeEntry);
            rtoRouteDelete(&routeEntry);

            staticRoute->nextHops[nh].nextHopRtr = nextHop;

            NIM_CONFIG_ID_COPY(&staticRoute->nextHops[nh].intfConfigId, 
                intfConfigId);

            /* Add route w/new next hop to RTO */
            ipMapSrEntryCreate(staticRoute, &routeEntry);
            return rtoRouteAdd(&routeEntry);
        }
    }
    return L7_ERROR;
}

/*********************************************************************
* @purpose  Remove a next hop from a static route.
*
* @param    staticRoute @b{(input)} existing static route where next hop 
*                                   will be deleted
* @param    nextHop @b{(input)}   next hop IP address to be removed
* @param    intfConfigId @b{(input)} Contains outgoing interface for the route
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    Fills the hole left in the next hop array.
*
* @end
*********************************************************************/
L7_RC_t ipMapSrNextHopRemove(L7_rtrStaticRouteCfgData_t *staticRoute, 
                             L7_uint32 nextHop, nimConfigID_t *intfConfigId)
{
    L7_uint32 nh, j;  /* array indices */
    L7_routeEntry_t routeEntry;

    for (nh = 0; (nh < platRtrRouteMaxEqualCostEntriesGet()) && 
          (staticRoute->nextHops[nh].nextHopRtr != 0); nh++)
    {
        
        if (_ipMapSrCmpNh(&staticRoute->nextHops[nh], nextHop, intfConfigId) 
            == L7_TRUE)
        {
            route->cfgHdr.dataChanged = L7_TRUE;  /* note config change */

            /* Delete existing route from RTO */
            ipMapSrEntryCreate(staticRoute, &routeEntry);
            rtoRouteDelete(&routeEntry);

            /* fill hole by shifting remaining entries left one index */
            for (j = nh; j < platRtrRouteMaxEqualCostEntriesGet(); j++)
            {
                if (j == (platRtrRouteMaxEqualCostEntriesGet() - 1))
                    memset(&staticRoute->nextHops[j], 0,
                        sizeof(L7_rtrStaticRouteNhCfgData_t));
                else
                    memcpy(&staticRoute->nextHops[j],
                        &staticRoute->nextHops[j+1], 
                        sizeof(L7_rtrStaticRouteNhCfgData_t));

                if (staticRoute->nextHops[j].nextHopRtr == 0)
                {
                    if (j == 0)
                    {
                        /* No next hops remain. Delete static route. */
                        memset(staticRoute, 0, sizeof(L7_rtrStaticRouteCfgData_t));
                    }
                    else
                    {
                        /* At least one next hop left. Add route w/o deleted 
                         * next hop to RTO. */
                        ipMapSrEntryCreate(staticRoute, &routeEntry);
                        rtoRouteAdd(&routeEntry);
                    }
                    return L7_SUCCESS;
                }
            }
        }
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Find the static route with the given destination
*
* @param    ipAddr @b{(input)} destination prefix
* @param    subnetMask @b{(input)} destination prefix network mask
*
* @returns  Pointer to matching static route
*
* @notes    Returns if a static route exists for a given destination
*
* @end
*********************************************************************/
L7_rtrStaticRouteCfgData_t*
ipMapSrFind(L7_uint32 ipAddr, L7_uint32 subnetMask)
{
    L7_uint32 i;  /* array indices */

    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        if (route->rtrStaticRouteCfgData[i].inUse == L7_TRUE)
        {
            if ((route->rtrStaticRouteCfgData[i].ipAddr == ipAddr) &&
                (route->rtrStaticRouteCfgData[i].ipMask == subnetMask))
            {
                return &route->rtrStaticRouteCfgData[i];
            }
        }
    }
    return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Find the static route with the given destination and next hop.
*
* @param    ipAddr @b{(input)} destination prefix
* @param    subnetMask @b{(input)} destination prefix network mask
* @param    nextHop @b{(input)}   next hop IP address 
* @param    intfConfigId @b{(input)} Contains outgoing interface for the route
*
* @returns  Pointer to matching static route
*
* @notes    There can only be one static route to a given destination 
*           that includes a given next hop IP address.
*
* @end
*********************************************************************/
L7_rtrStaticRouteCfgData_t*
ipMapSrNextHopFind(L7_uint32 ipAddr, L7_uint32 subnetMask, L7_uint32 nextHop, 
    nimConfigID_t *intfConfigId)
{
    L7_uint32 i, nh;  /* array indices */

    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        if (route->rtrStaticRouteCfgData[i].inUse == L7_TRUE)
        {
            if ((route->rtrStaticRouteCfgData[i].ipAddr == ipAddr) &&
                (route->rtrStaticRouteCfgData[i].ipMask == subnetMask))
            {
                for (nh = 0; (nh < platRtrRouteMaxEqualCostEntriesGet()) &&
                             (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr != 0); nh++)
                {
                    if (_ipMapSrCmpNh(&route->rtrStaticRouteCfgData[i].nextHops[nh], nextHop, intfConfigId) == L7_TRUE)
                    {
                      return &route->rtrStaticRouteCfgData[i];
                    }
                }
            }
        }
    }
    return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Find the static route with the given destination and preference.
*
* @param    ipAddr @b{(input)} destination prefix
* @param    subnetMask @b{(input)} destination prefix network mask
* @param    pref @b{(input)}   route preference
*
* @returns  Pointer to matching static route
*
* @notes    There can only be one static route to a given destination 
*           with a given preference.
*
* @end
*********************************************************************/
L7_rtrStaticRouteCfgData_t* ipMapSrPrefFind(L7_uint32 ipAddr, 
                                            L7_uint32 subnetMask, 
                                            L7_uint32 pref)
{
    L7_uint32 i;  

    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        if (route->rtrStaticRouteCfgData[i].inUse == L7_TRUE)
        {
            if ((route->rtrStaticRouteCfgData[i].ipAddr == ipAddr) &&
                (route->rtrStaticRouteCfgData[i].ipMask == subnetMask) &&
                (route->rtrStaticRouteCfgData[i].preference == pref))
            {
                return &route->rtrStaticRouteCfgData[i];
            }
        }
    }
    return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Find the static reject route with the given destination and mask.
*
* @param    ipAddr @b{(input)} destination prefix
* @param    subnetMask @b{(input)} destination prefix network mask
*
* @returns  Pointer to matching static route
*
* @notes    There can only be one static route to a given destination 
*           and mask
*
* @end
*********************************************************************/
L7_rtrStaticRouteCfgData_t* ipMapSrRejectFind(L7_uint32 ipAddr, 
                                              L7_uint32 subnetMask)
{
    L7_uint32 i;  

    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        if (route->rtrStaticRouteCfgData[i].inUse == L7_TRUE)
        {
            if ((route->rtrStaticRouteCfgData[i].ipAddr == ipAddr) &&
                (route->rtrStaticRouteCfgData[i].ipMask == subnetMask) &&
                (route->rtrStaticRouteCfgData[i].flags & L7_RTF_REJECT))
            {
                return &route->rtrStaticRouteCfgData[i];
            }
        }
    }
    return L7_NULLPTR;
}

/*********************************************************************
* @purpose  Add a static route.
*
* @param    ipAddr @b{(input)} destination prefix
* @param    subnetMask @b{(input)} destination prefix network mask
* @param    nextHop @b{(input)}   next hop IP address 
* @param    intfConfigId @b{(input)} Contains outgoing interface for the route
* @param    pref @b{(input)} pref
*
* @returns  L7_SUCCESS if static route added to configuration
* @returns  L7_TABLE_IS_FULL if the maximum number of static routes are
*                            already configured
* @returns  L7_ALREADY_CONFIGURED if the route is a duplicate (should not happen)
* @returns  L7_FAILURE for any other failure
*
* @notes    Assumes the caller has verified that there is not 
*           already a static route with the same destination and next hop
*           or the same destination and preference.
*
* @end
*********************************************************************/
L7_RC_t ipMapSrAdd(L7_uint32 ipAddr, L7_uint32 subnetMask, 
                   L7_uint32 nextHop, nimConfigID_t *intfConfigId, 
                   L7_uint32 pref)
{
    L7_uint32 i;
    L7_routeEntry_t routeEntry;
    L7_RC_t rc;
    L7_rtrStaticRouteNhCfgData_t *nh;

    /* find an available index in the static routes array */
    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        if (route->rtrStaticRouteCfgData[i].inUse == L7_FALSE)
        {
            bzero((L7_char8 *)&route->rtrStaticRouteCfgData[i], 
                  sizeof(L7_rtrStaticRouteCfgData_t));

            route->rtrStaticRouteCfgData[i].ipAddr = ipAddr;
            route->rtrStaticRouteCfgData[i].ipMask = subnetMask;
            route->rtrStaticRouteCfgData[i].preference = pref;
            nh = &route->rtrStaticRouteCfgData[i].nextHops[0];
            nh->nextHopRtr = nextHop;
            if(nextHop == 0)
            {
              /* next-hop is 0 for a static reject route */
              route->rtrStaticRouteCfgData[i].flags = L7_RTF_REJECT;
            }
            NIM_CONFIG_ID_COPY(&nh->intfConfigId, intfConfigId);
            /* add route to RTO */
            ipMapSrEntryCreate(&route->rtrStaticRouteCfgData[i], &routeEntry);

            if(route->rtrStaticRouteCfgData[i].flags & L7_RTF_REJECT)
            {
              rc = L7_SUCCESS;
              if (ipMapCfg->rtr.rtrAdminMode == L7_ENABLE)
              {
                /* Allow static reject route to be added into RTO
                 * only if the global routing mode is enabled, since
                 * reject route is not depedent on any outgoing interface's
                 * routing mode */
                rc = rtoRouteAdd(&routeEntry);
              }
            }
            else
            {
              rc = rtoRouteAdd(&routeEntry);
            }

            /* if route added to RTO or if not added because a next hop was not
             * on a local subnet, add to the configuration. When interface with 
             * this next hop comes up, IP MAP will add static route to RTO. For
             * any other failure, there won't be a trigger to re-add the route. */
            if ((rc == L7_SUCCESS) || (rc == L7_NOT_EXIST))
            {
              route->rtrStaticRouteCfgData[i].inUse = L7_TRUE;
              route->cfgHdr.dataChanged = L7_TRUE;
            }
            return rc;
        }
    }
    /* Already have maximum static routes configured */
    return L7_TABLE_IS_FULL;
}

/*********************************************************************
* @purpose  Delete static reject route to the given destination.
*
* @param    ipAddr @b{(input)} destination prefix
* @param    subnetMask @b{(input)} destination prefix network mask
*
* @returns  L7_SUCCESS 
*
* @notes    
*          
*
* @end
*********************************************************************/
L7_RC_t ipMapSrRejectDelete(L7_uint32 ipAddr, L7_uint32 subnetMask)
{
  L7_rtrStaticRouteCfgData_t *staticRoute;
  L7_routeEntry_t routeEntry;

  staticRoute = ipMapSrRejectFind(ipAddr, subnetMask);
  if(staticRoute != L7_NULLPTR)
  {
    /* Delete existing route from RTO */
    ipMapSrEntryCreate(staticRoute, &routeEntry);
    rtoRouteDelete(&routeEntry);
    memset(staticRoute, 0, sizeof(L7_rtrStaticRouteCfgData_t));
    route->cfgHdr.dataChanged = L7_TRUE;

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Delete all static routes to the given destination.
*
* @param    ipAddr @b{(input)} destination prefix
* @param    subnetMask @b{(input)} destination prefix network mask
*
* @returns  L7_SUCCESS 
*
* @notes    There can be multiple static routes to a given destination,
*           (with different preferences and next hops). Delete them all.
*
* @end
*********************************************************************/
L7_RC_t ipMapSrDelete(L7_uint32 ipAddr, L7_uint32 subnetMask)
{
    L7_uint32 i;
    L7_routeEntry_t routeEntry;

    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        if ((route->rtrStaticRouteCfgData[i].inUse == L7_TRUE) &&
            (route->rtrStaticRouteCfgData[i].ipAddr == ipAddr) &&
            (route->rtrStaticRouteCfgData[i].ipMask == subnetMask))
        {
            ipMapSrEntryCreate(&route->rtrStaticRouteCfgData[i], &routeEntry);
            rtoRouteDelete(&routeEntry);
            bzero((L7_char8 *)&route->rtrStaticRouteCfgData[i], 
                  sizeof(L7_rtrStaticRouteCfgData_t));
            route->cfgHdr.dataChanged = L7_TRUE;
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a static route in the form of a L7_rtrStaticRouteCfgData_t
*           instance, create an instance of L7_routeEntry_t with the same
*           attributes.
*
* @param    staticRoute @b{(input)} input static route
* @param    routeEntry @b{(output)} output route entry
*
* @returns  L7_SUCCESS if successfully created
*           L7_FAILURE otherwise
*
* @notes    
* A configured next hop in staticRoute is added under following conditions,
* a. Next hop configured with un-nmbered interface.
* b. Next hop configured with numbered interface and nexthoprtr is on one of
* the subnet configured for the interface.
*
* Since routing entries can be added with and without interface, it is
* possible that there can be two routes configured such that they use same
* interface. For example consider following routes,
* a. 1.1.1.0/24 nexthop = 2.2.2.2 without interface
* b. 1.1.1.0/24 nexthop = 2.2.2.2 on interface 1.
*
* Now if interface 1 is assigned IP address 2.2.2.1, then the routes configred
* above become duplicate routes. Make sure such duplicates are not passed to
* RTO.
*
* @end
*********************************************************************/
L7_RC_t ipMapSrEntryCreate(L7_rtrStaticRouteCfgData_t *staticRoute, 
                           L7_routeEntry_t *routeEntry)
{
    L7_uint32 nh;
    L7_uint32 intIfNum;
    L7_uint32 nhIntIfNum;
    L7_uint32 validHops;
    nimConfigID_t nullIntfConfigId;
    L7_uint32 i;

    if (!staticRoute || !routeEntry)
        return L7_FAILURE;

    memset(routeEntry, 0, sizeof(L7_routeEntry_t));
    routeEntry->ipAddr = staticRoute->ipAddr;
    routeEntry->subnetMask = staticRoute->ipMask;
    if (staticRoute->ipAddr == 0)
        routeEntry->protocol = RTO_DEFAULT;
    else
        routeEntry->protocol = RTO_STATIC;
    routeEntry->metric = FD_RTR_STATIC_ROUTE_COST;
    routeEntry->pref = staticRoute->preference;
    routeEntry->flags = staticRoute->flags;

    memset(&nullIntfConfigId, 0, sizeof(nullIntfConfigId));
    for (nh = 0, validHops = 0; 
          (nh < platRtrRouteMaxEqualCostEntriesGet()) && 
          (staticRoute->nextHops[nh].nextHopRtr != 0); 
          nh++)
    {
        intIfNum = L7_INVALID_INTF;
        /* Handle case where interface specified in nexthop */
        if ( NIM_CONFIG_ID_IS_EQUAL(&staticRoute->nextHops[nh].intfConfigId,
                            &nullIntfConfigId) != L7_TRUE )
        {
          if (nimIntIfFromConfigIDGet(&staticRoute->nextHops[nh].intfConfigId,
                            &intIfNum) != L7_SUCCESS)
          {
            continue;
          }

          /* For numbered interfaces, make sure next hop is on the interface
           * configured. */
          if (_ipMapIntfIsUnnumbered(intIfNum) != L7_TRUE)
          {
            if (_ipMapRouterIfResolve(staticRoute->nextHops[nh].nextHopRtr, 
                                       &nhIntIfNum) != L7_SUCCESS)
            {
              continue;
            }

            /* skip if next hop not on interface specified */
            if (nhIntIfNum != intIfNum)
            {
              continue;
            }
          }
        }
        else
        {
          if (_ipMapRouterIfResolve(staticRoute->nextHops[nh].nextHopRtr, 
                                       &intIfNum) != L7_SUCCESS)
          {
            continue;
          }
        }

        /* Make sure the outgoing interface is operational */
        if (ipMapIntfIsUp(intIfNum) != L7_TRUE)
        {
          continue;
        }

        /* We know route can be added. Make sure a duplicate is not already
         * configured as nexthop earlier. */
        for ( i = 0; i < validHops; i++ )
        {
          if (routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.intIfNum == 
              intIfNum && 
              routeEntry->ecmpRoutes.equalCostPath[i].arpEntry.ipAddr == 
              staticRoute->nextHops[nh].nextHopRtr)
          {
            break;
          }
        }

        /* Dont add nexthop if duplicate entry found */
        if ( i < validHops)
        {
          continue;
        }

        /* Safe to add the route */
        routeEntry->ecmpRoutes.equalCostPath[validHops].arpEntry.intIfNum = intIfNum;
        routeEntry->ecmpRoutes.equalCostPath[validHops].arpEntry.ipAddr = 
            staticRoute->nextHops[nh].nextHopRtr;
        validHops++;
    }
    routeEntry->ecmpRoutes.numOfRoutes = validHops;

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  When a routing interface is enabled, find static routes whose
*           next hop is on that interface. Add the static route to the 
*           routing table.
*
* @param    intIfNum @b{(input)} internal interface number of the interface
*                                just enabled
*
* @returns  L7_SUCCESS 
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ipMapSrInterfaceEnable(L7_uint32 intIfNum)
{
    L7_uint32 i, nh;

    /* interface interface number of interface to the next hop of a static
     * route */
    L7_uint32 ifcToNextHop;      

    L7_rtrStaticRouteCfgData_t *staticRoute;
    L7_routeEntry_t routeEntry;
    nimConfigID_t nullIntfConfigId;

    memset(&nullIntfConfigId, 0, sizeof(nullIntfConfigId));
    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        staticRoute = &route->rtrStaticRouteCfgData[i];
        if (staticRoute->inUse == L7_FALSE)
            continue;
        
        for (nh = 0; 
              (nh < platRtrRouteMaxEqualCostEntriesGet()) && 
              (staticRoute->nextHops[nh].nextHopRtr != 0); 
              nh++)
        {

            if ( NIM_CONFIG_ID_IS_EQUAL(&staticRoute->nextHops[nh].intfConfigId,
                            &nullIntfConfigId) != L7_TRUE )
            {
                if (nimIntIfFromConfigIDGet(&staticRoute->nextHops[nh].intfConfigId,
                            &ifcToNextHop) != L7_SUCCESS)
                {
                    continue;
                }
            }
            else
            {
                if (_ipMapRouterIfResolve(staticRoute->nextHops[nh].nextHopRtr, 
                                       &ifcToNextHop) != L7_SUCCESS)
                {
                    continue;
                }
            }

            if (ifcToNextHop == intIfNum)
            {   
                /* Next hop of static route is routing interface that just
                 * came up. */
                ipMapSrEntryCreate(staticRoute, &routeEntry);
                if (ipMapSrMultipleNextHops(staticRoute) == L7_TRUE)
                {
                    /* Assume static route may have been added to RTO when another
                     * next hop interface came up, so delete route from RTO. */
                    rtoRouteDelete(&routeEntry);
                }
                /* This will tell RTO about all next hops. Some may not be on a 
                 * local routing interface that is active. RTO will only keep 
                 * next hops on active interfaces and will ignore the rest. */ 
                if (rtoRouteAdd(&routeEntry) != L7_SUCCESS)
                {
                    L7_char8 destAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
                    L7_char8 destMaskStr[OSAPI_INET_NTOA_BUF_SIZE];
                    osapiInetNtoa(routeEntry.ipAddr, destAddrStr);
                    osapiInetNtoa(routeEntry.subnetMask, destMaskStr);
                    IPMAP_TRACE("On interface %u enable, failed to add static route to %s %s to RTO.\n",
                                intIfNum, destAddrStr, destMaskStr);
                }
            }
        }
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given two static routes to the same destination and with 
*           different preferences, merge the next hops from the 
*           source static route to the target static route. 
*
* @param    sourceStaticRoute @b{(input)} destination prefix
* @param    targetStaticRoute @b{(input)} destination prefix network mask
*
* @returns  L7_SUCCESS if successful
*
* @notes    Since there cannot be two static routes to the same destination
*           with the same next hop and different preferences, we can assume
*           that the next hops in the two static routes are unique.
*
* @end
*********************************************************************/
L7_RC_t ipMapSrNextHopsMerge(L7_rtrStaticRouteCfgData_t *sourceStaticRoute, 
                             L7_rtrStaticRouteCfgData_t *targetStaticRoute)
{
    L7_routeEntry_t routeEntry;
    L7_uint32 srcIndex, targetIndex;

    /* Do some sanity checking */
    if ((sourceStaticRoute->ipAddr != targetStaticRoute->ipAddr) ||
        (sourceStaticRoute->ipMask != targetStaticRoute->ipMask))
    {
        return L7_FAILURE;
    }

    if (sourceStaticRoute->preference == targetStaticRoute->preference)
    {
        return L7_FAILURE;
    }

    /* Delete the routes from RTO */
    ipMapSrEntryCreate(sourceStaticRoute, &routeEntry);
    rtoRouteDelete(&routeEntry);
    ipMapSrEntryCreate(targetStaticRoute, &routeEntry);
    rtoRouteDelete(&routeEntry);

    /* Find first available element in target next hop array */
    for (targetIndex = 0; 
          (targetIndex < platRtrRouteMaxEqualCostEntriesGet()) && 
          (targetStaticRoute->nextHops[targetIndex].nextHopRtr != 0); 
          targetIndex++);

    /* Copy next hops in configuration */
    srcIndex = 0;
    while ((targetIndex < platRtrRouteMaxEqualCostEntriesGet()) &&
           (sourceStaticRoute->nextHops[srcIndex].nextHopRtr != 0))
    {
        memcpy(&targetStaticRoute->nextHops[targetIndex], 
            &sourceStaticRoute->nextHops[srcIndex], 
            sizeof(L7_rtrStaticRouteNhCfgData_t));
        targetIndex++;
        srcIndex++;
    }
    
    /* Delete source static route configuration */
    bzero((L7_char8 *)sourceStaticRoute, sizeof(L7_rtrStaticRouteCfgData_t));
    route->cfgHdr.dataChanged = L7_TRUE;

    /* Add target static route with union of next hops to RTO */
    ipMapSrEntryCreate(targetStaticRoute, &routeEntry);
    rtoRouteAdd(&routeEntry);

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine whether the ip address passed to this function
*           is the same as the next hop ip address of a static route
*
* @param    ipAddr @b{(input)} ipAddr to be compared
*
* @returns  L7_TRUE if there is a conflict
* @returns  L7_FALSE otherwise
*
* @notes    Checks if the ip address passed is the same as the next
*           hop address of a static route
*
* @end
*********************************************************************/
L7_BOOL ipMapSrNextHopIpAddressConflictCheck(L7_uint32 ipAddr)
{
    L7_uint32 i;  /* static route array indices */
    L7_uint32 nh; /* next hop array indices */

    for (i = 0; i < L7_RTR_MAX_STATIC_ROUTES; i++)
    {
        for (nh = 0; (nh < platRtrRouteMaxEqualCostEntriesGet()) &&
            (route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr != 0); nh++)
        {
            if (ipAddr == route->rtrStaticRouteCfgData[i].nextHops[nh].nextHopRtr)
                return L7_TRUE;
        }
    }
    return L7_FALSE;
}

