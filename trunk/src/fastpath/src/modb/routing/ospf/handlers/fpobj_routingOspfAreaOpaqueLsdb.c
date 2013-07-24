/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingOspfAreaOpaqueLsdb.c
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
#include "_xe_routingOspfAreaOpaqueLsdb_obj.h"
#include "usmdb_mib_ospf_api.h"


/*******************************************************************************
* @function fpObjGet_routingOspfAreaOpaqueLsdb_AreaId
*
* @purpose Get 'AreaId'
 *@description  [AreaId] The 32 bit identifier of the Area from which the LSA
* was received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfAreaOpaqueLsdb_AreaId (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIdValue;
  xLibIpV4_t nextObjAreaIdValue;
  xLibU32_t objTypeValue;
  xLibIpV4_t objLsidValue;
  xLibIpV4_t objRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objAreaIdValue, 0, sizeof (objAreaIdValue));
    memset (&objTypeValue, 0, sizeof (objTypeValue));
    memset (&objLsidValue, 0, sizeof (objLsidValue));
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    owa.l7rc = usmDbOspfAreaOpaqueLsdbEntryNext (&objAreaIdValue, &objTypeValue,
                           &objLsidValue, &objRouterIdValue, &temp_Lsa);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);
    nextObjAreaIdValue = objAreaIdValue;
    memset (&objTypeValue, 0, sizeof (objTypeValue));
    memset (&objLsidValue, 0, sizeof (objLsidValue));
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    do
    {
      owa.l7rc = usmDbOspfAreaOpaqueLsdbEntryNext (&objAreaIdValue, &objTypeValue,
                             &objLsidValue, &objRouterIdValue, &temp_Lsa);
    }
    while ((objAreaIdValue == nextObjAreaIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjAreaIdValue = objAreaIdValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAreaIdValue, owa.len);

  /* return the object value: AreaId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAreaIdValue, sizeof (objAreaIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingOspfAreaOpaqueLsdb_Type
*
* @purpose Get 'Type'
 *@description  [Type] The type of the link state advertisement. Each link state
* type has a separate advertisement format.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfAreaOpaqueLsdb_Type (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIdValue;
  xLibU32_t nextObjAreaIdValue;
  xLibU32_t objTypeValue;
  xLibU32_t nextObjTypeValue;
  xLibIpV4_t objLsidValue;
  xLibIpV4_t objRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);

  /* retrieve key: Type */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Type,
                          (xLibU8_t *) & objTypeValue, &owa.len);
  nextObjAreaIdValue = objAreaIdValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objTypeValue, 0, sizeof (objTypeValue));
    memset (&objLsidValue, 0, sizeof (objLsidValue));
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    owa.l7rc = usmDbOspfAreaOpaqueLsdbEntryNext (&objAreaIdValue, &objTypeValue,
                           &objLsidValue, &objRouterIdValue, &temp_Lsa);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTypeValue, owa.len);
    nextObjTypeValue = objTypeValue;
    memset (&objLsidValue, 0, sizeof (objLsidValue));
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    do
    {
      owa.l7rc = usmDbOspfAreaOpaqueLsdbEntryNext (&objAreaIdValue, &objTypeValue,
                           &objLsidValue, &objRouterIdValue, &temp_Lsa);
    }
    while ((objAreaIdValue == nextObjAreaIdValue) && (objTypeValue == nextObjTypeValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objAreaIdValue != nextObjAreaIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjTypeValue = objTypeValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjTypeValue, owa.len);

  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjTypeValue, sizeof (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingOspfAreaOpaqueLsdb_Lsid
*
* @purpose Get 'Lsid'
 *@description  [Lsid] The Link State ID is an LS Type Specific field containing
* either a Router ID or an IP Address; it identifies the piece of
* the routing domain that is being described by the advertisement.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfAreaOpaqueLsdb_Lsid (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIdValue;
  xLibIpV4_t nextObjAreaIdValue;
  xLibU32_t objTypeValue;
  xLibIpV4_t nextObjTypeValue;
  xLibIpV4_t objLsidValue;
  xLibIpV4_t nextObjLsidValue;
  xLibIpV4_t objRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);

  /* retrieve key: Type */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Type,
                          (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objTypeValue, owa.len);

  /* retrieve key: Lsid */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Lsid,
                          (xLibU8_t *) & objLsidValue, &owa.len);
  nextObjAreaIdValue = objAreaIdValue;
  nextObjTypeValue = objTypeValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objLsidValue, 0, sizeof (objLsidValue));
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    owa.l7rc = usmDbOspfAreaOpaqueLsdbEntryNext (&objAreaIdValue, &objTypeValue,
                           &objLsidValue, &objRouterIdValue, &temp_Lsa);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLsidValue, owa.len);
    nextObjLsidValue = objLsidValue;   
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    do
    {
      owa.l7rc = usmDbOspfAreaOpaqueLsdbEntryNext (&objAreaIdValue, &objTypeValue,
                           &objLsidValue, &objRouterIdValue, &temp_Lsa);
    }
    while ((objAreaIdValue == nextObjAreaIdValue) && (objTypeValue == nextObjTypeValue)
           && (objLsidValue == nextObjLsidValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objAreaIdValue != nextObjAreaIdValue) || (objTypeValue != nextObjTypeValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjLsidValue = objLsidValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLsidValue, owa.len);

  /* return the object value: Lsid */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLsidValue, sizeof (objLsidValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingOspfAreaOpaqueLsdb_RouterId
*
* @purpose Get 'RouterId'
 *@description  [RouterId] The 32 bit number that uniquely identifies the
* originating router in the Autonomous System.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfAreaOpaqueLsdb_RouterId (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIdValue;
  xLibIpV4_t nextObjAreaIdValue;
  xLibU32_t objTypeValue;
  xLibIpV4_t nextObjTypeValue;
  xLibIpV4_t objLsidValue;
  xLibIpV4_t nextObjLsidValue;
  xLibIpV4_t objRouterIdValue;
  xLibIpV4_t nextObjRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);

  /* retrieve key: Type */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Type,
                          (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objTypeValue, owa.len);

  /* retrieve key: Lsid */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Lsid,
                          (xLibU8_t *) & objLsidValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLsidValue, owa.len);

  /* retrieve key: RouterId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_RouterId,
                          (xLibU8_t *) & objRouterIdValue, &owa.len);
  nextObjAreaIdValue = objAreaIdValue;
  nextObjTypeValue = objTypeValue;
  nextObjLsidValue = objLsidValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRouterIdValue, owa.len);

  }
  owa.l7rc = usmDbOspfAreaOpaqueLsdbEntryNext (&objAreaIdValue, &objTypeValue,
                         &objLsidValue, &objRouterIdValue, &temp_Lsa);

  if ((objAreaIdValue != nextObjAreaIdValue) || (objTypeValue != nextObjTypeValue)
      || (objLsidValue != nextObjLsidValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjRouterIdValue = objRouterIdValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRouterIdValue, owa.len);

  /* return the object value: RouterId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRouterIdValue, sizeof (objRouterIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#if 0
/*******************************************************************************
* @function fpObjGet_routingOspfAreaOpaqueLsdb_AreaId_Type_Lsid_RouterId
*
* @purpose Get 'AreaId + Type + Lsid + RouterId +'
*
* @description [AreaId]: The 32 bit identifier of the Area from which the
*              LSA was received. 
*              [Type]: The type of the link state advertisement. Each link
*              state type has a separate advertisement format. 
*              [Lsid]: The Link State ID is an LS Type Specific field containing
*              either a Router ID or an IP Address; it identifies the
*              piece of the routing domain that is being described by the
*              advertisement. 
*              [RouterId]: The 32 bit number that uniquely identifies the
*              originating router in the Autonomous System. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfAreaOpaqueLsdb_AreaId_Type_Lsid_RouterId (void
                                                                       *wap,
                                                                       void
                                                                       *bufp[],
                                                                       xLibU16_t
                                                                       keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaAreaId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaIdValue, nextObjAreaIdValue;
  fpObjWa_t owaType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue, nextObjTypeValue;
  fpObjWa_t owaLsid = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLsidValue, nextObjLsidValue;
  fpObjWa_t owaRouterId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRouterIdValue, nextObjRouterIdValue;
  L7_ospfOpaqueLsdbEntry_t temp_Lsa;

  void *outAreaId = (void *) bufp[--keyCount];
  void *outType = (void *) bufp[--keyCount];
  void *outLsid = (void *) bufp[--keyCount];
  void *outRouterId = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outAreaId);
  FPOBJ_TRACE_ENTER (outType);
  FPOBJ_TRACE_ENTER (outLsid);
  FPOBJ_TRACE_ENTER (outRouterId);

  /* retrieve key: AreaId */
  owaAreaId.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_AreaId,
                                (xLibU8_t *) & objAreaIdValue, &owaAreaId.len);
  if (owaAreaId.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: Type */
    owaType.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Type,
                                (xLibU8_t *) & objTypeValue, &owaType.len);
    if (owaType.rc == XLIBRC_SUCCESS)
    {
      /* retrieve key: Lsid */
      owaLsid.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Lsid,
                                  (xLibU8_t *) & objLsidValue, &owaLsid.len);
      if (owaLsid.rc == XLIBRC_SUCCESS)
      {
        /* retrieve key: RouterId */
        owaRouterId.rc =
          xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_RouterId,
                         (xLibU8_t *) & objRouterIdValue, &owaRouterId.len);
      }
    }
  }
  else
  {
     objAreaIdValue = 0; 
     nextObjAreaIdValue = 0;
     objTypeValue = 0; 
     nextObjTypeValue = 0;
     objLsidValue = 0; 
     nextObjLsidValue = 0;
     objRouterIdValue = 0; 
     nextObjRouterIdValue = 0;
  }  
  FPOBJ_TRACE_CURRENT_KEY (outAreaId, &objAreaIdValue, owaAreaId.len);
  FPOBJ_TRACE_CURRENT_KEY (outType, &objTypeValue, owaType.len);
  FPOBJ_TRACE_CURRENT_KEY (outLsid, &objLsidValue, owaLsid.len);
  FPOBJ_TRACE_CURRENT_KEY (outRouterId, &objRouterIdValue, owaRouterId.len);

  owa.rc = usmDbOspfAreaOpaqueLsdbEntryNext (&objAreaIdValue, &objTypeValue,
                         &objLsidValue, &objRouterIdValue, &temp_Lsa);

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outAreaId, owaAreaId);
    FPOBJ_TRACE_EXIT (outType, owaType);
    FPOBJ_TRACE_EXIT (outLsid, owaLsid);
    FPOBJ_TRACE_EXIT (outRouterId, owaRouterId);
    return owa.rc;
  }

  nextObjAreaIdValue = objAreaIdValue; 
  nextObjTypeValue = objTypeValue; 
  nextObjLsidValue = objLsidValue; 
  nextObjRouterIdValue = objRouterIdValue; 

  FPOBJ_TRACE_CURRENT_KEY (outAreaId, &nextObjAreaIdValue, owaAreaId.len);
  FPOBJ_TRACE_CURRENT_KEY (outType, &nextObjTypeValue, owaType.len);
  FPOBJ_TRACE_CURRENT_KEY (outLsid, &nextObjLsidValue, owaLsid.len);
  FPOBJ_TRACE_CURRENT_KEY (outRouterId, &nextObjRouterIdValue, owaRouterId.len);

  /* return the object value: AreaId */
  xLibBufDataSet (outAreaId,
                  (xLibU8_t *) & nextObjAreaIdValue,
                  sizeof (nextObjAreaIdValue));

  /* return the object value: Type */
  xLibBufDataSet (outType,
                  (xLibU8_t *) & nextObjTypeValue, sizeof (nextObjTypeValue));

  /* return the object value: Lsid */
  xLibBufDataSet (outLsid,
                  (xLibU8_t *) & nextObjLsidValue, sizeof (nextObjLsidValue));

  /* return the object value: RouterId */
  xLibBufDataSet (outRouterId,
                  (xLibU8_t *) & nextObjRouterIdValue,
                  sizeof (nextObjRouterIdValue));
  FPOBJ_TRACE_EXIT (outAreaId, owaAreaId);
  FPOBJ_TRACE_EXIT (outType, owaType);
  FPOBJ_TRACE_EXIT (outLsid, owaLsid);
  FPOBJ_TRACE_EXIT (outRouterId, owaRouterId);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_routingOspfAreaOpaqueLsdb_Sequence
*
* @purpose Get 'Sequence'
*
* @description [Sequence]: The sequence number field is a signed 32-bit integer.
*              It starts with the value '80000001'h, or -'7FFFFFFF'h,
*              and increments until '7FFFFFFF'h. Thus, a typical sequence
*              number will be very negative. It is used to detect old and
*              duplicate link state advertisements. The space of sequence
*              numbers is linearly ordered. The larger the sequence number
*              the more recent the advertisement. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfAreaOpaqueLsdb_Sequence (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTypeValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsidValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSequenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Type */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Type,
                           (xLibU8_t *) & keyTypeValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTypeValue, kwa2.len);

  /* retrieve key: Lsid */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Lsid,
                           (xLibU8_t *) & keyLsidValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsidValue, kwa3.len);

  /* retrieve key: RouterId */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_RouterId,
                           (xLibU8_t *) & keyRouterIdValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRouterIdValue, kwa4.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfAreaOpaqueLsdbSequenceGet (keyAreaIdValue,
                                        keyTypeValue, keyLsidValue,
                                        keyRouterIdValue, &objSequenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Sequence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSequenceValue,
                           sizeof (objSequenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingOspfAreaOpaqueLsdb_Age
*
* @purpose Get 'Age'
*
* @description [Age]: This field is the age of the link state advertisement
*              in seconds. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfAreaOpaqueLsdb_Age (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTypeValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsidValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAgeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Type */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Type,
                           (xLibU8_t *) & keyTypeValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTypeValue, kwa2.len);

  /* retrieve key: Lsid */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Lsid,
                           (xLibU8_t *) & keyLsidValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsidValue, kwa3.len);

  /* retrieve key: RouterId */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_RouterId,
                           (xLibU8_t *) & keyRouterIdValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRouterIdValue, kwa4.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfAreaOpaqueLsdbAgeGet (keyAreaIdValue,
                                            keyTypeValue,
                                            keyLsidValue,
                                            keyRouterIdValue, &objAgeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Age */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAgeValue,
                           sizeof (objAgeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingOspfAreaOpaqueLsdb_Checksum
*
* @purpose Get 'Checksum'
*
* @description [Checksum]: This field is the checksum of the complete contents
*              of the advertisement, excepting the age field. The age
*              field is excepted so that an advertisement's age can be incremented
*              without updating the checksum. The checksum used
*              is the same that is used for ISO connectionless datagrams;
*              it is commonly referred to as the Fletcher checksum. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfAreaOpaqueLsdb_Checksum (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTypeValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsidValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objChecksumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Type */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Type,
                           (xLibU8_t *) & keyTypeValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTypeValue, kwa2.len);

  /* retrieve key: Lsid */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Lsid,
                           (xLibU8_t *) & keyLsidValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsidValue, kwa3.len);

  /* retrieve key: RouterId */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_RouterId,
                           (xLibU8_t *) & keyRouterIdValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRouterIdValue, kwa4.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfAreaOpaqueLsdbChecksumGet (keyAreaIdValue,
                                        keyTypeValue, keyLsidValue,
                                        keyRouterIdValue, &objChecksumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Checksum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objChecksumValue,
                           sizeof (objChecksumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingOspfAreaOpaqueLsdb_Advertisement
*
* @purpose Get 'Advertisement'
*
* @description [Advertisement]: The entire Link State Advertisement, including
*              its header. Note that for variable length LSAs, SNMP agents
*              may not be able to return the largest string size 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingOspfAreaOpaqueLsdb_Advertisement (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTypeValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsidValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRouterIdValue;
  xLibU32_t bufflen;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAdvertisementValue;
  xLibS8_t *Str;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Type */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Type,
                           (xLibU8_t *) & keyTypeValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTypeValue, kwa2.len);

  /* retrieve key: Lsid */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_Lsid,
                           (xLibU8_t *) & keyLsidValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsidValue, kwa3.len);

  /* retrieve key: RouterId */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routingOspfAreaOpaqueLsdb_RouterId,
                           (xLibU8_t *) & keyRouterIdValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRouterIdValue, kwa4.len);

  /* get the value from application */
  Str = objAdvertisementValue;

  owa.l7rc =
    usmDbOspfAreaOpaqueLsdbAdvertisementGet (keyAreaIdValue,
                                             keyTypeValue, keyLsidValue,
                                             keyRouterIdValue,
                                             &Str, &bufflen);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Advertisement */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAdvertisementValue,
                           strlen (objAdvertisementValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
