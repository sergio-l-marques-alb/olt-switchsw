
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTdvmrpRouteTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to IPMCAST-object.xml
*
* @create  01 May 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_IPMCASTdvmrpRouteTable_obj.h"
#include "usmdb_mib_dvmrp_api.h"


/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteSource
*
* @purpose Get 'dvmrpRouteSource'
*
* @description [dvmrpRouteSource] The network address which when combined with the corresponding value of dvmrpRouteSourceMask identifies the sources for which this entry contains multicast routing information.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteSource (void *wap, void *bufp)
{

  L7_inet_addr_t objdvmrpRouteSourceValue;
  L7_inet_addr_t nextObjdvmrpRouteSourceValue;
  L7_inet_addr_t nextObjdvmrpRouteSourceMaskValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpRouteSource */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSource,
                          (xLibU8_t *) & objdvmrpRouteSourceValue, &owa.len);
  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteSourceValue);
  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteSourceMaskValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    
    owa.l7rc = usmDbDvmrpRouteEntryNextGet(L7_UNIT_CURRENT,
                                           &nextObjdvmrpRouteSourceValue,
                                           &nextObjdvmrpRouteSourceMaskValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpRouteSourceValue, owa.len);
   
    do
    {
      owa.l7rc = usmDbDvmrpRouteEntryNextGet (L7_UNIT_CURRENT,
                                              &nextObjdvmrpRouteSourceValue,
                                              &nextObjdvmrpRouteSourceMaskValue);
    }
    while (memcmp(&objdvmrpRouteSourceValue, &nextObjdvmrpRouteSourceValue,sizeof(L7_inet_addr_t)) 
                              && (owa.l7rc == L7_SUCCESS));
    if (owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbDvmrpRouteEntryNextGet (L7_UNIT_CURRENT,
                                              &nextObjdvmrpRouteSourceValue,
                                              &nextObjdvmrpRouteSourceMaskValue);
    }
    else
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpRouteSourceValue, owa.len);

  /* return the object value: dvmrpRouteSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpRouteSourceValue,
                           sizeof (objdvmrpRouteSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteSourceMask
*
* @purpose Get 'dvmrpRouteSourceMask'
*
* @description [dvmrpRouteSourceMask] The network mask which when combined with the corresponding value of dvmrpRouteSource identifies the sources for which this entry contains multicast routing information.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteSourceMask (void *wap, void *bufp)
{

  L7_inet_addr_t objdvmrpRouteSourceValue;
  L7_inet_addr_t nextObjdvmrpRouteSourceValue;
  L7_inet_addr_t objdvmrpRouteSourceMaskValue;
  L7_inet_addr_t nextObjdvmrpRouteSourceMaskValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpRouteSource */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSource,
                          (xLibU8_t *) & objdvmrpRouteSourceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpRouteSourceValue, owa.len);

  /* retrieve key: dvmrpRouteSourceMask */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSourceMask,
                          (xLibU8_t *) & objdvmrpRouteSourceMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteSourceValue);
    inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpRouteSourceMaskValue);
    do 
    {
      owa.l7rc = usmDbDvmrpRouteEntryNextGet(L7_UNIT_CURRENT,                
                                             &nextObjdvmrpRouteSourceValue,
                                             &nextObjdvmrpRouteSourceMaskValue);
    } while( (memcmp(&objdvmrpRouteSourceValue, &nextObjdvmrpRouteSourceValue, sizeof(L7_inet_addr_t))) 
            && (owa.l7rc == L7_SUCCESS));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpRouteSourceMaskValue, owa.len);
    memcpy(&nextObjdvmrpRouteSourceValue, &objdvmrpRouteSourceValue, sizeof(L7_inet_addr_t));
    memcpy(&nextObjdvmrpRouteSourceMaskValue, &objdvmrpRouteSourceMaskValue, sizeof(L7_inet_addr_t));

    owa.l7rc =usmDbDvmrpRouteEntryNextGet (L7_UNIT_CURRENT,
                                           &nextObjdvmrpRouteSourceValue,
                                           &nextObjdvmrpRouteSourceMaskValue);
  }

  if (memcmp(&objdvmrpRouteSourceValue, &nextObjdvmrpRouteSourceValue,sizeof(L7_inet_addr_t)) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpRouteSourceMaskValue, owa.len);

  /* return the object value: dvmrpRouteSourceMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpRouteSourceMaskValue,
                           sizeof (objdvmrpRouteSourceMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteUpstreamNeighbor
*
* @purpose Get 'dvmrpRouteUpstreamNeighbor'
*
* @description [dvmrpRouteUpstreamNeighbor] The address of the upstream neighbor (e.g., RPF neighbor) from which IP datagrams from these sources are received.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteUpstreamNeighbor (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceValue;
  fpObjWa_t kwadvmrpRouteSourceMask = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objdvmrpRouteUpstreamNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpRouteSource */
  kwadvmrpRouteSource.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSource,
                                          (xLibU8_t *) & keydvmrpRouteSourceValue,
                                          &kwadvmrpRouteSource.len);
  if (kwadvmrpRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSource);
    return kwadvmrpRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceValue, kwadvmrpRouteSource.len);

  /* retrieve key: dvmrpRouteSourceMask */
  kwadvmrpRouteSourceMask.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSourceMask,
                                              (xLibU8_t *) & keydvmrpRouteSourceMaskValue,
                                              &kwadvmrpRouteSourceMask.len);
  if (kwadvmrpRouteSourceMask.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSourceMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSourceMask);
    return kwadvmrpRouteSourceMask.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceMaskValue, kwadvmrpRouteSourceMask.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpRouteUpstreamNeighborGet (L7_UNIT_CURRENT, &keydvmrpRouteSourceValue,
                              &keydvmrpRouteSourceMaskValue, &objdvmrpRouteUpstreamNeighborValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objdvmrpRouteUpstreamNeighborValue.family = L7_AF_INET;
  /* return the object value: dvmrpRouteUpstreamNeighbor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpRouteUpstreamNeighborValue,
                           sizeof (objdvmrpRouteUpstreamNeighborValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteIfIndex
*
* @purpose Get 'dvmrpRouteIfIndex'
*
* @description [dvmrpRouteIfIndex] The value of ifIndex for the interface on which IP datagrams sent by these sources are received. A value of 0 typically means the route is an aggregate for which no next- hop interface exists.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteIfIndex (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceValue;
  fpObjWa_t kwadvmrpRouteSourceMask = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpRouteIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpRouteSource */
  kwadvmrpRouteSource.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSource,
                                          (xLibU8_t *) & keydvmrpRouteSourceValue,
                                          &kwadvmrpRouteSource.len);
  if (kwadvmrpRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSource);
    return kwadvmrpRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceValue, kwadvmrpRouteSource.len);

  /* retrieve key: dvmrpRouteSourceMask */
  kwadvmrpRouteSourceMask.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSourceMask,
                                              (xLibU8_t *) & keydvmrpRouteSourceMaskValue,
                                              &kwadvmrpRouteSourceMask.len);
  if (kwadvmrpRouteSourceMask.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSourceMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSourceMask);
    return kwadvmrpRouteSourceMask.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceMaskValue, kwadvmrpRouteSourceMask.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpRouteIfIndexGet (L7_UNIT_CURRENT, &keydvmrpRouteSourceValue,
                              &keydvmrpRouteSourceMaskValue, &objdvmrpRouteIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpRouteIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpRouteIfIndexValue,
                           sizeof (objdvmrpRouteIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteMetric
*
* @purpose Get 'dvmrpRouteMetric'
*
* @description [dvmrpRouteMetric] The distance in hops to the source subnet.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteMetric (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceValue;
  fpObjWa_t kwadvmrpRouteSourceMask = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU16_t));
  xLibU16_t objdvmrpRouteMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpRouteSource */
  kwadvmrpRouteSource.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSource,
                                          (xLibU8_t *) & keydvmrpRouteSourceValue,
                                          &kwadvmrpRouteSource.len);
  if (kwadvmrpRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSource);
    return kwadvmrpRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceValue, kwadvmrpRouteSource.len);

  /* retrieve key: dvmrpRouteSourceMask */
  kwadvmrpRouteSourceMask.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSourceMask,
                                              (xLibU8_t *) & keydvmrpRouteSourceMaskValue,
                                              &kwadvmrpRouteSourceMask.len);
  if (kwadvmrpRouteSourceMask.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSourceMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSourceMask);
    return kwadvmrpRouteSourceMask.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceMaskValue, kwadvmrpRouteSourceMask.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpRouteMetricGet (L7_UNIT_CURRENT, &keydvmrpRouteSourceValue,
                              &keydvmrpRouteSourceMaskValue, &objdvmrpRouteMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpRouteMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpRouteMetricValue,
                           sizeof (objdvmrpRouteMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteExpiryTime
*
* @purpose Get 'dvmrpRouteExpiryTime'
*
* @description [dvmrpRouteExpiryTime] The minimum amount of time remaining before this entry will be aged out.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteExpiryTime (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceValue;
  fpObjWa_t kwadvmrpRouteSourceMask = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibStr256_t objdvmrpRouteExpiryTimeValue;
  xLibUL32_t RouteExpiryTimeValue;
  L7_timespec timeSpec;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpRouteSource */
  kwadvmrpRouteSource.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSource,
                                          (xLibU8_t *) & keydvmrpRouteSourceValue,
                                          &kwadvmrpRouteSource.len);
  if (kwadvmrpRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSource);
    return kwadvmrpRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceValue, kwadvmrpRouteSource.len);

  /* retrieve key: dvmrpRouteSourceMask */
  kwadvmrpRouteSourceMask.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSourceMask,
                                              (xLibU8_t *) & keydvmrpRouteSourceMaskValue,
                                              &kwadvmrpRouteSourceMask.len);
  if (kwadvmrpRouteSourceMask.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSourceMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSourceMask);
    return kwadvmrpRouteSourceMask.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceMaskValue, kwadvmrpRouteSourceMask.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpRouteExpiryTimeGet (L7_UNIT_CURRENT, &keydvmrpRouteSourceValue,
                              &keydvmrpRouteSourceMaskValue, &RouteExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiConvertRawUpTime(RouteExpiryTimeValue,(L7_timespec *)&timeSpec);
  osapiSnprintf(objdvmrpRouteExpiryTimeValue, sizeof(objdvmrpRouteExpiryTimeValue), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);

  /* return the object value: dvmrpRouteExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpRouteExpiryTimeValue,
                           sizeof (objdvmrpRouteExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteUpTime
*
* @purpose Get 'dvmrpRouteUpTime'
*
* @description [dvmrpRouteUpTime] The time since the route represented by this entry was learned by the router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpRouteTable_dvmrpRouteUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceValue;
  fpObjWa_t kwadvmrpRouteSourceMask = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpRouteSourceMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibStr256_t objdvmrpRouteUpTimeValue;
  xLibUL32_t RouteUpTimeValue;
  L7_timespec timeSpec;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpRouteSource */
  kwadvmrpRouteSource.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSource,
                                          (xLibU8_t *) & keydvmrpRouteSourceValue,
                                          &kwadvmrpRouteSource.len);
  if (kwadvmrpRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSource);
    return kwadvmrpRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceValue, kwadvmrpRouteSource.len);

  /* retrieve key: dvmrpRouteSourceMask */
  kwadvmrpRouteSourceMask.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpRouteTable_dvmrpRouteSourceMask,
                                              (xLibU8_t *) & keydvmrpRouteSourceMaskValue,
                                              &kwadvmrpRouteSourceMask.len);
  if (kwadvmrpRouteSourceMask.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpRouteSourceMask.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpRouteSourceMask);
    return kwadvmrpRouteSourceMask.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpRouteSourceMaskValue, kwadvmrpRouteSourceMask.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpRouteUpTimeGet (L7_UNIT_CURRENT, &keydvmrpRouteSourceValue,
                              &keydvmrpRouteSourceMaskValue, &RouteUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiConvertRawUpTime(RouteUpTimeValue,(L7_timespec *)&timeSpec);
  osapiSnprintf(objdvmrpRouteUpTimeValue, sizeof(objdvmrpRouteUpTimeValue), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);
  /* return the object value: dvmrpRouteUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpRouteUpTimeValue,
                           sizeof (objdvmrpRouteUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
