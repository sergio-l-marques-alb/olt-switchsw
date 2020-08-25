/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingSwitchSnoopingQuerierGroup.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Snooping-object.xml
*
* @create  13 February 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingSwitchSnoopingQuerierGroup_obj.h"
#include "usmdb_snooping_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol
*
* @purpose Get 'SnoopingProtocol'
*
* @description [SnoopingProtocol]: The protocol type of network protocol in
*              use 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  L7_BOOL brc;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnoopingProtocolValue;
  xLibU8_t nextObjSnoopingProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & objSnoopingProtocolValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  { 
    objSnoopingProtocolValue = 0;
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    brc = usmDbSnoopProtocolNextGet ((xLibU8_t)objSnoopingProtocolValue,
                                          &nextObjSnoopingProtocolValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSnoopingProtocolValue, owa.len);
    brc =
      usmDbSnoopProtocolNextGet ((xLibU8_t)objSnoopingProtocolValue,
                                  &nextObjSnoopingProtocolValue);
  }
  if (brc != L7_TRUE)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSnoopingProtocolValue, owa.len);

  /* return the object value: SnoopingProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSnoopingProtocolValue,
                           sizeof (nextObjSnoopingProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierAdminMode
*
* @purpose Get 'QuerierAdminMode'
*
* @description [QuerierAdminMode]: This enables or disables Snooping Querier
*              on the system 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierAdminMode (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnoopQuerierAdminModeGet ( &objQuerierAdminModeValue,
                                    keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierAdminModeValue,
                           sizeof (objQuerierAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierAdminMode
*
* @purpose Set 'QuerierAdminMode'
*
* @description [QuerierAdminMode]: This enables or disables Snooping Querier
*              on the system 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierAdminMode (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierAdminModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: QuerierAdminMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objQuerierAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objQuerierAdminModeValue, owa.len);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSnoopQuerierAdminModeSet ( objQuerierAdminModeValue,
                                    keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierVersion
*
* @purpose Get 'QuerierVersion'
*
* @description [QuerierVersion]: This specifies the multicast protocol versions
*              that are supported by the system 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierVersion (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnoopQuerierVersionGet ( &objQuerierVersionValue,
                                  keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierVersionValue,
                           sizeof (objQuerierVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierVersion
*
* @purpose Set 'QuerierVersion'
*
* @description [QuerierVersion]: This specifies the multicast protocol versions
*              that are supported by the system 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierVersion (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierVersionValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: QuerierVersion */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objQuerierVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objQuerierVersionValue, owa.len);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSnoopQuerierVersionSet ( objQuerierVersionValue,
                                  keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierAddress
*
* @purpose Get 'QuerierAddress'
*
* @description [QuerierAddress]: This specifies the default source ip address
*              to be used while generating general queries. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierAddress (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  L7_inet_addr_t objQuerierAddressValue;
  xLibStr256_t querierAddress;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnoopQuerierAddressGet( querierAddress,
                                  keySnoopingProtocolValue);

  memset(&objQuerierAddressValue,0x0,sizeof(L7_inet_addr_t));
  inetAddressSet( keySnoopingProtocolValue, querierAddress,
                 (L7_inet_addr_t *)&objQuerierAddressValue );

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objQuerierAddressValue,
                           sizeof(objQuerierAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierAddress
*
* @purpose Set 'QuerierAddress'
*
* @description [QuerierAddress]: This specifies the default source ip address
*              to be used while generating general queries. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierAddress (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objQuerierAddressValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  xLibStr256_t querierAddress;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: QuerierAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objQuerierAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objQuerierAddressValue, owa.len);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  memset(querierAddress,0x0,sizeof(querierAddress));
  inetAddressGet(keySnoopingProtocolValue,&objQuerierAddressValue,querierAddress );

  /* set the value in application */
  owa.l7rc =
    usmDbSnoopQuerierAddressSet ( querierAddress,
                                 keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierQueryInterval
*
* @purpose Get 'QuerierQueryInterval'
*
* @description [QuerierQueryInterval]: This specified the timer interval after
*              which the system generates general queries. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierQueryInterval (void
                                                                            *wap,
                                                                            void
                                                                            *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierQueryIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnoopQuerierQueryIntervalGet ( &objQuerierQueryIntervalValue,
                                        keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierQueryInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierQueryIntervalValue,
                           sizeof (objQuerierQueryIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierQueryInterval
*
* @purpose Set 'QuerierQueryInterval'
*
* @description [QuerierQueryInterval]: This specified the timer interval after
*              which the system generates general queries. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierQueryInterval (void
                                                                            *wap,
                                                                            void
                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierQueryIntervalValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: QuerierQueryInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objQuerierQueryIntervalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objQuerierQueryIntervalValue, owa.len);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSnoopQuerierQueryIntervalSet ( objQuerierQueryIntervalValue,
                                        keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierExpiryInterval
*
* @purpose Get 'QuerierExpiryInterval'
*
* @description [QuerierExpiryInterval]: This specified the timer interval
*              after which the detected other querier information is no longer
*              valid. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingSwitchSnoopingQuerierGroup_QuerierExpiryInterval (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierExpiryIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnoopQuerierExpiryIntervalGet ( &objQuerierExpiryIntervalValue,
                                         keySnoopingProtocolValue );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: QuerierExpiryInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objQuerierExpiryIntervalValue,
                           sizeof (objQuerierExpiryIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierExpiryInterval
*
* @purpose Set 'QuerierExpiryInterval'
*
* @description [QuerierExpiryInterval]: This specified the timer interval
*              after which the detected other querier information is no longer
*              valid. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_SwitchingSwitchSnoopingQuerierGroup_QuerierExpiryInterval (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objQuerierExpiryIntervalValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnoopingProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: QuerierExpiryInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objQuerierExpiryIntervalValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objQuerierExpiryIntervalValue, owa.len);

  /* retrieve key: SnoopingProtocol */
  kwa.rc =
    xLibFilterGet (wap,
                   XOBJ_SwitchingSwitchSnoopingQuerierGroup_SnoopingProtocol,
                   (xLibU8_t *) & keySnoopingProtocolValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnoopingProtocolValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSnoopQuerierExpiryIntervalSet ( objQuerierExpiryIntervalValue,
                                         keySnoopingProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
