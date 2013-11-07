
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2010
*
********************************************************************************
*
* @filename fpobj_sdmTemplateConfiguration.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to sdmtemplate-object.xml
*
* @create  26 March 2010, Friday
*
* @notes   
*
* @author  Tulasiram 
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_sdmTemplateConfiguration_obj.h"
#include "usmdb_sdm.h"


/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_Current
*
* @purpose Get 'Current SDM Template ID'
 *@description  [Current] To Display the current SDM Template ID.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_current (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objCurrentValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  objCurrentValue = usmDbSdmActiveTemplateGet();
  
  FPOBJ_TRACE_VALUE (bufp, &objCurrentValue, sizeof(objCurrentValue));

  /* return the object value: Current */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objCurrentValue,
                          sizeof(objCurrentValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_next
*
* @purpose Get 'Next SDM Template ID'
 *@description  [next] To Get the Next SDM Template ID.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_next (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objnextValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* get the value from application */
  objnextValue = usmDbSdmNextActiveTemplateGet();
  
  FPOBJ_TRACE_VALUE (bufp, &objnextValue, sizeof(objnextValue));

  /* return the object value: next */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objnextValue,
                          sizeof(objnextValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_sdmTemplateConfiguration_next
*
* @purpose Set 'Next SDM Template'
 *@description  [next] To Set the Next SDM Template.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_sdmTemplateConfiguration_next (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objnextValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: next */
  owa.len = sizeof(objnextValue);
  owa.rc = xLibBufDataGet(bufp,
              (xLibU8_t *)&objnextValue,
              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objnextValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  
  owa.l7rc = usmDbSdmNextActiveTemplateSet(objnextValue);
  
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */   
  }
  
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_ID
*
* @purpose Get 'ID'
 *@description  [ID] To Display the Summary of SDM Template ID.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_ID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t nextObjIDValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: ID */
  owa.len = sizeof(nextObjIDValue);
  owa.rc = xLibFilterGet(wap, XOBJ_sdmTemplateConfiguration_ID,
                         (xLibU8_t *) &nextObjIDValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY(bufp, NULL, 0);
    nextObjIDValue = 0;
    owa.l7rc = usmDbSdmTemplateIdNextGet(&nextObjIDValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY(bufp, &nextObjIDValue, owa.len);
    owa.l7rc = usmDbSdmTemplateIdNextGet(&nextObjIDValue);
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_NEW_KEY(bufp, &nextObjIDValue, owa.len);

  /* return the object value: ID */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&nextObjIDValue,
                          sizeof(nextObjIDValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_ARPEntries
*
* @purpose Get 'ARPEntries'
 *@description  [ARPEntries] The maximum number of entries in the IPv4 Address
* Resolution Protocol (ARP) cache for routing interfaces.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_ARPEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objARPEntriesValue;
  xLibU32_t keyIDValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: ID */
  owa.len = sizeof(keyIDValue);
  owa.rc = xLibFilterGet(wap, XOBJ_sdmTemplateConfiguration_ID,
                              (xLibU8_t *) &keyIDValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyIDValue, owa.len);

  /* get the value from application */
  objARPEntriesValue = usmDbSdmTemplateMaxArpEntriesGet(keyIDValue);
  FPOBJ_TRACE_VALUE (bufp, &objARPEntriesValue, sizeof(objARPEntriesValue));

  /* return the object value: ARPEntries */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objARPEntriesValue,
                          sizeof(objARPEntriesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_IPv4UnicastRoutes
*
* @purpose Get 'IPv4UnicastRoutes'
 *@description  [IPv4UnicastRoutes] The maximum number of IPv4 unicast
* forwarding table entries.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_IPv4UnicastRoutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objIPv4UnicastRoutesValue;
  xLibU32_t keyIDValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: ID */
  owa.len = sizeof(keyIDValue);
  owa.rc = xLibFilterGet(wap, XOBJ_sdmTemplateConfiguration_ID,
                              (xLibU8_t *) &keyIDValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyIDValue, owa.len);

  /* get the value from application */
  objIPv4UnicastRoutesValue = usmDbSdmTemplateMaxIpv4RoutesGet(keyIDValue);
  FPOBJ_TRACE_VALUE (bufp, &objIPv4UnicastRoutesValue, sizeof(objIPv4UnicastRoutesValue));

  /* return the object value: IPv4UnicastRoutes */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objIPv4UnicastRoutesValue,
                          sizeof(objIPv4UnicastRoutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_IPv6NDPEntries
*
* @purpose Get 'IPv6NDPEntries'
 *@description  [IPv6NDPEntries] The maximum number of IPv6 Neighbor Discovery
* Protocol (NDP) cache entries.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_IPv6NDPEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objIPv6NDPEntriesValue;
  xLibU32_t keyIDValue;

  FPOBJ_TRACE_ENTER(bufp);

 /* retrieve key: ID */
 owa.len = sizeof(keyIDValue);
 owa.rc = xLibFilterGet(wap, XOBJ_sdmTemplateConfiguration_ID,
                             (xLibU8_t *) &keyIDValue, 
                             &owa.len);
 if(owa.rc != XLIBRC_SUCCESS)
 {
   owa.rc = XLIBRC_FILTER_MISSING; 
   FPOBJ_TRACE_EXIT(bufp, owa);
   return owa.rc;
 }

 FPOBJ_TRACE_CURRENT_KEY(bufp, &keyIDValue, owa.len);

    /* get the value from application */
  objIPv6NDPEntriesValue = usmDbSdmTemplateMaxNdpEntriesGet(keyIDValue);

  FPOBJ_TRACE_VALUE (bufp, &objIPv6NDPEntriesValue, sizeof(objIPv6NDPEntriesValue));

  /* return the object value: IPv6NDPEntries */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objIPv6NDPEntriesValue,
                          sizeof(objIPv6NDPEntriesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_IPv6UnicastRoutes
*
* @purpose Get 'IPv6UnicastRoutes'
 *@description  [IPv6UnicastRoutes] The maximum number of IPv6 unicast
* forwarding table entries.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_IPv6UnicastRoutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objIPv6UnicastRoutesValue;
  xLibU32_t keyIDValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: ID */
  owa.len = sizeof(keyIDValue);
  owa.rc = xLibFilterGet(wap, XOBJ_sdmTemplateConfiguration_ID,
                              (xLibU8_t *) &keyIDValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyIDValue, owa.len);

  /* get the value from application */
  objIPv6UnicastRoutesValue = usmDbSdmTemplateMaxIpv6RoutesGet(keyIDValue);

  FPOBJ_TRACE_VALUE (bufp, &objIPv6UnicastRoutesValue, sizeof(objIPv6UnicastRoutesValue));

  /* return the object value: IPv6UnicastRoutes */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objIPv6UnicastRoutesValue,
                          sizeof(objIPv6UnicastRoutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_ECMPNextHops
*
* @purpose Get 'ECMPNextHops'
 *@description  [ECMPNextHops] The maximum number of equal-cost next hops that can be
* installed in the IPv4 and IPv6 unicast forwarding tables.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_ECMPNextHops (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objECMPNextHopsValue;
  xLibU32_t keyIDValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: ID */
  owa.len = sizeof(keyIDValue);
  owa.rc = xLibFilterGet(wap, XOBJ_sdmTemplateConfiguration_ID,
                              (xLibU8_t *) &keyIDValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyIDValue, owa.len);

  /* get the value from application */
  objECMPNextHopsValue =  usmDbSdmTemplateMaxEcmpNextHopsGet(keyIDValue);
   
  FPOBJ_TRACE_VALUE (bufp, &objECMPNextHopsValue, sizeof(objECMPNextHopsValue));

  /* return the object value: ECMPNextHops */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objECMPNextHopsValue,
                          sizeof(objECMPNextHopsValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_IPv4MulticastRoutes
*
* @purpose Get 'IPv4MulticastRoutes'
 *@description  [IPv4MulticastRoutes] The maximum number of IPv4 multicast
* forwarding table entries.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_IPv4MulticastRoutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objIPv4MulticastRoutesValue;
  xLibU32_t keyIDValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: ID */
  owa.len = sizeof(keyIDValue);
  owa.rc = xLibFilterGet(wap, XOBJ_sdmTemplateConfiguration_ID,
                              (xLibU8_t *) &keyIDValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyIDValue, owa.len);

  objIPv4MulticastRoutesValue = usmDbSdmTemplateMaxIpv4McastRoutesGet(keyIDValue);
   
  FPOBJ_TRACE_VALUE (bufp, &objIPv4MulticastRoutesValue, sizeof(objIPv4MulticastRoutesValue));

  /* return the object value: IPv4MulticastRoutes */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objIPv4MulticastRoutesValue,
                          sizeof(objIPv4MulticastRoutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_sdmTemplateConfiguration_IPv6MulticastRoutes
*
* @purpose Get 'IPv6MulticastRoutes'
 *@description  [IPv6MulticastRoutes] The maximum number of IPv6 multicast
* forwarding table entries.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_sdmTemplateConfiguration_IPv6MulticastRoutes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objIPv6MulticastRoutesValue;
  xLibU32_t keyIDValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: ID */
  owa.len = sizeof(keyIDValue);
  owa.rc = xLibFilterGet(wap, XOBJ_sdmTemplateConfiguration_ID,
                              (xLibU8_t *) &keyIDValue, 
                              &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyIDValue, owa.len);

  /* get the value from application */
  objIPv6MulticastRoutesValue = usmDbSdmTemplateMaxIpv6McastRoutesGet(keyIDValue);

  FPOBJ_TRACE_VALUE (bufp, &objIPv6MulticastRoutesValue, sizeof(objIPv6MulticastRoutesValue));

  /* return the object value: IPv6MulticastRoutes */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objIPv6MulticastRoutesValue,
                          sizeof(objIPv6MulticastRoutesValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}


