/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfExtLsdb.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfExtLsdb-object.xml
*
* @create  25 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfExtLsdb_obj.h"
#include "usmdb_mib_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfExtLsdb_Type
*
* @purpose Get 'Type'
 *@description  [Type] The type of the link state advertisement. Each link state
* type has a separate advertisement format.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfExtLsdb_Type (void *wap, void *bufp)
{

  xLibU32_t objTypeValue;
  xLibU32_t nextObjTypeValue;
  xLibIpV4_t objLsidValue;
  xLibIpV4_t objRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_ospfLsdbEntry_t temp_Lsa;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Type */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Type, (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objTypeValue, 0, sizeof (objTypeValue));
    memset (&objLsidValue, 0, sizeof (objLsidValue));
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    owa.l7rc =
      usmDbOspfExtLsdbEntryNext(L7_UNIT_CURRENT, &objTypeValue, &objLsidValue, &objRouterIdValue, &temp_Lsa);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTypeValue, owa.len);
    nextObjTypeValue = objTypeValue;
    memset (&objLsidValue, 0, sizeof (objLsidValue));
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    do
    {
      owa.l7rc =
        usmDbOspfExtLsdbEntryNext(L7_UNIT_CURRENT, &objTypeValue, &objLsidValue, &objRouterIdValue, &temp_Lsa);
    }
    while ((objTypeValue == nextObjTypeValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
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
* @function fpObjGet_routingospfExtLsdb_Lsid
*
* @purpose Get 'Lsid'
 *@description  [Lsid] The Link State ID is an LS Type Specific field containing
* either a Router ID or an IP Address; it identifies the piece of
* the routing domain that is being described by the advertisement.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfExtLsdb_Lsid (void *wap, void *bufp)
{

  xLibU32_t objTypeValue;
  xLibIpV4_t nextObjTypeValue;
  xLibIpV4_t objLsidValue;
  xLibIpV4_t nextObjLsidValue;
  xLibIpV4_t objRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  L7_ospfLsdbEntry_t temp_Lsa;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Type */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Type, (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objTypeValue, owa.len);

  /* retrieve key: Lsid */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Lsid, (xLibU8_t *) & objLsidValue, &owa.len);
  nextObjTypeValue = objTypeValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objLsidValue, 0, sizeof (objLsidValue));
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    owa.l7rc =
      usmDbOspfExtLsdbEntryNext(L7_UNIT_CURRENT, &objTypeValue, &objLsidValue, &objRouterIdValue, &temp_Lsa);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLsidValue, owa.len);
    nextObjLsidValue = objLsidValue;
    memset (&objRouterIdValue, 0, sizeof (objRouterIdValue));
    do
    {
      owa.l7rc =
        usmDbOspfExtLsdbEntryNext(L7_UNIT_CURRENT, &objTypeValue, &objLsidValue, &objRouterIdValue, &temp_Lsa);
    }
    while ((objTypeValue == nextObjTypeValue) && (objLsidValue == nextObjLsidValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((objTypeValue != nextObjTypeValue) || (owa.l7rc != L7_SUCCESS))
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
* @function fpObjGet_routingospfExtLsdb_RouterId
*
* @purpose Get 'RouterId'
 *@description  [RouterId] The 32 bit number that uniquely identifies the
* originating router in the Autonomous System.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfExtLsdb_RouterId (void *wap, void *bufp)
{

  xLibU32_t objTypeValue;
  xLibIpV4_t nextObjTypeValue;
  xLibIpV4_t objLsidValue;
  xLibIpV4_t nextObjLsidValue;
  xLibIpV4_t objRouterIdValue;
  xLibIpV4_t nextObjRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  L7_ospfLsdbEntry_t temp_Lsa;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Type */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Type, (xLibU8_t *) & objTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objTypeValue, owa.len);

  /* retrieve key: Lsid */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Lsid, (xLibU8_t *) & objLsidValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLsidValue, owa.len);

  /* retrieve key: RouterId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_RouterId,
                          (xLibU8_t *) & objRouterIdValue, &owa.len);

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

  owa.l7rc =
    usmDbOspfExtLsdbEntryNext(L7_UNIT_CURRENT, &objTypeValue, &objLsidValue, &objRouterIdValue, &temp_Lsa);

  if ((objTypeValue != nextObjTypeValue) || (objLsidValue != nextObjLsidValue)
      || (owa.l7rc != L7_SUCCESS))
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
* @function fpObjGet_routingospfExtLsdb_Type_Lsid_RouterId
*
* @purpose Get 'Type + Lsid + RouterId +'
*
* @description [Type]: The type of the link state advertisement. Each link
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
xLibRC_t fpObjGet_routingospfExtLsdb_Type_Lsid_RouterId (void *wap,
                                                         void *bufp[],
                                                         xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue, nextObjTypeValue;
  fpObjWa_t owaLsid = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLsidValue, nextObjLsidValue;
  fpObjWa_t owaRouterId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRouterIdValue, nextObjRouterIdValue;
  L7_ospfLsdbEntry_t temp_Lsa;
  void *outType = (void *) bufp[--keyCount];
  void *outLsid = (void *) bufp[--keyCount];
  void *outRouterId = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outType);
  FPOBJ_TRACE_ENTER (outLsid);
  FPOBJ_TRACE_ENTER (outRouterId);

  /* retrieve key: Type */
  owaType.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Type,
                              (xLibU8_t *) & objTypeValue, &owaType.len);
  if (owaType.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: Lsid */
    owaLsid.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Lsid,
                                (xLibU8_t *) & objLsidValue, &owaLsid.len);
    if (owaLsid.rc == XLIBRC_SUCCESS)
    {
      /* retrieve key: RouterId */
      owaRouterId.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_RouterId,
                                      (xLibU8_t *) & objRouterIdValue,
                                      &owaRouterId.len);
    }
  }
  else
  {
     objTypeValue = 0;
     nextObjTypeValue = 0;
     objLsidValue = 0;
     nextObjLsidValue = 0;
     objRouterIdValue = 0;
     nextObjRouterIdValue = 0;
  }

  FPOBJ_TRACE_CURRENT_KEY (outType, &objTypeValue, owaType.len);
  FPOBJ_TRACE_CURRENT_KEY (outLsid, &objLsidValue, owaLsid.len);
  FPOBJ_TRACE_CURRENT_KEY (outRouterId, &objRouterIdValue, owaRouterId.len);

  owa.rc =
    usmDbOspfExtLsdbEntryNext(L7_UNIT_CURRENT, &objTypeValue, &objLsidValue, &objRouterIdValue, &temp_Lsa);


  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outType, owaType);
    FPOBJ_TRACE_EXIT (outLsid, owaLsid);
    FPOBJ_TRACE_EXIT (outRouterId, owaRouterId);
    return owa.rc;
  }

  nextObjTypeValue = objTypeValue;
  nextObjLsidValue = objLsidValue;
  nextObjRouterIdValue = objRouterIdValue;

  FPOBJ_TRACE_CURRENT_KEY (outType, &nextObjTypeValue, owaType.len);
  FPOBJ_TRACE_CURRENT_KEY (outLsid, &nextObjLsidValue, owaLsid.len);
  FPOBJ_TRACE_CURRENT_KEY (outRouterId, &nextObjRouterIdValue, owaRouterId.len);

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
  FPOBJ_TRACE_EXIT (outType, owaType);
  FPOBJ_TRACE_EXIT (outLsid, owaLsid);
  FPOBJ_TRACE_EXIT (outRouterId, owaRouterId);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_routingospfExtLsdb_Sequence
*
* @purpose Get 'Sequence'
*
* @description [Sequence]: The sequence number field is a signed 32-bit integer.
*              It is used to detect old and dupli- cate link state
*              advertisements. The space of sequence numbers is linearly ordered.
*              The larger the sequence number the more recent the
*              advertisement. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfExtLsdb_Sequence (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsidValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSequenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Type */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Type,
                           (xLibU8_t *) & keyTypeValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTypeValue, kwa1.len);

  /* retrieve key: Lsid */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Lsid,
                           (xLibU8_t *) & keyLsidValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsidValue, kwa2.len);

  /* retrieve key: RouterId */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_RouterId,
                           (xLibU8_t *) & keyRouterIdValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRouterIdValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfExtLsdbSequenceGet (L7_UNIT_CURRENT, keyTypeValue,
                                          keyLsidValue,
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
* @function fpObjGet_routingospfExtLsdb_Age
*
* @purpose Get 'Age'
*
* @description [Age]: This field is the age of the link state adver- tisement
*              in seconds. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfExtLsdb_Age (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsidValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAgeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Type */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Type,
                           (xLibU8_t *) & keyTypeValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTypeValue, kwa1.len);

  /* retrieve key: Lsid */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Lsid,
                           (xLibU8_t *) & keyLsidValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsidValue, kwa2.len);

  /* retrieve key: RouterId */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_RouterId,
                           (xLibU8_t *) & keyRouterIdValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRouterIdValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfExtLsdbAgeGet (L7_UNIT_CURRENT, keyTypeValue,
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
* @function fpObjGet_routingospfExtLsdb_Checksum
*
* @purpose Get 'Checksum'
*
* @description [Checksum]: This field is the checksum of the complete contents
*              of the advertisement, excepting the age field. The age
*              field is excepted so that an advertisement's age can be incremented
*              without updating the checksum. The checksum used
*              is the same that is used for ISO connec- tionless datagrams;
*              it is commonly referred to as the Fletcher checksum. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfExtLsdb_Checksum (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsidValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRouterIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objChecksumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Type */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Type,
                           (xLibU8_t *) & keyTypeValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTypeValue, kwa1.len);

  /* retrieve key: Lsid */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Lsid,
                           (xLibU8_t *) & keyLsidValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsidValue, kwa2.len);

  /* retrieve key: RouterId */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_RouterId,
                           (xLibU8_t *) & keyRouterIdValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRouterIdValue, kwa3.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfExtLsdbChecksumGet (L7_UNIT_CURRENT, keyTypeValue,
                                          keyLsidValue,
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
* @function fpObjGet_routingospfExtLsdb_Advertisement
*
* @purpose Get 'Advertisement'
*
* @description [Advertisement]: The entire Link State Advertisement, including
*              its header. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfExtLsdb_Advertisement (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLsidValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRouterIdValue;
  xLibU32_t len;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAdvertisementValue;
  xLibS8_t *Str;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Type */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Type,
                           (xLibU8_t *) & keyTypeValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTypeValue, kwa1.len);

  /* retrieve key: Lsid */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_Lsid,
                           (xLibU8_t *) & keyLsidValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLsidValue, kwa2.len);

  /* retrieve key: RouterId */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routingospfExtLsdb_RouterId,
                           (xLibU8_t *) & keyRouterIdValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRouterIdValue, kwa3.len);

  /* get the value from application */
  Str = objAdvertisementValue;
  owa.l7rc = usmDbOspfExtLsdbAdvertisementGet (L7_UNIT_CURRENT, keyTypeValue,
                                               keyLsidValue,
                                               keyRouterIdValue,
                                               &Str, &len);
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
