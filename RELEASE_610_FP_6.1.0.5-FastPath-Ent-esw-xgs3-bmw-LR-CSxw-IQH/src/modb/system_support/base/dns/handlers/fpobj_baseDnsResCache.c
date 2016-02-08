/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDnsResCache.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dns-object.xml
*
* @create  4 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseDnsResCache_obj.h"
#include "usmdb_dns_client_api.h"

/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRName
*
* @purpose Get 'RRName'
*
* @description [RRName] Owner name of the Resource Record in the cache which is identified in this row of the table. As described in RFC-1034, the owner of the record is the domain name were the RR is found.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRName (void *wap, void *bufp)
{

  xLibStr256_t objRRNameValue;
  xLibStr256_t nextObjRRNameValue;
  xLibU32_t objRRClassValue;
  xLibU32_t objRRTypeValue;
  xLibU32_t objRRIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t qname;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RRName */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName, (xLibU8_t *) objRRNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objRRNameValue,0x00,sizeof(objRRNameValue));
    objRRClassValue = objRRTypeValue = objRRIndexValue = 0;
    owa.l7rc = usmDbDNSClientCacheRRNextGet(objRRNameValue,
                &objRRTypeValue, &objRRIndexValue, &objRRClassValue, qname);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objRRNameValue, owa.len);
    strcpy(nextObjRRNameValue,objRRNameValue);
    objRRClassValue = objRRTypeValue = objRRIndexValue = 0;
    do
    {
      owa.l7rc = usmDbDNSClientCacheRRNextGet(objRRNameValue,
                &objRRTypeValue, &objRRIndexValue, &objRRClassValue, qname);
    }
    while ((strncmp(objRRNameValue, nextObjRRNameValue, sizeof(objRRNameValue)) == 0) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  strcpy(nextObjRRNameValue,objRRNameValue);
  FPOBJ_TRACE_NEW_KEY (bufp, objRRNameValue, owa.len);

  /* return the object value: RRName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objRRNameValue, strlen (objRRNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRClass
*
* @purpose Get 'RRClass'
*
* @description [RRClass] DNS class of the Resource Record in the cache which is identified in this row of the table.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRClass (void *wap, void *bufp)
{

  xLibStr256_t objRRNameValue;
  xLibU32_t objRRClassValue;
  xLibU32_t nextObjRRClassValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  memset(objRRNameValue,0x00,sizeof(objRRNameValue));
  /* retrieve key: RRName */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName, (xLibU8_t *) objRRNameValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objRRNameValue, kwa.len);

  /* retrieve key: RRClass */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                          (xLibU8_t *) & objRRClassValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objRRClassValue = DNS_RR_CLASS_INTERNET;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRRClassValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjRRClassValue = objRRClassValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRRClassValue, owa.len);

  /* return the object value: RRClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRRClassValue, sizeof (objRRClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRType
*
* @purpose Get 'RRType'
*
* @description [RRType] DNS type of the Resource Record in the cache which is identified in this row of the table.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRType (void *wap, void *bufp)
{

  xLibStr256_t objRRNameValue;
  xLibStr256_t nextObjRRNameValue;
  xLibU32_t objRRClassValue;
  xLibU32_t nextObjRRClassValue;
  xLibU32_t objRRTypeValue;
  xLibU32_t nextObjRRTypeValue;
  xLibU32_t objRRIndexValue;
  xLibStr256_t qname;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset(objRRNameValue,0x00,sizeof(objRRNameValue));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName, (xLibU8_t *) objRRNameValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                          (xLibU8_t *) & objRRClassValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                          (xLibU8_t *) & objRRTypeValue, &owa.len); 
  
  nextObjRRClassValue = objRRClassValue;
  memset(nextObjRRNameValue,0x00,sizeof(nextObjRRNameValue));
  osapiStrncpy(nextObjRRNameValue,objRRNameValue, sizeof(objRRNameValue));

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objRRTypeValue = objRRIndexValue = 0;
    owa.l7rc = usmDbDNSClientCacheRRNextGet(objRRNameValue, 
                  &objRRTypeValue, &objRRIndexValue, &objRRClassValue, qname);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRRTypeValue, owa.len);
    nextObjRRTypeValue = objRRTypeValue;
    objRRTypeValue = 0;  
    objRRIndexValue = 0;
    do
    {
      owa.l7rc = usmDbDNSClientCacheRRNextGet(objRRNameValue, 
                  &objRRTypeValue, &objRRIndexValue, &objRRClassValue, qname);
    }
    while ((strncmp(objRRNameValue, nextObjRRNameValue, sizeof(objRRNameValue)) == 0)
           && (objRRTypeValue != nextObjRRTypeValue) && (owa.l7rc == L7_SUCCESS));

    if((owa.l7rc == L7_SUCCESS) &&
       (strncmp(objRRNameValue, nextObjRRNameValue, sizeof(objRRNameValue)) == 0) &&
       (objRRTypeValue == nextObjRRTypeValue))
    {
      do
      {
        owa.l7rc = usmDbDNSClientCacheRRNextGet(objRRNameValue, 
                    &objRRTypeValue, &objRRIndexValue, &objRRClassValue, qname);
      }
      while((strncmp(objRRNameValue, nextObjRRNameValue, sizeof(objRRNameValue)) == 0) &&
            (objRRTypeValue == nextObjRRTypeValue) && (owa.l7rc == L7_SUCCESS));
    }
  }

  if ((strncmp(objRRNameValue, nextObjRRNameValue, sizeof(objRRNameValue)) != 0)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjRRTypeValue = objRRTypeValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRRTypeValue, owa.len);

  /* return the object value: RRType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRRTypeValue, sizeof (objRRTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRIndex
*
* @purpose Get 'RRIndex'
*
* @description [RRIndex] A value which makes entries in the table unique when the other index values (dnsResCacheRRName, dnsResCacheRRClass, and dnsResCacheRRType) do not provide a unique index.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRIndex (void *wap, void *bufp)
{

  xLibStr256_t objRRNameValue;
  xLibStr256_t nextObjRRNameValue;
  xLibU32_t objRRClassValue;
  xLibU32_t nextObjRRClassValue;
  xLibU32_t objRRTypeValue;
  xLibU32_t nextObjRRTypeValue;
  xLibU32_t objRRIndexValue;
  xLibU32_t nextObjRRIndexValue;
  xLibStr256_t qname;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset(objRRNameValue,0x00,sizeof(objRRNameValue));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName, (xLibU8_t *) objRRNameValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                          (xLibU8_t *) & objRRClassValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                          (xLibU8_t *) & objRRTypeValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objRRTypeValue, kwa3.len);

  /* retrieve key: RRIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRIndex,
                          (xLibU8_t *) & objRRIndexValue, &owa.len);

  nextObjRRClassValue = objRRClassValue;
  memset(nextObjRRNameValue,0x00,sizeof(nextObjRRNameValue));
  osapiStrncpy(nextObjRRNameValue,objRRNameValue, sizeof(objRRNameValue));
  nextObjRRTypeValue = objRRTypeValue;

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objRRTypeValue = 0;  
    objRRIndexValue = 0;
    do
    {
      owa.l7rc = usmDbDNSClientCacheRRNextGet(objRRNameValue, 
                  &objRRTypeValue, &objRRIndexValue, &objRRClassValue, qname);
    }
    while ((strncmp(objRRNameValue, nextObjRRNameValue, sizeof(objRRNameValue)) == 0)
           && (objRRTypeValue != nextObjRRTypeValue) && (owa.l7rc == L7_SUCCESS));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRRIndexValue, owa.len);
    owa.l7rc = usmDbDNSClientCacheRRNextGet(objRRNameValue,
                      &objRRTypeValue, &objRRIndexValue, &objRRClassValue, qname);

  }

  if ((strncmp(objRRNameValue, nextObjRRNameValue, sizeof(objRRNameValue)) != 0)
      || (objRRTypeValue != nextObjRRTypeValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjRRIndexValue = objRRIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRRIndexValue, owa.len);

  /* return the object value: RRIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRRIndexValue, sizeof (objRRIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRTTL
*
* @purpose Get 'RRTTL'
*
* @description [RRTTL]: Time-To-Live of RR in DNS cache. This is the initial
*              TTL value which was received with the RR when it was originally
*              received. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRTTL (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRRNameValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRClassValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRTypeValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRRTTLValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(keyRRNameValue,0x00,sizeof(keyRRNameValue));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName,
                           (xLibU8_t *) keyRRNameValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                           (xLibU8_t *) & keyRRClassValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                           (xLibU8_t *) & keyRRTypeValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRTypeValue, kwa3.len);

  /* retrieve key: RRIndex */
  kwa4.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRIndex,
                           (xLibU8_t *) & keyRRIndexValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRIndexValue, kwa4.len);

  /* get the value from application */
  owa.l7rc = usmDbDNSClientCacheRRTTLGet (keyRRNameValue,
                                          keyRRTypeValue,
                                          keyRRIndexValue, &objRRTTLValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RRTTL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRRTTLValue,
                           sizeof (objRRTTLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRElapsedTTL
*
* @purpose Get 'RRElapsedTTL'
*
* @description [RRElapsedTTL]: Elapsed seconds since RR was received. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRElapsedTTL (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRRNameValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRClassValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRTypeValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRRElapsedTTLValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(keyRRNameValue,0x00,sizeof(keyRRNameValue));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName,
                           (xLibU8_t *) keyRRNameValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                           (xLibU8_t *) & keyRRClassValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                           (xLibU8_t *) & keyRRTypeValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRTypeValue, kwa3.len);

  /* retrieve key: RRIndex */
  kwa4.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRIndex,
                           (xLibU8_t *) & keyRRIndexValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRIndexValue, kwa4.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCacheRRTTLElapsedGet (keyRRNameValue,
                                        keyRRTypeValue,
                                        keyRRIndexValue, &objRRElapsedTTLValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RRElapsedTTL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRRElapsedTTLValue,
                           sizeof (objRRElapsedTTLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRSource
*
* @purpose Get 'RRSource'
*
* @description [RRSource]: Host from which RR was received, 0.0.0.0 if unknown.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRSource (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRRNameValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRClassValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRTypeValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRRSourceValue;

  L7_inet_addr_t sourceAddr;

  FPOBJ_TRACE_ENTER (bufp);

  inetAddressReset(&sourceAddr);

  memset(keyRRNameValue,0x00,sizeof(keyRRNameValue));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName,
                           (xLibU8_t *) keyRRNameValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                           (xLibU8_t *) & keyRRClassValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                           (xLibU8_t *) & keyRRTypeValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRTypeValue, kwa3.len);

  /* retrieve key: RRIndex */
  kwa4.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRIndex,
                           (xLibU8_t *) & keyRRIndexValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRIndexValue, kwa4.len);

  /* get the value from application */
  owa.l7rc = usmDbDNSClientCacheRRSourceGet ( keyRRNameValue,
                                             keyRRTypeValue,
                                             keyRRIndexValue,
                                             &sourceAddr);

  inetAddressGet(L7_AF_INET, &sourceAddr, (void*)&objRRSourceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RRSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRRSourceValue,
                           sizeof (objRRSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRData
*
* @purpose Get 'RRData'
*
* @description [RRData]: RDATA portion of a cached RR. The value is in the
*              format defined for the particular DNS class and type of the
*              resource record. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRData (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRRNameValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRClassValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRTypeValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRRDataValue;
  xLibStr256_t hostname, strAddr;

  L7_BOOL   primary = L7_FALSE;

  FPOBJ_TRACE_ENTER (bufp);

  memset(keyRRNameValue,0x00,sizeof(keyRRNameValue));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName,
                           (xLibU8_t *) keyRRNameValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                           (xLibU8_t *) & keyRRClassValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                           (xLibU8_t *) & keyRRTypeValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRTypeValue, kwa3.len);

  /* retrieve key: RRIndex */
  kwa4.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRIndex,
                           (xLibU8_t *) & keyRRIndexValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRIndexValue, kwa4.len);

  memset(objRRDataValue, 0, sizeof(objRRDataValue));
  /* get the value from application */
  owa.l7rc = usmDbDNSClientCacheRRDataGet ( keyRRNameValue,
                                           keyRRTypeValue,
                                           keyRRIndexValue, objRRDataValue,
                                           &primary);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    memset(strAddr, 0, sizeof(strAddr));
    if ( keyRRTypeValue == DNS_RR_TYPE_ADDRESS)
    {
      inetAddrHtop((L7_inet_addr_t *)objRRDataValue, strAddr);
    }
    else if (keyRRTypeValue == DNS_RR_TYPE_IPV6_ADDRESS)
    {
      inetAddrHtop((L7_inet_addr_t *)objRRDataValue, strAddr);
    }
    else
    {
      memset(hostname, 0, sizeof(hostname));
      usmDbDNSClientDisplayNameGet(objRRDataValue, hostname);
      osapiStrncpy(strAddr, hostname, sizeof(strAddr));
      strAddr[sizeof(strAddr)-1] = 0;
    }
  }

  /* return the object value: RRData */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) strAddr,
                           strlen (strAddr));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRStatus
*
* @purpose Get 'RRStatus'
*
* @description [RRStatus]: Status column for the resolver cache table. Since
*              only the agent (DNS resolver) creates rows in this table,
*              the only values that a manager may write to this variable
*              are active(1) and destroy(6). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRRNameValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRClassValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRTypeValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRRStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(keyRRNameValue,0x00,sizeof(keyRRNameValue));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName,
                           (xLibU8_t *) keyRRNameValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                           (xLibU8_t *) & keyRRClassValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                           (xLibU8_t *) & keyRRTypeValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRTypeValue, kwa3.len);

  /* retrieve key: RRIndex */
  kwa4.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRIndex,
                           (xLibU8_t *) & keyRRIndexValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRIndexValue, kwa4.len);

  /* get the value from application */
  objRRStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: RRStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRRStatusValue,
                           sizeof (objRRStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDnsResCache_RRStatus
*
* @purpose Set 'RRStatus'
*
* @description [RRStatus]: Status column for the resolver cache table. Since
*              only the agent (DNS resolver) creates rows in this table,
*              the only values that a manager may write to this variable
*              are active(1) and destroy(6). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDnsResCache_RRStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRRStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRRNameValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRClassValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRTypeValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRIndexValue;
  xLibStr256_t objRRPrettyNameValue;
  xLibStr256_t hostname;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RRStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRRStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRRStatusValue, owa.len);

  memset(keyRRNameValue,0x00,sizeof(keyRRNameValue));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName,
                           (xLibU8_t *) keyRRNameValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                           (xLibU8_t *) & keyRRClassValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                           (xLibU8_t *) & keyRRTypeValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRTypeValue, kwa3.len);

  /* retrieve key: RRIndex */
  kwa4.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRIndex,
                           (xLibU8_t *) & keyRRIndexValue, &kwa4.len);
  if (kwa4.rc != XLIBRC_SUCCESS)
  {
    kwa4.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRIndexValue, kwa4.len);

  /* call the usmdb only for add and delete */
  if (objRRStatusValue == L7_ROW_STATUS_ACTIVE)
  {
    owa.rc = XLIBRC_SUCCESS;  /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objRRStatusValue == L7_ROW_STATUS_DESTROY)
  {
    memset(objRRPrettyNameValue, 0, sizeof(objRRPrettyNameValue));

    /* get the value from application */
    owa.l7rc =
      usmDbDNSClientCacheRRPrettyNameGet (keyRRNameValue,
                                          keyRRTypeValue,
                                          keyRRIndexValue, objRRPrettyNameValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else
    {
      memset(hostname, 0, sizeof(hostname));
      usmDbDNSClientDisplayNameGet( objRRPrettyNameValue, hostname);

      /* Delete the existing row */
      owa.l7rc = usmDbDNSClientCacheHostFlush (hostname);
    }

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
* @function fpObjGet_baseDnsResCache_RRPrettyName
*
* @purpose Get 'RRPrettyName'
*
* @description [RRPrettyName]: Name of the RR at this row in the table. This
*              is identical to the dnsResCacheRRName variable, except that
*              character case is preserved in this variable, per DNS conventions.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRPrettyName (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRRNameValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRClassValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRTypeValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRRPrettyNameValue;
  xLibStr256_t hostname;  
  FPOBJ_TRACE_ENTER (bufp);

  memset(keyRRNameValue,0x00,sizeof(keyRRNameValue));
  memset(hostname,0x00,sizeof(hostname));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName,
                           (xLibU8_t *) keyRRNameValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                           (xLibU8_t *) & keyRRClassValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                           (xLibU8_t *) & keyRRTypeValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRTypeValue, kwa3.len);

  /* retrieve key: RRIndex */
  kwa4.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRIndex,
                           (xLibU8_t *) & keyRRIndexValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRIndexValue, kwa4.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCacheRRPrettyNameGet ( keyRRNameValue,
                                        keyRRTypeValue,
                                        keyRRIndexValue, objRRPrettyNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.l7rc =  
     usmDbDNSClientDisplayNameGet(objRRPrettyNameValue, hostname);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RRPrettyName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) hostname,
                           strlen (hostname));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseDnsResCache_RRDisplayName
*
* @purpose Get 'RRDisplayName'
*
* @description [RRDisplayName]: Display name of the RR at this row in the table.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_RRDisplayName (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRRNameValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRClassValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRTypeValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRRIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objRRPrettyNameValue;
  xLibStr256_t hostname;
  FPOBJ_TRACE_ENTER (bufp);

  memset(keyRRNameValue,0x00,sizeof(keyRRNameValue));
  /* retrieve key: RRName */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRName,
                           (xLibU8_t *) keyRRNameValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRRNameValue, kwa1.len);

  /* retrieve key: RRClass */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRClass,
                           (xLibU8_t *) & keyRRClassValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRClassValue, kwa2.len);

  /* retrieve key: RRType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRType,
                           (xLibU8_t *) & keyRRTypeValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRTypeValue, kwa3.len);

  /* retrieve key: RRIndex */
  kwa4.rc = xLibFilterGet (wap, XOBJ_baseDnsResCache_RRIndex,
                           (xLibU8_t *) & keyRRIndexValue, &kwa4.len);
  if (kwa4.l7rc != L7_SUCCESS)
  {
    kwa4.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa4);
    return kwa4.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRRIndexValue, kwa4.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCacheRRPrettyNameGet ( keyRRNameValue,
                                        keyRRTypeValue,
                                        keyRRIndexValue, objRRPrettyNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    memset(hostname, 0, sizeof(hostname));
    usmDbDNSClientDisplayNameGet( objRRPrettyNameValue, hostname);
  }

  /* return the object value: RRPrettyName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) hostname,
                           strlen (hostname));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseDnsResCache_BadCaches
*
* @purpose Get 'BadCaches'
*
* @description Number of RRs the resolver has refused to cache because they appear 
*              to be dangerous or irrelevant. E.g., RRs with suspiciously 
*              high TTLs, unsolicited root information, or that just don't 
*              appear to be relevant to 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_BadCaches (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBadCachesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCacheBadCachesGet (&objBadCachesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBadCachesValue, sizeof (objBadCachesValue));

  /* return the object value: BadCaches */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadCachesValue,
                           sizeof (objBadCachesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCache_MaxTTL
*
* @purpose Get 'MaxTTL'
*
* @description Maximum Time-To-Live for RRs in this cache. If the resolver does 
*              not implement a TTL ceiling, the value of this field should 
*              be zero. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_MaxTTL (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxTTLValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDNSClientCacheMaxTTLGet (&objMaxTTLValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxTTLValue, sizeof (objMaxTTLValue));

  /* return the object value: MaxTTL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxTTLValue,
                           sizeof (objMaxTTLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDnsResCache_GoodCaches
*
* @purpose Get 'GoodCaches'
*
* @description Number of RRs the resolver has cached successfully. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDnsResCache_GoodCaches (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGoodCachesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDNSClientCacheGoodCachesGet (&objGoodCachesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGoodCachesValue, sizeof (objGoodCachesValue));

  /* return the object value: GoodCaches */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGoodCachesValue,
                           sizeof (objGoodCachesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDnsResCache_FlushStatus
*
* @purpose Set 'FlushStatus'
*
* @description The cache Flush status. If set to enable all dynamic cache entries 
*              would be removed. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDnsResCache_FlushStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFlushStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FlushStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objFlushStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFlushStatusValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDNSClientCacheFlush ();
  if (owa.l7rc != L7_SUCCESS)
  {
    objFlushStatusValue = L7_DISABLE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objFlushStatusValue = L7_ENABLE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

