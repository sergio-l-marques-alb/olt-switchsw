/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rto_porting.c
*
* @purpose To direct customers to the portion of the components which are 
*          most likely to be customized for the customer environment. 
*
* @component Routing Table Object for IP Mapping Layer
*
* @comments none
*
* @create 05/28/2002
*
* @author betsyt
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include "stdlib.h"
#include "string.h"
#include "log.h"
#include "l7_common.h"
#include "l7_common_l3.h"
#include "osapi.h"
#include "l3end_api.h"
#include "rto_api.h"
#include "rto.h"
#include "avl_api.h"
#include "rtiprecv.h"
#include "l7_ip_api.h"


rtoRouteInfo_t *rtoSelectPreferredRoute(rtoRouteInfo_t  *route1, rtoRouteInfo_t  *route2);

/*********************************************************************
* @purpose  Select the preferred route of two routes
*
* @param    *route1  pointer to first route to be compared, rtoRouteInfo_t
* @param    *route2  pointer to second route to be compared, rtoRouteInfo_t
*
* @returns  Pointer to the preferred rtoRouteInfo_t structure
*
* @notes    none 
*       
* @end
*********************************************************************/
rtoRouteInfo_t *rtoSelectPreferredRoute(rtoRouteInfo_t  *route1, rtoRouteInfo_t  *route2)
{
  L7_uint32 route1Pref;
  L7_uint32 route2Pref;

  /* Get rid of any ties in preference */

  /*start with primary preference*/
  route1Pref = route1->preference1;
  route2Pref = route2->preference1;

  if (route1Pref == route2Pref)
  {
    /* if a tie, then use secondary preference */
    route1Pref = route1->preference2;
    route2Pref = route2->preference2;
  }

  /* find the best route */
  if (route1Pref < route2Pref)
  {
    return route1;
  }
  else 
  {
    return route2;
  }
}
