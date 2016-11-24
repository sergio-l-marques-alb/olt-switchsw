
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingIpEcmpRoutes.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  22 June 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingIpEcmpRoutes_obj.h"
#include "usmdb_nim_api.h"
/*******************************************************************************
* @function fpObjGet_routingIpEcmpRoutes_routeIp
*
* @purpose Get 'routeIp'
 *@description  [routeIp] The IP Address of destination network.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpEcmpRoutes_routeIp (void *wap, void *bufp)
{

  xLibIpV4_t objrouteIpValue = 0;
  xLibIpV4_t nextObjrouteIpValue = 0;
  L7_routeEntry_t  routeEntry;
  xLibBool_t routeType;
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibBool_t));

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset(&routeEntry, 0x0, sizeof(routeEntry));
  routeType = L7_TRUE;

  /* retrieve key: routeIp */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeIp,
                          (xLibU8_t *) & objrouteIpValue, &owa.len);

  owa1.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_RouteTypeflag,
                          (xLibU8_t *) & routeType, &owa1.len);
 
  if (owa.rc != XLIBRC_SUCCESS)
  {
    
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT,
                                      &routeEntry, routeType);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteIpValue, owa.len);

    routeEntry.ipAddr = objrouteIpValue;

    do
    {
       owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT,
                                      &routeEntry, routeType); 
    }
    while ((objrouteIpValue == routeEntry.ipAddr) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjrouteIpValue = routeEntry.ipAddr;
 
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjrouteIpValue, owa.len);

  /* return the object value: routeIp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjrouteIpValue, sizeof (nextObjrouteIpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpEcmpRoutes_routeMask
*
* @purpose Get 'routeMask'
 *@description  [routeMask] The network maks of destination network.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpEcmpRoutes_routeMask (void *wap, void *bufp)
{

  xLibIpV4_t objrouteIpValue = 0;
  xLibIpV4_t objrouteMaskValue = 0;
  xLibIpV4_t nextObjrouteMaskValue = 0;
  L7_routeEntry_t  routeEntry;
  xLibBool_t routeType;
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibBool_t));

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: routeIp */
  memset(&routeEntry, 0x0, sizeof(routeEntry));
  routeType = L7_TRUE;

  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeIp,
                          (xLibU8_t *) & objrouteIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteIpValue, owa.len);

  /* retrieve key: routeMask */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeMask,
                          (xLibU8_t *) & objrouteMaskValue, &owa.len);

  owa1.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_RouteTypeflag,
                          (xLibU8_t *) & routeType, &owa1.len);

  routeEntry.ipAddr = objrouteIpValue;
  routeEntry.subnetMask = objrouteMaskValue;

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT,
                                      &routeEntry, routeType);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteMaskValue, owa.len);
    do
    {
      owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT,
                                      &routeEntry, routeType);
    }
    while ((objrouteIpValue == routeEntry.ipAddr) && (objrouteMaskValue == routeEntry.subnetMask)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objrouteIpValue != routeEntry.ipAddr) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjrouteMaskValue = routeEntry.subnetMask;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjrouteMaskValue, owa.len);

  /* return the object value: routeMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjrouteMaskValue,
                           sizeof (nextObjrouteMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpEcmpRoutes_protocol
*
* @purpose Get 'protocol'
 *@description  [protocol] Protocol via which the route(s) were learned.A value
* from L7_RTO_PROTOCOL_INDICES_t..
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpEcmpRoutes_protocol (void *wap, void *bufp)
{
  xLibU32_t objprotocolValue = 0;
  xLibU32_t nextObjprotocolValue = 0;

  xLibIpV4_t objrouteIpValue = 0;
  xLibIpV4_t objrouteMaskValue = 0;
  L7_uint32  routeFound = 0;
  xLibBool_t routeType;
  L7_RC_t rc;

  L7_routeEntry_t  routeEntry;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibBool_t));

  FPOBJ_TRACE_ENTER (bufp);

  routeType = L7_TRUE;
  routeFound = L7_FALSE;

  memset(&objprotocolValue, 0x0, sizeof(objprotocolValue));
  memset(&nextObjprotocolValue, 0x0, sizeof(nextObjprotocolValue));
  memset(&routeEntry, 0x0, sizeof(routeEntry));
  /* retrieve key: routeIp */
  owa.len = sizeof (objrouteIpValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeIp,
                          (xLibU8_t *) & objrouteIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteIpValue, owa.len);

  /* retrieve key: routeMask */
  owa.len = sizeof (objrouteMaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeMask,
                          (xLibU8_t *) & objrouteMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteMaskValue, owa.len);

  /* retrieve key: protocol */
  objprotocolValue = 0;
  owa.len = sizeof (objprotocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_protocol,
                          (xLibU8_t *) & objprotocolValue, &owa.len);

  owa1.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_RouteTypeflag,
                          (xLibU8_t *) & routeType, &owa1.len);
 
 routeEntry.ipAddr = objrouteIpValue;
 routeEntry.subnetMask = objrouteMaskValue;
 routeEntry.protocol = 0;
 routeEntry.pref = 0;

 if(routeType == L7_TRUE)
 {  
   /* In best route case we have only one protocol value is available.Since only one route is available */
   if (owa.rc == XLIBRC_SUCCESS)
   {
     owa.rc = XLIBRC_ENDOF_TABLE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }

 if(owa.rc != XLIBRC_SUCCESS)
 {
    routeEntry.subnetMask = 0;
    while ((rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType)) == L7_SUCCESS)
    {
     if((routeEntry.ipAddr == objrouteIpValue) && (routeEntry.subnetMask == objrouteMaskValue))
     {
        routeFound = L7_TRUE;
        nextObjprotocolValue = routeEntry.protocol;  
        break;
     }
    }
  
    if (routeFound == L7_FALSE)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
 }
 else
 {
   FPOBJ_TRACE_CURRENT_KEY (bufp, &objprotocolValue, owa.len);

   do
   {
     owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT,
                                       &routeEntry, routeType);
   }
   while ((objrouteIpValue == routeEntry.ipAddr) && (objrouteMaskValue == routeEntry.subnetMask) &&
          (objprotocolValue != routeEntry.protocol) && (owa.l7rc == L7_SUCCESS));

   while((owa.l7rc == L7_SUCCESS) && (objrouteIpValue == routeEntry.ipAddr) &&
         (objrouteMaskValue == routeEntry.subnetMask) && (objprotocolValue == routeEntry.protocol))
   {
     owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType);
   }
 }

  if ((objrouteIpValue != routeEntry.ipAddr) || (objrouteMaskValue != routeEntry.subnetMask) ||
      (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  nextObjprotocolValue = routeEntry.protocol;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjprotocolValue, owa.len);
  /* return the object value: protocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjprotocolValue,
                           sizeof (nextObjprotocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpEcmpRoutes_pref
*
* @purpose Get 'pref'
* @description  [pref] Preference value for this route.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpEcmpRoutes_pref (void *wap, void *bufp)
{
  xLibU32_t objprotocolValue = 0;
  xLibU32_t objprefValue = 0;
  xLibU32_t nextObjprefValue = 0;
  xLibIpV4_t objrouteIpValue = 0;
  xLibIpV4_t objrouteMaskValue = 0;
  L7_uint32  routeFound = 0;
  xLibBool_t routeType;
  L7_RC_t rc;

  L7_routeEntry_t  routeEntry;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibBool_t));

  FPOBJ_TRACE_ENTER (bufp);

  routeType = L7_TRUE;
  routeFound = L7_FALSE;

  memset(&routeEntry, 0x0, sizeof(routeEntry));
  /* retrieve key: routeIp */
  owa.len = sizeof (objrouteIpValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeIp,
                          (xLibU8_t *) & objrouteIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteIpValue, owa.len);

  /* retrieve key: routeMask */
  owa.len = sizeof (objrouteMaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeMask,
                          (xLibU8_t *) & objrouteMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteMaskValue, owa.len);

  /* retrieve key: protocol */
  objprotocolValue = 0;
  owa.len = sizeof (objprotocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_protocol,
                          (xLibU8_t *) & objprotocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprotocolValue, owa.len);

  /* retrieve key: pref */
  objprefValue = 0;
  owa.len = sizeof (objprefValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_pref,
                          (xLibU8_t *) & objprefValue, &owa.len);

  owa1.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_RouteTypeflag,
                          (xLibU8_t *) & routeType, &owa1.len);
 
  routeEntry.ipAddr = objrouteIpValue;
  routeEntry.subnetMask = objrouteMaskValue;
  routeEntry.protocol = 0;
  routeEntry.pref = 0;

  if(routeType == L7_TRUE)
  {
    /* In best route case we have only one protocol value is available.Since only one route is available */
    if (owa.rc == XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if(owa.rc != XLIBRC_SUCCESS)
  {
    routeEntry.subnetMask = 0;
    while ((rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType)) == L7_SUCCESS)
    {
      if((routeEntry.ipAddr == objrouteIpValue) && (routeEntry.subnetMask== objrouteMaskValue) &&
         (routeEntry.protocol == objprotocolValue))
      {
        routeFound = L7_TRUE;
        nextObjprefValue = routeEntry.pref;  
        break;
      }
    }
 
    if (routeFound == L7_FALSE)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objprefValue, owa.len);

    do
    {
      owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType);
    }
    while ((objrouteIpValue == routeEntry.ipAddr) && (objrouteMaskValue == routeEntry.subnetMask) &&
           (objprotocolValue != routeEntry.protocol) && (owa.l7rc == L7_SUCCESS));

    while((owa.l7rc == L7_SUCCESS) && (objrouteIpValue == routeEntry.ipAddr) &&
          (objrouteMaskValue == routeEntry.subnetMask) && (objprotocolValue == routeEntry.protocol)
          && (objprefValue != routeEntry.pref))
    {
      owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType);
    }

    while((owa.l7rc == L7_SUCCESS) && (objrouteIpValue == routeEntry.ipAddr) &&
          (objrouteMaskValue == routeEntry.subnetMask) && (objprotocolValue == routeEntry.protocol)
          && (objprefValue == routeEntry.pref))
    {
      owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType);
    }

    if ((objrouteIpValue != routeEntry.ipAddr) || (objrouteMaskValue != routeEntry.subnetMask) ||
        (objprotocolValue != routeEntry.protocol) || (owa.l7rc != L7_SUCCESS))
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    nextObjprefValue = routeEntry.pref;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjprefValue, owa.len);
  /* return the object value: pref */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjprefValue,
                           sizeof (nextObjprefValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingIpEcmpRoutes_nextHop
*
* @purpose Get 'IpAddress'
 *@description  [IpAddress] Ip Address of the gateway.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpEcmpRoutes_nextHop (void *wap, void *bufp)
{

  xLibIpV4_t objrouteIpValue = 0;
  xLibIpV4_t nextObjrouteIpValue = 0;
  xLibIpV4_t objrouteMaskValue = 0;
  xLibIpV4_t nextObjrouteMaskValue = 0;
  xLibIpV4_t objIpAddressValue = 0;
  xLibIpV4_t nextObjIpAddressValue = 0;
  xLibS32_t nextObjIfindexValue = 0;
  xLibU32_t objprotocolValue = 0;
  xLibU32_t nextObjprotocolValue = 0;
  xLibU32_t objprefValue = 0;
  xLibU32_t nextObjprefValue = 0;
  xLibBool_t routeType;
  L7_routeEntry_t  routeEntry;
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibBool_t));

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);
  
  /* retrieve key: routeIp */
  routeType = L7_TRUE;

  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeIp,
                          (xLibU8_t *) & objrouteIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteIpValue, owa.len);

  /* retrieve key: routeMask */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeMask,
                          (xLibU8_t *) & objrouteMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteMaskValue, owa.len);

  owa.len = sizeof (objprotocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_protocol,
                          (xLibU8_t *) & objprotocolValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprotocolValue, owa.len);

  owa.len = sizeof (objprefValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_pref,
                          (xLibU8_t *) & objprefValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprefValue, owa.len);

  /* retrieve key: nextHop */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_nextHop,
                          (xLibU8_t *) & objIpAddressValue, &owa.len);

  owa1.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_RouteTypeflag,
                          (xLibU8_t *) & routeType, &owa1.len);
 

  nextObjrouteIpValue = objrouteIpValue;
  nextObjrouteMaskValue = objrouteMaskValue;
  nextObjprotocolValue = objprotocolValue;
  nextObjprefValue = objprefValue;
  nextObjIpAddressValue = objIpAddressValue;

  memset(&routeEntry, 0x0, sizeof(routeEntry));
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    while ((owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType)) == L7_SUCCESS)
    {
       if((routeEntry.ipAddr == nextObjrouteIpValue) && (routeEntry.subnetMask == nextObjrouteMaskValue) &&
          (routeEntry.protocol == nextObjprotocolValue) && (routeEntry.pref == nextObjprefValue))
       {
          if(routeEntry.flags & L7_RTF_REJECT)
          {
            nextObjIpAddressValue = 0;
            break;
          }
          else
          {
            nextObjIpAddressValue = routeEntry.ecmpRoutes.equalCostPath[0].arpEntry.ipAddr;
            break;
          }
       }
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);
    do
    {
      owa.l7rc = usmDbGetNextEcmpRoute(L7_UNIT_CURRENT,
                                      &nextObjrouteIpValue, &nextObjrouteMaskValue, &nextObjprotocolValue,
                                      &nextObjprefValue, &nextObjIpAddressValue, &nextObjIfindexValue,routeType);
    }
    while ((objrouteIpValue == nextObjrouteIpValue) && (objrouteMaskValue == nextObjrouteMaskValue)
           && (objprotocolValue == nextObjprotocolValue) && (objprefValue == nextObjprefValue)
           && (objIpAddressValue == nextObjIpAddressValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objrouteIpValue != nextObjrouteIpValue) || (objrouteMaskValue != nextObjrouteMaskValue) || (objprotocolValue != nextObjprotocolValue)
      || (objprefValue != nextObjprefValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIpAddressValue, owa.len);

  /* return the object value: IpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIpAddressValue,
                           sizeof (nextObjIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpEcmpRoutes_Ifindex
*
* @purpose Get 'Ifindex'
 *@description  [Ifindex] The outgoing internal interface number.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpEcmpRoutes_Ifindex (void *wap, void *bufp)
{

  xLibIpV4_t objrouteIpValue = 0;
  xLibIpV4_t nextObjrouteIpValue = 0;
  xLibIpV4_t objrouteMaskValue = 0;
  xLibIpV4_t nextObjrouteMaskValue = 0;
  xLibIpV4_t objIpAddressValue = 0;
  xLibIpV4_t nextObjIpAddressValue = 0;
  xLibS32_t objIfindexValue = 0;
  xLibS32_t nextObjIfindexValue = 0;
  xLibU32_t objprotocolValue = 0;
  xLibU32_t nextObjprotocolValue = 0;
  xLibU32_t objprefValue = 0;
  xLibU32_t nextObjprefValue = 0;
  L7_routeEntry_t routeEntry;
  L7_uint32 nh = 0, nextHopFound = L7_FALSE;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibS32_t));
  FPOBJ_TRACE_ENTER (bufp);
  xLibBool_t routeType;
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibBool_t));

  /* retrieve key: routeIp */
  memset(&objrouteIpValue, 0x0, sizeof(objrouteIpValue));
  memset(&nextObjrouteIpValue, 0x0, sizeof(nextObjrouteIpValue));

  memset(&objrouteMaskValue, 0x0, sizeof(objrouteMaskValue));
  memset(&nextObjrouteMaskValue, 0x0, sizeof(nextObjrouteMaskValue));

  memset(&objIpAddressValue, 0x0, sizeof(objIpAddressValue));
  memset(&nextObjIpAddressValue, 0x0, sizeof(nextObjIpAddressValue));

  memset(&objIfindexValue, 0x0, sizeof(objIfindexValue));
  memset(&nextObjIfindexValue, 0x0, sizeof(nextObjIfindexValue));

  memset(&objprotocolValue, 0x0, sizeof(objprotocolValue));
  memset(&nextObjprotocolValue, 0x0, sizeof(nextObjprotocolValue));
  routeType = L7_TRUE;

  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeIp,
                          (xLibU8_t *) & objrouteIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteIpValue, owa.len);

  /* retrieve key: routeMask */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeMask,
                          (xLibU8_t *) & objrouteMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteMaskValue, owa.len);

  owa.len = sizeof (objprotocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_protocol,
                          (xLibU8_t *) & objprotocolValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprotocolValue, owa.len);

  owa.len = sizeof (objprefValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_pref,
                          (xLibU8_t *) & objprefValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprefValue, owa.len);

  /* retrieve key: nextHop */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_nextHop,
                          (xLibU8_t *) & objIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);

  /* retrieve key: Ifindex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_Ifindex,
                          (xLibU8_t *) & objIfindexValue, &owa.len);

  owa1.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_RouteTypeflag,
                          (xLibU8_t *) & routeType, &owa1.len);

  nextObjrouteIpValue = objrouteIpValue;
  nextObjrouteMaskValue = objrouteMaskValue;
  nextObjprotocolValue = objprotocolValue;
  nextObjprefValue = objprefValue;
  nextObjIpAddressValue = objIpAddressValue;
  nextObjIfindexValue = objIfindexValue;

  memset(&routeEntry, 0x0, sizeof(routeEntry));
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    while ((owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType)) == L7_SUCCESS)
    {
       if((routeEntry.ipAddr == nextObjrouteIpValue) && (routeEntry.subnetMask == nextObjrouteMaskValue) && 
          (routeEntry.protocol == nextObjprotocolValue) && (routeEntry.pref == nextObjprefValue))
       {
          if(routeEntry.flags & L7_RTF_REJECT)
          {
            /* This will be the case of a REJECT ROUTE since reject route
             * doesn't have any next hops.
             *
             * But for the SNMP query to proceed with the other valid routes following
             * this route, we shall give the interface number as CPU interface's
             * internal interface number, as the packets with DA best match as
             * a reject route are directed to CPU and get discarded there.
             */
            if(usmDbMgtSwitchintIfNumGet(0, &nextObjIfindexValue) != L7_SUCCESS)
            {
              /* If usmDbMgtSwitchintIfNumGet() fails, we default to 1.
               * This shouldn't generally happen */
              nextObjIfindexValue = 1;
            }
            break;
          }
          else
          {
            for (nh = 0; nh < routeEntry.ecmpRoutes.numOfRoutes; nh++)
            {
              if(routeEntry.ecmpRoutes.equalCostPath[nh].arpEntry.ipAddr == nextObjIpAddressValue)
              {
                nextObjIfindexValue = routeEntry.ecmpRoutes.equalCostPath[nh].arpEntry.intIfNum;
                nextHopFound = L7_TRUE;
                break;
              }
            }
            if(L7_TRUE == nextHopFound)
            {
              break;
            }
          }
       }
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIfindexValue, owa.len);
    /* Only one ifindex per nexthop possible. Return end of table here */
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIfindexValue, owa.len);

  /* return the object value: Ifindex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIfindexValue, sizeof (nextObjIfindexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpEcmpRoutes_metric
*
* @purpose Get 'metric'
 *@description  [metric] Cost associated with this route.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpEcmpRoutes_metric (void *wap, void *bufp)
{
  xLibIpV4_t objrouteIpValue = 0;
  xLibIpV4_t objrouteMaskValue = 0;
  xLibU32_t objprotocolValue = 0;
  xLibS32_t objprefValue = 0;
  xLibS32_t objmetricValue = 0;
  xLibBool_t routeType;
  L7_routeEntry_t routeEntry;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibS32_t));
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibBool_t));

  FPOBJ_TRACE_ENTER (bufp);
  routeType = L7_TRUE;

  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeIp,
                          (xLibU8_t *) & objrouteIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteIpValue, owa.len);

  /* retrieve key: routeMask */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeMask,
                          (xLibU8_t *) & objrouteMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteMaskValue, owa.len);

  owa.len = sizeof (objprotocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_protocol,
                          (xLibU8_t *) & objprotocolValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprotocolValue, owa.len);

  owa.len = sizeof (objprefValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_pref,
                          (xLibU8_t *) & objprefValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprefValue, owa.len);

  owa1.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_RouteTypeflag,
                          (xLibU8_t *) & routeType, &owa1.len);

  memset(&routeEntry, 0x0, sizeof(routeEntry));

  while ((owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType)) == L7_SUCCESS)
  {
     if((routeEntry.ipAddr == objrouteIpValue) && (routeEntry.subnetMask == objrouteMaskValue) && 
        (routeEntry.protocol == objprotocolValue) && (routeEntry.pref == objprefValue))
     {
       break;
     }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objmetricValue = routeEntry.metric;

  /* return the object value: Ifindex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmetricValue, sizeof (objmetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingIpEcmpRoutes_flags
*
* @purpose Get 'flags'
 *@description  [flags] Route flags associated with this route.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingIpEcmpRoutes_flags (void *wap, void *bufp)
{
  xLibIpV4_t objrouteIpValue = 0;
  xLibIpV4_t objrouteMaskValue = 0;
  xLibU32_t objprotocolValue = 0;
  xLibS32_t objprefValue = 0;
  xLibS32_t objflagsValue = 0;
  xLibBool_t routeType;
  L7_routeEntry_t routeEntry;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibS32_t));
  fpObjWa_t owa1 = FPOBJ_INIT_WA (sizeof (xLibBool_t));

  FPOBJ_TRACE_ENTER (bufp);
  routeType = L7_TRUE;

  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeIp,
                          (xLibU8_t *) & objrouteIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteIpValue, owa.len);

  /* retrieve key: routeMask */
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_routeMask,
                          (xLibU8_t *) & objrouteMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteMaskValue, owa.len);

  owa.len = sizeof (objprotocolValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_protocol,
                          (xLibU8_t *) & objprotocolValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprotocolValue, owa.len);

  owa.len = sizeof (objprefValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_pref,
                          (xLibU8_t *) & objprefValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprefValue, owa.len);

  owa1.rc = xLibFilterGet (wap, XOBJ_routingIpEcmpRoutes_RouteTypeflag,
                          (xLibU8_t *) & routeType, &owa1.len);

  memset(&routeEntry, 0x0, sizeof(routeEntry));

  while ((owa.l7rc = usmDbNextRouteEntryGet(L7_UNIT_CURRENT, &routeEntry, routeType)) == L7_SUCCESS)
  {
     if((routeEntry.ipAddr == objrouteIpValue) && (routeEntry.subnetMask == objrouteMaskValue) && 
        (routeEntry.protocol == objprotocolValue) && (routeEntry.pref == objprefValue))
     {
       break;
     }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objflagsValue = routeEntry.flags;

  /* return the object value: Ifindex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objflagsValue, sizeof (objflagsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingIpEcmpRoutes_RouteTypeflag
*
* @purpose Set 'RouteTypeflag'
 *@description  [RouteTypeflag] This flag tells to application call get the best
* route or not
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingIpEcmpRoutes_RouteTypeflag (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRouteTypeflagValue;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve object: RouteTypeflag */
  owa.len = sizeof (objRouteTypeflagValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRouteTypeflagValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRouteTypeflagValue, owa.len);

  /* if row status object is specified and eual to delete return success */
  owa.l7rc = L7_SUCCESS;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

