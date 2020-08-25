/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfAreaAggregate.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfAreaAggregate-object.xml
*
* @create  2 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfAreaAggregate_obj.h"
#include "usmdb_mib_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfAreaAggregate_AreaID
*
* @purpose Get 'AreaID'
 *@description  [AreaID] The Area the Address Aggregate is to be found within.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfAreaAggregate_AreaID (void *wap, void *bufp)
{


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIdValue;
  xLibU32_t nextObjIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_AreaID,
                          (xLibU8_t *) & objIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    /*objIdValue = 0;
    owa.l7rc = usmDbOspfAreaEntryNext(L7_UNIT_CURRENT, objIdValue, &nextObjIdValue); */
    owa.l7rc = usmDbOspfAreaCfgGetFirst(L7_UNIT_CURRENT, &nextObjIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIdValue, owa.len);
    /*owa.l7rc = usmDbOspfAreaEntryNext (L7_UNIT_CURRENT, objIdValue,
                                       &nextObjIdValue);*/
      owa.l7rc = usmDbOspfAreaCfgGetNext(L7_UNIT_CURRENT, objIdValue,
                                       &nextObjIdValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIdValue, owa.len);

  /* return the object value: Id */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIdValue,
                           sizeof (objIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;  

}

/*******************************************************************************
* @function fpObjGet_routingospfAreaAggregate_LsdbType
*
* @purpose Get 'LsdbType'
 *@description  [LsdbType] The type of the Address Aggregate. This field
* specifies the Lsdb type that this Address Ag- gregate applies to.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfAreaAggregate_LsdbType (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIDValue;
  xLibIpV4_t nextObjAreaIDValue;
  xLibU32_t  objLsdbTypeValue;
  xLibU32_t  nextObjLsdbTypeValue;
  xLibIpV4_t objNetValue;
  xLibIpV4_t nextObjNetValue;
  xLibIpV4_t objMaskValue;
  xLibIpV4_t nextObjMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset (&objAreaIDValue, 0, sizeof (objAreaIDValue));
  memset (&objLsdbTypeValue, 0, sizeof (objLsdbTypeValue));
  memset (&objNetValue, 0, sizeof (objNetValue));
  memset (&objMaskValue, 0, sizeof (objMaskValue));
  memset (&nextObjAreaIDValue, 0, sizeof (nextObjAreaIDValue));
  memset (&nextObjLsdbTypeValue, 0, sizeof (nextObjLsdbTypeValue));
  memset (&nextObjNetValue, 0, sizeof (nextObjNetValue));
  memset (&nextObjMaskValue, 0, sizeof (nextObjMaskValue));

  /* retrieve key: AreaID */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_AreaID,
                          (xLibU8_t *) & objAreaIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIDValue, owa.len);

  /* retrieve key: LsdbType */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_LsdbType,
                          (xLibU8_t *) & objLsdbTypeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjAreaIDValue = objAreaIDValue;
    owa.l7rc =  usmDbOspfAnyAreaAggregateEntryNext(L7_UNIT_CURRENT, &nextObjAreaIDValue, &nextObjLsdbTypeValue,
                          &nextObjNetValue, &nextObjMaskValue); 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLsdbTypeValue, owa.len);
    nextObjAreaIDValue = objAreaIDValue;
    nextObjLsdbTypeValue = objLsdbTypeValue;
    do
    {
       owa.l7rc =  usmDbOspfAnyAreaAggregateEntryNext(L7_UNIT_CURRENT, &nextObjAreaIDValue, &nextObjLsdbTypeValue,
                          &nextObjNetValue, &nextObjMaskValue); 
    }
    while ((objAreaIDValue == nextObjAreaIDValue) && (objLsdbTypeValue == nextObjLsdbTypeValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objAreaIDValue != nextObjAreaIDValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLsdbTypeValue, owa.len);

  /* return the object value: LsdbType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLsdbTypeValue, sizeof (objLsdbTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfAreaAggregate_Net
*
* @purpose Get 'Net'
 *@description  [Net] The IP Address of the Net or Subnet indicated by the range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfAreaAggregate_Net (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIDValue;
  xLibIpV4_t nextObjAreaIDValue;
  xLibU32_t  objLsdbTypeValue;
  xLibU32_t  nextObjLsdbTypeValue;
  xLibIpV4_t objNetValue;
  xLibIpV4_t nextObjNetValue;
  xLibIpV4_t objMaskValue;
  xLibIpV4_t nextObjMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset (&objAreaIDValue, 0, sizeof (objAreaIDValue));
  memset (&objLsdbTypeValue, 0, sizeof (objLsdbTypeValue));
  memset (&objNetValue, 0, sizeof (objNetValue));
  memset (&objMaskValue, 0, sizeof (objMaskValue));
  memset (&nextObjAreaIDValue, 0, sizeof (nextObjAreaIDValue));
  memset (&nextObjLsdbTypeValue, 0, sizeof (nextObjLsdbTypeValue));
  memset (&nextObjNetValue, 0, sizeof (nextObjNetValue));
  memset (&nextObjMaskValue, 0, sizeof (nextObjMaskValue));

  /* retrieve key: AreaID */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_AreaID,
                          (xLibU8_t *) & objAreaIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIDValue, owa.len);

  /* retrieve key: LsdbType */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_LsdbType,
                          (xLibU8_t *) & objLsdbTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLsdbTypeValue, owa.len);

  /* retrieve key: Net */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Net,
                          (xLibU8_t *) & objNetValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjAreaIDValue = objAreaIDValue;
    nextObjLsdbTypeValue = objLsdbTypeValue;
    owa.l7rc =  usmDbOspfAnyAreaAggregateEntryNext(L7_UNIT_CURRENT, &nextObjAreaIDValue, &nextObjLsdbTypeValue,
                          &nextObjNetValue, &nextObjMaskValue); 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objNetValue, owa.len);
    nextObjAreaIDValue = objAreaIDValue;
    nextObjLsdbTypeValue = objLsdbTypeValue;
    nextObjNetValue = objNetValue;
  
    do
    {
      owa.l7rc =  usmDbOspfAnyAreaAggregateEntryNext(L7_UNIT_CURRENT, &nextObjAreaIDValue, &nextObjLsdbTypeValue,
                          &nextObjNetValue, &nextObjMaskValue); 
    }
    while ((objAreaIDValue == nextObjAreaIDValue) && (objLsdbTypeValue == nextObjLsdbTypeValue)
           && (objNetValue == nextObjNetValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objAreaIDValue != nextObjAreaIDValue) || (objLsdbTypeValue != nextObjLsdbTypeValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjNetValue, owa.len);

  /* return the object value: Net */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjNetValue, sizeof (objNetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfAreaAggregate_Mask
*
* @purpose Get 'Mask'
 *@description  [Mask] The Subnet Mask that pertains to the Net or Subnet.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfAreaAggregate_Mask (void *wap, void *bufp)
{
  xLibIpV4_t objAreaIDValue;
  xLibIpV4_t nextObjAreaIDValue;
  xLibU32_t  objLsdbTypeValue;
  xLibU32_t  nextObjLsdbTypeValue;
  xLibIpV4_t objNetValue;
  xLibIpV4_t nextObjNetValue;
  xLibIpV4_t objMaskValue;
  xLibIpV4_t nextObjMaskValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset (&objAreaIDValue, 0, sizeof (objAreaIDValue));
  memset (&objLsdbTypeValue, 0, sizeof (objLsdbTypeValue));
  memset (&objNetValue, 0, sizeof (objNetValue));
  memset (&objMaskValue, 0, sizeof (objMaskValue));
  memset (&nextObjAreaIDValue, 0, sizeof (nextObjAreaIDValue));
  memset (&nextObjLsdbTypeValue, 0, sizeof (nextObjLsdbTypeValue));
  memset (&nextObjNetValue, 0, sizeof (nextObjNetValue));
  memset (&nextObjMaskValue, 0, sizeof (nextObjMaskValue));

  /* retrieve key: AreaID */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_AreaID,
                          (xLibU8_t *) & objAreaIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIDValue, owa.len);

  /* retrieve key: LsdbType */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_LsdbType,
                          (xLibU8_t *) & objLsdbTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLsdbTypeValue, owa.len);

  /* retrieve key: Net */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Net,
                          (xLibU8_t *) & objNetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objNetValue, owa.len);

  nextObjAreaIDValue = objAreaIDValue;
  nextObjLsdbTypeValue = objLsdbTypeValue;
  nextObjNetValue = objNetValue;
  
  /* retrieve key: Mask */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Mask,
                          (xLibU8_t *) & objMaskValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  }
  else
  {
    nextObjMaskValue = objMaskValue;
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMaskValue, owa.len);
  }

  owa.l7rc =  usmDbOspfAnyAreaAggregateEntryNext(L7_UNIT_CURRENT, &nextObjAreaIDValue, &nextObjLsdbTypeValue,
              &nextObjNetValue, &nextObjMaskValue); 

  if ((objAreaIDValue != nextObjAreaIDValue) || (objLsdbTypeValue != nextObjLsdbTypeValue)
      || (objNetValue != nextObjNetValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjMaskValue, owa.len);

  /* return the object value: Mask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjMaskValue, sizeof (objMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfAreaAggregate_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This variable displays the status of the en try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfAreaAggregate_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIDValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsdbTypeValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNetValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMaskValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaID */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_AreaID,
                           (xLibU8_t *) & keyAreaIDValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIDValue, kwa1.len);

  /* retrieve key: LsdbType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_LsdbType,
                           (xLibU8_t *) & keyLsdbTypeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsdbTypeValue, kwa2.len);

  /* retrieve key: Net */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Net,
                           (xLibU8_t *) & keyNetValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNetValue, kwa3.len);

  /* retrieve key: Mask */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Mask,
                           (xLibU8_t *) & keyMaskValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMaskValue, kwa4.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfAreaAggregateStatusGet (L7_UNIT_CURRENT, keyAreaIDValue,
                                              keyLsdbTypeValue,
                                              keyNetValue,
                                              keyMaskValue, &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfAreaAggregate_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This variable displays the status of the en try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfAreaAggregate_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIDValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsdbTypeValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNetValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: AreaID */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_AreaID,
                           (xLibU8_t *) & keyAreaIDValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIDValue, kwa1.len);

  /* retrieve key: LsdbType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_LsdbType,
                           (xLibU8_t *) & keyLsdbTypeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsdbTypeValue, kwa2.len);

  /* retrieve key: Net */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Net,
                           (xLibU8_t *) & keyNetValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNetValue, kwa3.len);

  /* retrieve key: Mask */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Mask,
                           (xLibU8_t *) & keyMaskValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    kwa4.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMaskValue, kwa4.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc = usmDbOspfAreaAggregateStatusSet (L7_UNIT_CURRENT, keyAreaIDValue,
                                                keyLsdbTypeValue,
                                                keyNetValue,
                                                keyMaskValue, L7_CREATE);
    if (owa.l7rc == L7_ERROR)
    {
      owa.rc = XLIBRC_OSPF_AREA_RANGE_CONFLICT;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbOspfAreaAggregateStatusSet (L7_UNIT_CURRENT, keyAreaIDValue,
                                                keyLsdbTypeValue,
                                                keyNetValue,
                                                keyMaskValue, L7_DELETE);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_routingospfAreaAggregate_Effect
*
* @purpose Get 'Effect'
*
* @description [Effect]: Subnets subsumed by ranges either trigger the advertisement
*              of the indicated aggregate (advertiseMatching),
*              or result in the subnet's not being advertised at all outside
*              the area. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfAreaAggregate_Effect (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIDValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsdbTypeValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNetValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMaskValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEffectValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaID */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_AreaID,
                           (xLibU8_t *) & keyAreaIDValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIDValue, kwa1.len);

  /* retrieve key: LsdbType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_LsdbType,
                           (xLibU8_t *) & keyLsdbTypeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsdbTypeValue, kwa2.len);

  /* retrieve key: Net */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Net,
                           (xLibU8_t *) & keyNetValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNetValue, kwa3.len);

  /* retrieve key: Mask */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Mask,
                           (xLibU8_t *) & keyMaskValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMaskValue, kwa4.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfAreaAggregateEffectGet (L7_UNIT_CURRENT, keyAreaIDValue,
                                              keyLsdbTypeValue,
                                              keyNetValue,
                                              keyMaskValue, &objEffectValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Effect */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEffectValue,
                           sizeof (objEffectValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfAreaAggregate_Effect
*
* @purpose Set 'Effect'
*
* @description [Effect]: Subnets subsumed by ranges either trigger the advertisement
*              of the indicated aggregate (advertiseMatching),
*              or result in the subnet's not being advertised at all outside
*              the area. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfAreaAggregate_Effect (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEffectValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIDValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsdbTypeValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNetValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Effect */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objEffectValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objEffectValue, owa.len);

  /* retrieve key: AreaID */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_AreaID,
                           (xLibU8_t *) & keyAreaIDValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIDValue, kwa1.len);

  /* retrieve key: LsdbType */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_LsdbType,
                           (xLibU8_t *) & keyLsdbTypeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsdbTypeValue, kwa2.len);

  /* retrieve key: Net */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Net,
                           (xLibU8_t *) & keyNetValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNetValue, kwa3.len);

  /* retrieve key: Mask */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routingospfAreaAggregate_Mask,
                           (xLibU8_t *) & keyMaskValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    kwa4.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMaskValue, kwa4.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfAreaAggregateEffectSet (L7_UNIT_CURRENT, keyAreaIDValue,
                                              keyLsdbTypeValue,
                                              keyNetValue,
                                              keyMaskValue, objEffectValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
