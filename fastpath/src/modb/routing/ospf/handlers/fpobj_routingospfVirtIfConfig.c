/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfVirtIfConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfConfig-object.xml
*
* @create  24 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfVirtIfConfig_obj.h"
#include "usmdb_mib_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfVirtIfConfig_AreaId
*
* @purpose Get 'AreaId'
 *@description  [AreaId] The Transit Area that the Virtual Link traverses. By
* definition, this is not 0.0.0.0   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtIfConfig_AreaId (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIdValue;
  xLibIpV4_t nextObjAreaIdValue;
  xLibIpV4_t nextObjNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfVirtIfConfig_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&nextObjAreaIdValue, 0, sizeof (nextObjAreaIdValue));
    memset (&nextObjNeighborValue, 0, sizeof (nextObjNeighborValue));
    owa.l7rc =
       usmDbOspfVirtIfEntryNext(L7_UNIT_CURRENT, &nextObjAreaIdValue, &nextObjNeighborValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);
    nextObjAreaIdValue = objAreaIdValue;
    nextObjNeighborValue = 0;

    do
    {
      owa.l7rc = usmDbOspfVirtIfEntryNext(L7_UNIT_CURRENT,
                                      &nextObjAreaIdValue, &nextObjNeighborValue);
    }
    while ((objAreaIdValue == nextObjAreaIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAreaIdValue, owa.len);

  /* return the object value: AreaId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAreaIdValue, sizeof (objAreaIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfVirtIfConfig_Neighbor
*
* @purpose Get 'Neighbor'
 *@description  [Neighbor] The Router ID of the Virtual Neighbor.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtIfConfig_Neighbor (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIdValue;
  xLibIpV4_t nextObjAreaIdValue;
  xLibIpV4_t objNeighborValue;
  xLibIpV4_t nextObjNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfVirtIfConfig_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);

  /* retrieve key: Neighbor */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfVirtIfConfig_Neighbor,
                          (xLibU8_t *) & objNeighborValue, &owa.len);
  nextObjAreaIdValue = objAreaIdValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&nextObjNeighborValue, 0, sizeof (nextObjNeighborValue));
    owa.l7rc = usmDbOspfVirtIfEntryNext(L7_UNIT_CURRENT, &nextObjAreaIdValue, &nextObjNeighborValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objNeighborValue, owa.len);
    nextObjNeighborValue = objNeighborValue;
    owa.l7rc = usmDbOspfVirtIfEntryNext(L7_UNIT_CURRENT, &nextObjAreaIdValue, &nextObjNeighborValue);

  }


  if ((objAreaIdValue != nextObjAreaIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjNeighborValue, owa.len);

  /* return the object value: Neighbor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjNeighborValue, sizeof (objNeighborValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_routingospfVirtIfConfig_AreaId_Neighbor
*
* @purpose Get 'AreaId + Neighbor +'
*
* @description [AreaId]: The Transit Area that the Virtual Link traverses.
*              By definition, this is not 0.0.0.0 
*              [Neighbor]: The Router ID of the Virtual Neighbor. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtIfConfig_AreaId_Neighbor (void *wap,
                                                           void *bufp[],
                                                           xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaAreaId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaIdValue, nextObjAreaIdValue;
  fpObjWa_t owaNeighbor = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNeighborValue, nextObjNeighborValue;
  void *outAreaId = (void *) bufp[--keyCount];
  void *outNeighbor = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outAreaId);
  FPOBJ_TRACE_ENTER (outNeighbor);

  /* retrieve key: AreaId */
  owaAreaId.rc = xLibFilterGet (wap, XOBJ_routingospfVirtIfConfig_AreaId,
                                (xLibU8_t *) & objAreaIdValue, &owaAreaId.len);
  if (owaAreaId.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: Neighbor */
    owaNeighbor.rc = xLibFilterGet (wap, XOBJ_routingospfVirtIfConfig_Neighbor,
                                    (xLibU8_t *) & objNeighborValue,
                                    &owaNeighbor.len);
  }
  else
  {
    objAreaIdValue = 0;
    nextObjAreaIdValue = 0;
    objNeighborValue = 0;
    nextObjNeighborValue = 0; 
  } 
  
  FPOBJ_TRACE_CURRENT_KEY (outAreaId, &objAreaIdValue, owaAreaId.len);
  FPOBJ_TRACE_CURRENT_KEY (outNeighbor, &objNeighborValue, owaNeighbor.len);

  owa.rc =
    usmDbOspfVirtIfEntryNext(L7_UNIT_CURRENT, &objAreaIdValue, &objNeighborValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outAreaId, owaAreaId);
    FPOBJ_TRACE_EXIT (outNeighbor, owaNeighbor);
    return owa.rc;
  }

  nextObjNeighborValue = objNeighborValue;
  nextObjAreaIdValue = objAreaIdValue;
 
  FPOBJ_TRACE_CURRENT_KEY (outAreaId, &nextObjAreaIdValue, owaAreaId.len);
  FPOBJ_TRACE_CURRENT_KEY (outNeighbor, &nextObjNeighborValue, owaNeighbor.len);

  /* return the object value: AreaId */
  xLibBufDataSet (outAreaId,
                  (xLibU8_t *) & nextObjAreaIdValue,
                  sizeof (nextObjAreaIdValue));

  /* return the object value: Neighbor */
  xLibBufDataSet (outNeighbor,
                  (xLibU8_t *) & nextObjNeighborValue,
                  sizeof (nextObjNeighborValue));
  FPOBJ_TRACE_EXIT (outAreaId, owaAreaId);
  FPOBJ_TRACE_EXIT (outNeighbor, owaNeighbor);
  return XLIBRC_SUCCESS;
}

#endif
/*******************************************************************************
* @function fpObjGet_routingospfVirtIfConfig_AuthKeyId
*
* @purpose Get 'AuthKeyId'
*
* @description [AuthKeyId]: The identifier for the authentication key used
*              on this virtual interface. This field is only meaningful when
*              the OSPF-MIB ospfVirtIfAuthType is md5(2); otherwise, the
*              value is not used. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtIfConfig_AuthKeyId (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthKeyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtIfConfig_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtIfConfig_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfAuthKeyIdGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                          keyNeighborValue, &objAuthKeyIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthKeyId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthKeyIdValue,
                           sizeof (objAuthKeyIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfVirtIfConfig_AuthKeyId
*
* @purpose Set 'AuthKeyId'
*
* @description [AuthKeyId]: The identifier for the authentication key used
*              on this virtual interface. This field is only meaningful when
*              the OSPF-MIB ospfVirtIfAuthType is md5(2); otherwise, the
*              value is not used. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfVirtIfConfig_AuthKeyId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthKeyIdValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthKeyId */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthKeyIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthKeyIdValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtIfConfig_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtIfConfig_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfVirtIfAuthKeyIdSet (L7_UNIT_CURRENT, keyAreaIdValue,
                                          keyNeighborValue, objAuthKeyIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
