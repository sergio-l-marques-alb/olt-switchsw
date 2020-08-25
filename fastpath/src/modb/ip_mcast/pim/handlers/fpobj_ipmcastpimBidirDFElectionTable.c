
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimBidirDFElectionTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimBidirDFElectionTable_obj.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType
*
* @purpose Get 'pimBidirDFElectionAddressType'
*
* @description [pimBidirDFElectionAddressType] The address type of the RP for which the DF state is being maintained.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType (void *wap,
                                                                                void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBidirDFElectionAddressTypeValue;
  xLibU32_t nextObjpimBidirDFElectionAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType,
                          (xLibU8_t *) & objpimBidirDFElectionAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjpimBidirDFElectionAddressTypeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBidirDFElectionAddressTypeValue, owa.len);
    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT, objpimBidirDFElectionAddressTypeValue,
                                    &nextObjpimBidirDFElectionAddressTypeValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBidirDFElectionAddressTypeValue, owa.len);

  /* return the object value: pimBidirDFElectionAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimBidirDFElectionAddressTypeValue,
                           sizeof (objpimBidirDFElectionAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress
*
* @purpose Get 'pimBidirDFElectionRPAddress'
*
* @description [pimBidirDFElectionRPAddress] The IP address of the RP for which the DF state is being maintained.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBidirDFElectionRPAddressValue;
  xLibU32_t nextObjpimBidirDFElectionRPAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionRPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress,
                          (xLibU8_t *) & objpimBidirDFElectionRPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjpimBidirDFElectionRPAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBidirDFElectionRPAddressValue, owa.len);
    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT, objpimBidirDFElectionRPAddressValue,
                                    &nextObjpimBidirDFElectionRPAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBidirDFElectionRPAddressValue, owa.len);

  /* return the object value: pimBidirDFElectionRPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimBidirDFElectionRPAddressValue,
                           sizeof (objpimBidirDFElectionRPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex
*
* @purpose Get 'pimBidirDFElectionIfIndex'
*
* @description [pimBidirDFElectionIfIndex] The value of ifIndex for the interface for which the DF state is being maintained.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBidirDFElectionIfIndexValue;
  xLibU32_t nextObjpimBidirDFElectionIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex,
                          (xLibU8_t *) & objpimBidirDFElectionIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjpimBidirDFElectionIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBidirDFElectionIfIndexValue, owa.len);
    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT, objpimBidirDFElectionIfIndexValue,
                                    &nextObjpimBidirDFElectionIfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBidirDFElectionIfIndexValue, owa.len);

  /* return the object value: pimBidirDFElectionIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimBidirDFElectionIfIndexValue,
                           sizeof (objpimBidirDFElectionIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerAddressType
*
* @purpose Get 'pimBidirDFElectionWinnerAddressType'
*
* @description [pimBidirDFElectionWinnerAddressType] The primary address type of the winner of the DF Election process. A value of unknown(0) indicates there is currently no DF.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerAddressType (void *wap,
                                                                                      void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBidirDFElectionAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionAddressTypeValue;
  fpObjWa_t kwapimBidirDFElectionRPAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionRPAddressValue;
  fpObjWa_t kwapimBidirDFElectionIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBidirDFElectionWinnerAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionAddressType */
  kwapimBidirDFElectionAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType,
                   (xLibU8_t *) & keypimBidirDFElectionAddressTypeValue,
                   &kwapimBidirDFElectionAddressType.len);
  if (kwapimBidirDFElectionAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionAddressType);
    return kwapimBidirDFElectionAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionAddressTypeValue,
                           kwapimBidirDFElectionAddressType.len);

  /* retrieve key: pimBidirDFElectionRPAddress */
  kwapimBidirDFElectionRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress,
                   (xLibU8_t *) & keypimBidirDFElectionRPAddressValue,
                   &kwapimBidirDFElectionRPAddress.len);
  if (kwapimBidirDFElectionRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionRPAddress);
    return kwapimBidirDFElectionRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionRPAddressValue,
                           kwapimBidirDFElectionRPAddress.len);

  /* retrieve key: pimBidirDFElectionIfIndex */
  kwapimBidirDFElectionIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex,
                   (xLibU8_t *) & keypimBidirDFElectionIfIndexValue,
                   &kwapimBidirDFElectionIfIndex.len);
  if (kwapimBidirDFElectionIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionIfIndex);
    return kwapimBidirDFElectionIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionIfIndexValue,
                           kwapimBidirDFElectionIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBidirDFElectionAddressTypeValue,
                              keypimBidirDFElectionRPAddressValue,
                              keypimBidirDFElectionIfIndexValue,
                              &objpimBidirDFElectionWinnerAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBidirDFElectionWinnerAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBidirDFElectionWinnerAddressTypeValue,
                           sizeof (objpimBidirDFElectionWinnerAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerAddress
*
* @purpose Get 'pimBidirDFElectionWinnerAddress'
*
* @description [pimBidirDFElectionWinnerAddress] The primary IP address of the winner of the DF Election process.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerAddress (void *wap,
                                                                                  void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBidirDFElectionAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionAddressTypeValue;
  fpObjWa_t kwapimBidirDFElectionRPAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionRPAddressValue;
  fpObjWa_t kwapimBidirDFElectionIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimBidirDFElectionWinnerAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionAddressType */
  kwapimBidirDFElectionAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType,
                   (xLibU8_t *) & keypimBidirDFElectionAddressTypeValue,
                   &kwapimBidirDFElectionAddressType.len);
  if (kwapimBidirDFElectionAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionAddressType);
    return kwapimBidirDFElectionAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionAddressTypeValue,
                           kwapimBidirDFElectionAddressType.len);

  /* retrieve key: pimBidirDFElectionRPAddress */
  kwapimBidirDFElectionRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress,
                   (xLibU8_t *) & keypimBidirDFElectionRPAddressValue,
                   &kwapimBidirDFElectionRPAddress.len);
  if (kwapimBidirDFElectionRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionRPAddress);
    return kwapimBidirDFElectionRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionRPAddressValue,
                           kwapimBidirDFElectionRPAddress.len);

  /* retrieve key: pimBidirDFElectionIfIndex */
  kwapimBidirDFElectionIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex,
                   (xLibU8_t *) & keypimBidirDFElectionIfIndexValue,
                   &kwapimBidirDFElectionIfIndex.len);
  if (kwapimBidirDFElectionIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionIfIndex);
    return kwapimBidirDFElectionIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionIfIndexValue,
                           kwapimBidirDFElectionIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBidirDFElectionAddressTypeValue,
                              keypimBidirDFElectionRPAddressValue,
                              keypimBidirDFElectionIfIndexValue,
                              objpimBidirDFElectionWinnerAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBidirDFElectionWinnerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objpimBidirDFElectionWinnerAddressValue,
                           sizeof (objpimBidirDFElectionWinnerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerUpTime
*
* @purpose Get 'pimBidirDFElectionWinnerUpTime'
*
* @description [pimBidirDFElectionWinnerUpTime] The time since the current winner (last) became elected as the DF for this RP.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerUpTime (void *wap,
                                                                                 void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBidirDFElectionAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionAddressTypeValue;
  fpObjWa_t kwapimBidirDFElectionRPAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionRPAddressValue;
  fpObjWa_t kwapimBidirDFElectionIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBidirDFElectionWinnerUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionAddressType */
  kwapimBidirDFElectionAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType,
                   (xLibU8_t *) & keypimBidirDFElectionAddressTypeValue,
                   &kwapimBidirDFElectionAddressType.len);
  if (kwapimBidirDFElectionAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionAddressType);
    return kwapimBidirDFElectionAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionAddressTypeValue,
                           kwapimBidirDFElectionAddressType.len);

  /* retrieve key: pimBidirDFElectionRPAddress */
  kwapimBidirDFElectionRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress,
                   (xLibU8_t *) & keypimBidirDFElectionRPAddressValue,
                   &kwapimBidirDFElectionRPAddress.len);
  if (kwapimBidirDFElectionRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionRPAddress);
    return kwapimBidirDFElectionRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionRPAddressValue,
                           kwapimBidirDFElectionRPAddress.len);

  /* retrieve key: pimBidirDFElectionIfIndex */
  kwapimBidirDFElectionIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex,
                   (xLibU8_t *) & keypimBidirDFElectionIfIndexValue,
                   &kwapimBidirDFElectionIfIndex.len);
  if (kwapimBidirDFElectionIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionIfIndex);
    return kwapimBidirDFElectionIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionIfIndexValue,
                           kwapimBidirDFElectionIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBidirDFElectionAddressTypeValue,
                              keypimBidirDFElectionRPAddressValue,
                              keypimBidirDFElectionIfIndexValue,
                              &objpimBidirDFElectionWinnerUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBidirDFElectionWinnerUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBidirDFElectionWinnerUpTimeValue,
                           sizeof (objpimBidirDFElectionWinnerUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerMetricPref
*
* @purpose Get 'pimBidirDFElectionWinnerMetricPref'
*
* @description [pimBidirDFElectionWinnerMetricPref] The metric preference advertised by the DF Winner, or zero if there is currently no DF.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerMetricPref (void *wap,
                                                                                     void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBidirDFElectionAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionAddressTypeValue;
  fpObjWa_t kwapimBidirDFElectionRPAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionRPAddressValue;
  fpObjWa_t kwapimBidirDFElectionIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBidirDFElectionWinnerMetricPrefValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionAddressType */
  kwapimBidirDFElectionAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType,
                   (xLibU8_t *) & keypimBidirDFElectionAddressTypeValue,
                   &kwapimBidirDFElectionAddressType.len);
  if (kwapimBidirDFElectionAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionAddressType);
    return kwapimBidirDFElectionAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionAddressTypeValue,
                           kwapimBidirDFElectionAddressType.len);

  /* retrieve key: pimBidirDFElectionRPAddress */
  kwapimBidirDFElectionRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress,
                   (xLibU8_t *) & keypimBidirDFElectionRPAddressValue,
                   &kwapimBidirDFElectionRPAddress.len);
  if (kwapimBidirDFElectionRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionRPAddress);
    return kwapimBidirDFElectionRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionRPAddressValue,
                           kwapimBidirDFElectionRPAddress.len);

  /* retrieve key: pimBidirDFElectionIfIndex */
  kwapimBidirDFElectionIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex,
                   (xLibU8_t *) & keypimBidirDFElectionIfIndexValue,
                   &kwapimBidirDFElectionIfIndex.len);
  if (kwapimBidirDFElectionIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionIfIndex);
    return kwapimBidirDFElectionIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionIfIndexValue,
                           kwapimBidirDFElectionIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBidirDFElectionAddressTypeValue,
                              keypimBidirDFElectionRPAddressValue,
                              keypimBidirDFElectionIfIndexValue,
                              &objpimBidirDFElectionWinnerMetricPrefValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBidirDFElectionWinnerMetricPref */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBidirDFElectionWinnerMetricPrefValue,
                           sizeof (objpimBidirDFElectionWinnerMetricPrefValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerMetric
*
* @purpose Get 'pimBidirDFElectionWinnerMetric'
*
* @description [pimBidirDFElectionWinnerMetric] The metric advertised by the DF Winner, or zero if there is currently no DF.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionWinnerMetric (void *wap,
                                                                                 void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBidirDFElectionAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionAddressTypeValue;
  fpObjWa_t kwapimBidirDFElectionRPAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionRPAddressValue;
  fpObjWa_t kwapimBidirDFElectionIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBidirDFElectionWinnerMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionAddressType */
  kwapimBidirDFElectionAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType,
                   (xLibU8_t *) & keypimBidirDFElectionAddressTypeValue,
                   &kwapimBidirDFElectionAddressType.len);
  if (kwapimBidirDFElectionAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionAddressType);
    return kwapimBidirDFElectionAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionAddressTypeValue,
                           kwapimBidirDFElectionAddressType.len);

  /* retrieve key: pimBidirDFElectionRPAddress */
  kwapimBidirDFElectionRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress,
                   (xLibU8_t *) & keypimBidirDFElectionRPAddressValue,
                   &kwapimBidirDFElectionRPAddress.len);
  if (kwapimBidirDFElectionRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionRPAddress);
    return kwapimBidirDFElectionRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionRPAddressValue,
                           kwapimBidirDFElectionRPAddress.len);

  /* retrieve key: pimBidirDFElectionIfIndex */
  kwapimBidirDFElectionIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex,
                   (xLibU8_t *) & keypimBidirDFElectionIfIndexValue,
                   &kwapimBidirDFElectionIfIndex.len);
  if (kwapimBidirDFElectionIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionIfIndex);
    return kwapimBidirDFElectionIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionIfIndexValue,
                           kwapimBidirDFElectionIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBidirDFElectionAddressTypeValue,
                              keypimBidirDFElectionRPAddressValue,
                              keypimBidirDFElectionIfIndexValue,
                              &objpimBidirDFElectionWinnerMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBidirDFElectionWinnerMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBidirDFElectionWinnerMetricValue,
                           sizeof (objpimBidirDFElectionWinnerMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionState
*
* @purpose Get 'pimBidirDFElectionState'
*
* @description [pimBidirDFElectionState] The state of this interface with respect to DF-Election for this RP. The states correspond to the ones defined in the BIDIR-PIM specification.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionState (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBidirDFElectionAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionAddressTypeValue;
  fpObjWa_t kwapimBidirDFElectionRPAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionRPAddressValue;
  fpObjWa_t kwapimBidirDFElectionIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBidirDFElectionStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionAddressType */
  kwapimBidirDFElectionAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType,
                   (xLibU8_t *) & keypimBidirDFElectionAddressTypeValue,
                   &kwapimBidirDFElectionAddressType.len);
  if (kwapimBidirDFElectionAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionAddressType);
    return kwapimBidirDFElectionAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionAddressTypeValue,
                           kwapimBidirDFElectionAddressType.len);

  /* retrieve key: pimBidirDFElectionRPAddress */
  kwapimBidirDFElectionRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress,
                   (xLibU8_t *) & keypimBidirDFElectionRPAddressValue,
                   &kwapimBidirDFElectionRPAddress.len);
  if (kwapimBidirDFElectionRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionRPAddress);
    return kwapimBidirDFElectionRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionRPAddressValue,
                           kwapimBidirDFElectionRPAddress.len);

  /* retrieve key: pimBidirDFElectionIfIndex */
  kwapimBidirDFElectionIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex,
                   (xLibU8_t *) & keypimBidirDFElectionIfIndexValue,
                   &kwapimBidirDFElectionIfIndex.len);
  if (kwapimBidirDFElectionIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionIfIndex);
    return kwapimBidirDFElectionIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionIfIndexValue,
                           kwapimBidirDFElectionIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBidirDFElectionAddressTypeValue,
                              keypimBidirDFElectionRPAddressValue,
                              keypimBidirDFElectionIfIndexValue, &objpimBidirDFElectionStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBidirDFElectionState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBidirDFElectionStateValue,
                           sizeof (objpimBidirDFElectionStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionStateTimer
*
* @purpose Get 'pimBidirDFElectionStateTimer'
*
* @description [pimBidirDFElectionStateTimer] The minimum time remaining after which the local router will expire the current DF state represented by pimBidirDFElectionState.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBidirDFElectionTable_pimBidirDFElectionStateTimer (void *wap,
                                                                               void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBidirDFElectionAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionAddressTypeValue;
  fpObjWa_t kwapimBidirDFElectionRPAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionRPAddressValue;
  fpObjWa_t kwapimBidirDFElectionIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBidirDFElectionIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBidirDFElectionStateTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBidirDFElectionAddressType */
  kwapimBidirDFElectionAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionAddressType,
                   (xLibU8_t *) & keypimBidirDFElectionAddressTypeValue,
                   &kwapimBidirDFElectionAddressType.len);
  if (kwapimBidirDFElectionAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionAddressType);
    return kwapimBidirDFElectionAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionAddressTypeValue,
                           kwapimBidirDFElectionAddressType.len);

  /* retrieve key: pimBidirDFElectionRPAddress */
  kwapimBidirDFElectionRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionRPAddress,
                   (xLibU8_t *) & keypimBidirDFElectionRPAddressValue,
                   &kwapimBidirDFElectionRPAddress.len);
  if (kwapimBidirDFElectionRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionRPAddress);
    return kwapimBidirDFElectionRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionRPAddressValue,
                           kwapimBidirDFElectionRPAddress.len);

  /* retrieve key: pimBidirDFElectionIfIndex */
  kwapimBidirDFElectionIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBidirDFElectionTable_pimBidirDFElectionIfIndex,
                   (xLibU8_t *) & keypimBidirDFElectionIfIndexValue,
                   &kwapimBidirDFElectionIfIndex.len);
  if (kwapimBidirDFElectionIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBidirDFElectionIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBidirDFElectionIfIndex);
    return kwapimBidirDFElectionIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBidirDFElectionIfIndexValue,
                           kwapimBidirDFElectionIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBidirDFElectionAddressTypeValue,
                              keypimBidirDFElectionRPAddressValue,
                              keypimBidirDFElectionIfIndexValue,
                              &objpimBidirDFElectionStateTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBidirDFElectionStateTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBidirDFElectionStateTimerValue,
                           sizeof (objpimBidirDFElectionStateTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
