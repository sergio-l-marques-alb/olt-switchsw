
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingipstaticrouteconfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  28 August 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingipstaticrouteconfig_obj.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "usmdb_nim_api.h"

/*******************************************************************************
* @function fpObjGet_routingipstaticrouteconfig_ipaddress
*
* @purpose Get 'ipaddress'
 *@description  [ipaddress] Ip Address of the static route.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipstaticrouteconfig_ipaddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibIpV4_t nextObjipaddressValue = 0;
  xLibIpV4_t objipaddressValue = 0;

  xLibIpV4_t objipmaskValue = 0;
  xLibU32_t objpreferenceValue = 0;
  xLibU32_t objnhIfNum = 0;
  xLibIpV4_t objnexthopipaddressValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipaddress */
  memset(&objipaddressValue, 0x0, sizeof(objipaddressValue));
  memset(&nextObjipaddressValue, 0x0, sizeof(nextObjipaddressValue));
  memset(&objipmaskValue, 0x0, sizeof(objipmaskValue));
  memset(&objnexthopipaddressValue, 0x0, sizeof(objnexthopipaddressValue));

  owa.len = sizeof (objipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipaddress,
                          (xLibU8_t *) &objipaddressValue, &owa.len);
  
  nextObjipaddressValue = objipaddressValue;

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetNextStaticRoute(&nextObjipaddressValue,
                                     &objipmaskValue, &objpreferenceValue,
                                     &objnexthopipaddressValue, &objnhIfNum);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objipaddressValue, owa.len);
    do
    {
      owa.l7rc = usmDbGetNextStaticRoute(&nextObjipaddressValue,
                                      &objipmaskValue, &objpreferenceValue,
                                      &objnexthopipaddressValue, &objnhIfNum);
    }
    while ((objipaddressValue == nextObjipaddressValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjipaddressValue, owa.len);

  /* return the object value: ipaddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjipaddressValue,
                           sizeof(nextObjipaddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingipstaticrouteconfig_ipmask
*
* @purpose Get 'ipmask'
 *@description  [ipmask] network mask of the static route.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipstaticrouteconfig_ipmask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibIpV4_t objipaddressValue = 0;
  xLibIpV4_t nextObjipaddressValue = 0;
  xLibIpV4_t objipmaskValue = 0;
  xLibU32_t objpreferenceValue = 0;
  xLibU32_t objnhIfNum = 0;
  xLibIpV4_t nextObjipmaskValue = 0;
  xLibIpV4_t objnexthopipaddressValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  memset(&objipaddressValue, 0x0, sizeof(objipaddressValue));
  memset(&nextObjipaddressValue, 0x0, sizeof(nextObjipaddressValue));
  memset(&objipmaskValue, 0x0, sizeof(objipmaskValue));
  memset(&nextObjipmaskValue, 0x0, sizeof(nextObjipmaskValue));
  memset(&objnexthopipaddressValue, 0x0, sizeof(objnexthopipaddressValue));

  /* retrieve key: ipaddress */
  owa.len = sizeof (objipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipaddress,
                          (xLibU8_t *) &objipaddressValue, &owa.len);

  nextObjipaddressValue = objipaddressValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objipaddressValue, owa.len);

  /* retrieve key: ipmask */
  owa.len = sizeof (objipmaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipmask,
                          (xLibU8_t *) &objipmaskValue, &owa.len);

  nextObjipaddressValue = objipaddressValue;
  nextObjipmaskValue = objipmaskValue;

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetNextStaticRoute(&nextObjipaddressValue,
                                     &nextObjipmaskValue, &objpreferenceValue,
                                     &objnexthopipaddressValue, &objnhIfNum);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objipmaskValue, owa.len);
    do
    {
    owa.l7rc = usmDbGetNextStaticRoute (&nextObjipaddressValue,
                                     &nextObjipmaskValue, &objpreferenceValue,
                                     &objnexthopipaddressValue, &objnhIfNum);
    }
    while ((objipaddressValue == nextObjipaddressValue)
           && (objipmaskValue == nextObjipmaskValue) && (owa.l7rc == L7_SUCCESS));
  }

  if (((objipaddressValue != nextObjipaddressValue)) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjipmaskValue, owa.len);

  /* return the object value: ipmask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)&nextObjipmaskValue, sizeof(nextObjipmaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingipstaticrouteconfig_preference
*
* @purpose Get 'preference'
 *@description  [preference] The preference of this static route entry
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipstaticrouteconfig_preference (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibIpV4_t objipaddressValue = 0;
  xLibIpV4_t nextObjipaddressValue = 0;
  xLibIpV4_t objipmaskValue = 0;
  xLibIpV4_t nextObjipmaskValue = 0;
  xLibU32_t objpreferenceValue = 0;
  xLibU32_t objnhIfNum = 0;
  xLibU32_t nextObjpreferenceValue = 0;
  xLibIpV4_t nextObjnexthopipaddressValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipaddress */
  owa.len = sizeof (objipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipaddress,
                          (xLibU8_t *) &objipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objipaddressValue, owa.len);

  /* retrieve key: ipmask */
  owa.len = sizeof (objipmaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipmask,
                          (xLibU8_t *) &objipmaskValue, &owa.len);

 
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objipmaskValue, owa.len);

  /* retrieve key: preference */
  owa.len = sizeof (objpreferenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_preference,
                          (xLibU8_t *) & objpreferenceValue, &owa.len);

  nextObjipaddressValue = objipaddressValue;
  nextObjipmaskValue = objipmaskValue;
  nextObjpreferenceValue = objpreferenceValue;

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetNextStaticRoute(&nextObjipaddressValue,
                                     &nextObjipmaskValue, &nextObjpreferenceValue,
                                     &nextObjnexthopipaddressValue, &objnhIfNum);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpreferenceValue, owa.len);

    do
    {
      owa.l7rc = usmDbGetNextStaticRoute(&nextObjipaddressValue,
                                       &nextObjipmaskValue, &nextObjpreferenceValue,
                                       &nextObjnexthopipaddressValue, &objnhIfNum);
    }
    while ((objipaddressValue == nextObjipaddressValue)
           && (objipmaskValue == nextObjipmaskValue)
           && (objpreferenceValue == nextObjpreferenceValue) && (owa.l7rc == L7_SUCCESS));
  }

  if (((objipaddressValue  != nextObjipaddressValue))
      || ((objipmaskValue !=  nextObjipmaskValue)) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpreferenceValue, owa.len);

  /* return the object value: nexthopipaddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpreferenceValue,
                           sizeof (nextObjpreferenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_routingipstaticrouteconfig_nexthopipaddress
*
* @purpose Get 'nexthopipaddress'
 *@description  [nexthopipaddress] The ip address of the next-hop-router
* associated with static routing entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipstaticrouteconfig_nexthopipaddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibIpV4_t objipaddressValue = 0;
  xLibIpV4_t nextObjipaddressValue = 0;
  xLibIpV4_t objipmaskValue = 0;
  xLibIpV4_t nextObjipmaskValue = 0;
  xLibU32_t objpreferenceValue = 0;
  xLibU32_t objnhIfNum = 0;
  xLibU32_t nextObjpreferenceValue = 0;
  xLibIpV4_t objnexthopipaddressValue = 0;
  xLibIpV4_t nextObjnexthopipaddressValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  memset(&objipaddressValue, 0x0, sizeof(objipaddressValue));
  memset(&nextObjipaddressValue, 0x0, sizeof(nextObjipaddressValue));
  memset(&objipmaskValue, 0x0, sizeof(objipmaskValue));
  memset(&nextObjipmaskValue, 0x0, sizeof(nextObjipmaskValue));
  memset(&objnexthopipaddressValue, 0x0, sizeof(objnexthopipaddressValue));
  memset(&nextObjnexthopipaddressValue, 0x0, sizeof(nextObjnexthopipaddressValue));

  /* retrieve key: ipaddress */
  owa.len = sizeof (objipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipaddress,
                          (xLibU8_t *) &objipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objipaddressValue, owa.len);

  /* retrieve key: ipmask */
  owa.len = sizeof (objipmaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipmask,
                          (xLibU8_t *) &objipmaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objipmaskValue, owa.len);

  /* retrieve key: preference */
  owa.len = sizeof (objpreferenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_preference,
                          (xLibU8_t *) & objpreferenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objpreferenceValue, owa.len);

  /* retrieve key: nexthopipaddress */
  owa.len = sizeof (objnexthopipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_nexthopipaddress,
                          (xLibU8_t *) & objnexthopipaddressValue, &owa.len);

  nextObjipaddressValue = objipaddressValue;
  nextObjipmaskValue = objipmaskValue;
  nextObjpreferenceValue = objpreferenceValue;
  nextObjnexthopipaddressValue = objnexthopipaddressValue;

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetNextStaticRoute(&nextObjipaddressValue,
                                     &nextObjipmaskValue, &nextObjpreferenceValue,
                                     &nextObjnexthopipaddressValue, &objnhIfNum);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objnexthopipaddressValue, owa.len);

    do
    {
      owa.l7rc = usmDbGetNextStaticRoute(&nextObjipaddressValue,
                                       &nextObjipmaskValue, &nextObjpreferenceValue,
                                       &nextObjnexthopipaddressValue, &objnhIfNum);
    }
    while ((objipaddressValue == nextObjipaddressValue)
           && (objipmaskValue == nextObjipmaskValue)
           && (objpreferenceValue == nextObjpreferenceValue) && (objnexthopipaddressValue == nextObjnexthopipaddressValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objipaddressValue  != nextObjipaddressValue)
      || (objipmaskValue !=  nextObjipmaskValue) || (objpreferenceValue != nextObjpreferenceValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjnexthopipaddressValue, owa.len);

  /* return the object value: nexthopipaddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjnexthopipaddressValue,
                           sizeof (nextObjnexthopipaddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipstaticrouteconfig_ifindex
*
* @purpose Get 'ifindex'
 *@description  [ifindex] The outgoing internal interface number.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipstaticrouteconfig_ifindex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objifindexValue = 0;

  xLibIpV4_t keyipaddressValue = 0;
  xLibIpV4_t keyipmaskValue = 0;
  xLibU32_t keypreferenceValue = 0;
  xLibIpV4_t keynexthopipaddressValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipaddress */
  owa.len = sizeof (keyipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipaddress,
                          (xLibU8_t *) &keyipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipaddressValue, owa.len);

  /* retrieve key: ipmask */
  owa.len = sizeof (keyipmaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipmask,
                          (xLibU8_t *) &keyipmaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipmaskValue, owa.len);

  /* retrieve key: preference */
  owa.len = sizeof (keypreferenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_preference,
                          (xLibU8_t *) & keypreferenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypreferenceValue, owa.len);

  /* retrieve key: nexthopipaddress */
  owa.len = sizeof (keynexthopipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_nexthopipaddress,
                          (xLibU8_t *) & keynexthopipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynexthopipaddressValue, owa.len);

  /* get the value from application */
  owa.l7rc =  usmDbStaticRouteIfindexGet(keyipaddressValue, keyipmaskValue,
                                         keypreferenceValue, keynexthopipaddressValue, &objifindexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objifindexValue, sizeof (objifindexValue));

  /* return the object value: ifindex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifindexValue, sizeof (objifindexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingipstaticrouteconfig_ifindex
*
* @purpose Set 'ifindex'
 *@description  [ifindex] The outgoing internal interface number.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipstaticrouteconfig_ifindex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objifindexValue = 0;

  xLibIpV4_t keyipaddressValue = 0;
  xLibIpV4_t keyipmaskValue = 0;
  xLibIpV4_t keynexthopipaddressValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifindex */
  owa.len = sizeof (objifindexValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objifindexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifindexValue, owa.len);

  /* retrieve key: ipaddress */
  owa.len = sizeof (keyipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipaddress,
                          (xLibU8_t *) &keyipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipaddressValue, owa.len);

  /* retrieve key: ipmask */
  owa.len = sizeof (keyipmaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipmask,
                          (xLibU8_t *) &keyipmaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipmaskValue, owa.len);

  /* retrieve key: nexthopipaddress */
  owa.len = sizeof (keynexthopipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_nexthopipaddress,
                          (xLibU8_t *) & keynexthopipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynexthopipaddressValue, owa.len);

  owa.l7rc = L7_SUCCESS;

  /* set the value in application 
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyipaddressValue,
                              keyipmaskValue, keynexthopipaddressValue, objifindexValue);
 */

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipstaticrouteconfig_preference
*
* @purpose Set 'preference'
 *@description  [preference] The preference associated with this route.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipstaticrouteconfig_preference (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objpreferenceValue = 0;

  xLibIpV4_t keyipaddressValue = 0;
  xLibIpV4_t keyipmaskValue = 0;
  xLibIpV4_t keynexthopipaddressValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: preference */
  owa.len = sizeof (objpreferenceValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpreferenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpreferenceValue, owa.len);

  /* retrieve key: ipaddress */
  owa.len = sizeof (keyipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipaddress,
                          (xLibU8_t *) &keyipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipaddressValue, owa.len);

  /* retrieve key: ipmask */
  owa.len = sizeof (keyipmaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipmask,
                          (xLibU8_t *) &keyipmaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipmaskValue, owa.len);

  /* retrieve key: nexthopipaddress */
  owa.len = sizeof (keynexthopipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_nexthopipaddress,
                          (xLibU8_t *) &keynexthopipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynexthopipaddressValue, owa.len);

  owa.l7rc = L7_SUCCESS;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingipstaticrouteconfig_routeType
*
* @purpose Get 'routeType'
 *@description  [routeType] The preference associated with this route.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipstaticrouteconfig_routeType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objrouteTypeValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application 
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objrouteTypeValue);
*/
  owa.l7rc = L7_SUCCESS;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objrouteTypeValue, sizeof (objrouteTypeValue));

  /* return the object value: routeType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objrouteTypeValue, sizeof (objrouteTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_routingipstaticrouteconfig_routeType
*
* @purpose List 'routeType'
 *@description  [routeType] The preference associated with this route.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_routingipstaticrouteconfig_routeType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objrouteTypeValue = 0;
  xLibU32_t nextObjrouteTypeValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objrouteTypeValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_routeType,
                          (xLibU8_t *) & objrouteTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = L7_SUCCESS;
    nextObjrouteTypeValue = L7_XUI_ROUTE_TYPE_ROUTRTF_DEFAULT_ROUTE;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objrouteTypeValue, owa.len);
    owa.l7rc = L7_SUCCESS;
    
    if(objrouteTypeValue == L7_XUI_ROUTE_TYPE_ROUTRTF_DEFAULT_ROUTE)
    {
       nextObjrouteTypeValue = L7_XUI_ROUTE_TYPE_RTF_STATIC;  
    }
    else if(objrouteTypeValue == L7_XUI_ROUTE_TYPE_RTF_STATIC)
    {
       nextObjrouteTypeValue = L7_XUI_ROUTE_TYPE_RTF_REJEC;  
    } 
    else if(objrouteTypeValue == L7_XUI_ROUTE_TYPE_RTF_REJEC)
    {
      owa.l7rc = L7_FAILURE;
    } 
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjrouteTypeValue, owa.len);

  /* return the object value: routeType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjrouteTypeValue,
                           sizeof (nextObjrouteTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_routingipstaticrouteconfig_routeType
*
* @purpose Set 'routeType'
 *@description  [routeType] The preference associated with this route.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipstaticrouteconfig_routeType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objrouteTypeValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: routeType */
  owa.len = sizeof (objrouteTypeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objrouteTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objrouteTypeValue, owa.len);

  owa.l7rc = L7_SUCCESS;

  /* if row status object is specified and eual to delete return success */

  /* set the value in application 
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objrouteTypeValue);
*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingipstaticrouteconfig_RowStatus
*
* @purpose Get 'RowStatus'
 *@description  [RowStatus] This variable displays the status of the en-try.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingipstaticrouteconfig_RowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue = 0;

  xLibIpV4_t keyipaddressValue = 0;
  xLibIpV4_t keyipmaskValue = 0;
  xLibIpV4_t keynexthopipaddressValue = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipaddress */
  owa.len = sizeof (keyipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipaddress,
                          (xLibU8_t *) &keyipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipaddressValue, owa.len);

  /* retrieve key: ipmask */
  owa.len = sizeof (keyipmaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipmask,
                          (xLibU8_t *) &keyipmaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipmaskValue, owa.len);

  /* retrieve key: nexthopipaddress */
  owa.len = sizeof (keynexthopipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_nexthopipaddress,
                          (xLibU8_t *) & keynexthopipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynexthopipaddressValue, owa.len);

  owa.l7rc = L7_SUCCESS;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, sizeof (objRowStatusValue));

  /* return the object value: RowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRowStatusValue, sizeof (objRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingipstaticrouteconfig_RowStatus
*
* @purpose Set 'RowStatus'
 *@description  [RowStatus] This variable displays the status of the en-try.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingipstaticrouteconfig_RowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRowStatusValue = 0;
  xLibU32_t objpreferenceValue = 0;
  xLibU32_t objrouteTypeValue = 0;

  xLibIpV4_t keyipaddressValue = 0;
  xLibIpV4_t keyipmaskValue = 0;
  xLibIpV4_t keynexthopipaddressValue = 0;
  L7_RC_t rc;
  L7_uint32 pref = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RowStatus */
  owa.len = sizeof (objRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRowStatusValue, owa.len);

  /* retrieve key: ipaddress */
  owa.len = sizeof (keyipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipaddress,
                          (xLibU8_t *) &keyipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipaddressValue, owa.len);

  /* retrieve key: ipmask */
  owa.len = sizeof (keyipmaskValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_ipmask,
                          (xLibU8_t *) &keyipmaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipmaskValue, owa.len);

  /* retrieve key: nexthopipaddress */
  owa.len = sizeof (keynexthopipaddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_nexthopipaddress,
                          (xLibU8_t *) & keynexthopipaddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynexthopipaddressValue, owa.len);

  owa.len = sizeof (objpreferenceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingipstaticrouteconfig_preference,
                          (xLibU8_t *) & objpreferenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objpreferenceValue == 0)
  {
     rc = usmDbIpRouterPreferenceGet(L7_UNIT_CURRENT, ROUTE_PREF_STATIC, &pref);
     if(rc != L7_SUCCESS)
     {
        pref =  FD_RTR_RTO_PREF_STATIC;
     }
  }
  else
  {
     pref = objpreferenceValue;
  }
 

  owa.l7rc = L7_SUCCESS;

  if(keynexthopipaddressValue == 0)
  {
    objrouteTypeValue = L7_RTF_REJECT;
  }
  else
  {
    objrouteTypeValue = 0;
  }
  if(keyipaddressValue == 0)
  {
    objrouteTypeValue = L7_RTF_DEFAULT_ROUTE;
  }
  if (objRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    if (L7_RTF_DEFAULT_ROUTE != objrouteTypeValue)
    {
       if (usmDbNetmaskIsContiguous(keyipmaskValue) != L7_TRUE) 
       {
          owa.rc = XLIBRC_INVALID_SUBNET_MASK;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
       }
       if (usmDbIpStaticRouteAddrIsValid(L7_UNIT_CURRENT, keyipaddressValue, keyipmaskValue) != L7_TRUE)
       {
         owa.rc = XLIBRC_INVALID_STATIC_ADDRESS;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
       } 
    }
    if ((keynexthopipaddressValue != 0 ) && (usmDbNetworkAddressValidate(keynexthopipaddressValue) != L7_SUCCESS))
    {
       owa.rc = XLIBRC_INVALID_NEXTHOP_ADDRESS;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;     
    }
    owa.l7rc = usmDbIpStaticRouteAdd (L7_UNIT_CURRENT, keyipaddressValue,
               keyipmaskValue, keynexthopipaddressValue, pref, L7_INVALID_INTF, objrouteTypeValue);


     if (owa.l7rc != L7_SUCCESS)
     {
       switch (owa.l7rc)
       {
         case L7_ERROR:
         owa.rc = XLIBRC_MAX_NEXTHOPS_EXCEEDED;
         break;

         case L7_REQUEST_DENIED:
         owa.rc = XLIBRC_INVALID_NEXTHOP;
         break;

          case L7_TABLE_IS_FULL:
          owa.rc = XLIBRC_DEFAULT_ROUTE_TABLE_FULL;
          break;

          case L7_NOT_EXIST:
          owa.rc = XLIBRC_ROUTE_ADD_MESSAGE;
          break;

          default:
          owa.rc = XLIBRC_COULD_NOT_ADD_DEFAULT_ROUTE;
          break;
       }
     }
  }
  else if (objRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbIpStaticRouteDelete(L7_UNIT_CURRENT, keyipaddressValue,
                                   keyipmaskValue, keynexthopipaddressValue, L7_INVALID_INTF, objrouteTypeValue);
     if (owa.l7rc != L7_SUCCESS)
     {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
