
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimNeighborTable.c
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
#include "_xe_ipmcastpimNeighborTable_obj.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"
/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborIfIndex
*
* @purpose Get 'pimNeighborIfIndex'
*
* @description [pimNeighborIfIndex] The value of ifIndex for the interface used to reach this PIM neighbor.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborIfIndex (void *wap, void *bufp)
{

  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;

  xLibU32_t objpimNeighborIfIndexValue;
  xLibU32_t nextObjpimNeighborIfIndexValue;
  
  L7_inet_addr_t nextObjpimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                          (xLibU8_t *) & objpimNeighborIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objpimNeighborIfIndexValue = 0;
    nextObjpimNeighborIfIndexValue = 0;
    inetAddressZeroSet(keypimNeighborAddressTypeValue, &nextObjpimNeighborAddressValue);
    
    owa.l7rc = usmDbPimNeighborEntryNextGet (L7_UNIT_CURRENT, keypimNeighborAddressTypeValue,
                                                                         &nextObjpimNeighborIfIndexValue, &nextObjpimNeighborAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimNeighborIfIndexValue, owa.len);

    nextObjpimNeighborIfIndexValue = objpimNeighborIfIndexValue;
    inetAddressZeroSet(keypimNeighborAddressTypeValue, &nextObjpimNeighborAddressValue);

    do
    {
      owa.l7rc = usmDbPimNeighborEntryNextGet (L7_UNIT_CURRENT, keypimNeighborAddressTypeValue,
                                                                           &nextObjpimNeighborIfIndexValue, &nextObjpimNeighborAddressValue);
    }
    while ((objpimNeighborIfIndexValue == nextObjpimNeighborIfIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimNeighborIfIndexValue, owa.len);

  /* return the object value: pimNeighborIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimNeighborIfIndexValue,
                           sizeof (objpimNeighborIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborAddressType
*
* @purpose Get 'pimNeighborAddressType'
*
* @description [pimNeighborAddressType] The address type of this PIM neighbor.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborAddressTypeValue;
  xLibU32_t nextObjpimNeighborAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                          (xLibU8_t *) & objpimNeighborAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimNeighborAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimNeighborAddressTypeValue, owa.len);

    if (objpimNeighborAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimNeighborAddressTypeValue = objpimNeighborAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimNeighborAddressTypeValue, owa.len);

  /* return the object value: pimNeighborAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimNeighborAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborAddress
*
* @purpose Get 'pimNeighborAddress'
*
* @description [pimNeighborAddress] The address type of this PIM neighbor
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;

  xLibU32_t objpimNeighborIfIndexValue;
  xLibU32_t nextObjpimNeighborIfIndexValue=0;

  L7_inet_addr_t objpimNeighborAddressValue;
  L7_inet_addr_t nextObjpimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                          (xLibU8_t *) & objpimNeighborIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimNeighborIfIndexValue, owa.len);
  nextObjpimNeighborIfIndexValue = objpimNeighborIfIndexValue;
  owa.len = sizeof(L7_inet_addr_t);

  /* retrieve key: pimNeighborAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                          (xLibU8_t *) &objpimNeighborAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(keypimNeighborAddressTypeValue, &nextObjpimNeighborAddressValue);

    owa.l7rc = usmDbPimNeighborEntryNextGet (L7_UNIT_CURRENT, keypimNeighborAddressTypeValue,
                                                                         &nextObjpimNeighborIfIndexValue, &nextObjpimNeighborAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimNeighborAddressValue, owa.len);
    memcpy(&nextObjpimNeighborAddressValue, &objpimNeighborAddressValue, sizeof(L7_inet_addr_t));
    
    owa.l7rc = usmDbPimNeighborEntryNextGet (L7_UNIT_CURRENT, keypimNeighborAddressTypeValue,
                                                                         &nextObjpimNeighborIfIndexValue, &nextObjpimNeighborAddressValue);
  }

  if ((objpimNeighborIfIndexValue != nextObjpimNeighborIfIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimNeighborAddressValue, owa.len);

  /* return the object value: pimNeighborAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimNeighborAddressValue,
                           sizeof (objpimNeighborAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborGenerationIDPresent
*
* @purpose Get 'pimNeighborGenerationIDPresent'
*
* @description [pimNeighborGenerationIDPresent] Evaluates to TRUE if this neighbor is using the Generation ID option.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborGenerationIDPresent (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborGenerationIDPresentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborGenerationIDPresentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborGenerationIDPresent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborGenerationIDPresentValue,
                           sizeof (objpimNeighborGenerationIDPresentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborGenerationIDValue
*
* @purpose Get 'pimNeighborGenerationIDValue'
*
* @description [pimNeighborGenerationIDValue] The value of the Generation ID from the last PIM Hello message received from this neighbor. This object is always zero if pimNeighborGenerationIDPresent is FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborGenerationIDValue (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborGenerationIDValueValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborGenerationIDValueValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborGenerationIDValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborGenerationIDValueValue,
                           sizeof (objpimNeighborGenerationIDValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborUpTime
*
* @purpose Get 'pimNeighborUpTime'
*
* @description [pimNeighborUpTime] The time since this PIM neighbor (last) became a neighbor of the local router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborUpTime (void *wap, void *bufp)
{
  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;

  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;

  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborUpTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) &keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimNeighborUpTimeGet (L7_UNIT_CURRENT, keypimNeighborAddressTypeValue,
                                                                    keypimNeighborIfIndexValue, &keypimNeighborAddressValue, 
                                                                    &objpimNeighborUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborUpTimeValue,
                           sizeof (objpimNeighborUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborExpiryTime
*
* @purpose Get 'pimNeighborExpiryTime'
*
* @description [pimNeighborExpiryTime] The minimum time remaining before this PIM neighbor will time out. The value zero indicates that this PIM neighbor will never time out.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborExpiryTime (void *wap, void *bufp)
{
  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;

  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;

  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborExpiryTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) &keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimNeighborExpiryTimeGet (L7_UNIT_CURRENT, keypimNeighborAddressTypeValue,
                                                                    keypimNeighborIfIndexValue, &keypimNeighborAddressValue, 
                                                                    &objpimNeighborExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborExpiryTimeValue,
                           sizeof (objpimNeighborExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborDRPriorityPresent
*
* @purpose Get 'pimNeighborDRPriorityPresent'
*
* @description [pimNeighborDRPriorityPresent] Evaluates to TRUE if this neighbor is using the DR Priority option.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborDRPriorityPresent (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborDRPriorityPresentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborDRPriorityPresentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborDRPriorityPresent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborDRPriorityPresentValue,
                           sizeof (objpimNeighborDRPriorityPresentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborDRPriority
*
* @purpose Get 'pimNeighborDRPriority'
*
* @description [pimNeighborDRPriority] The value of the Designated Router Priority from the last PIM Hello message received from this neighbor. This object is always zero if pimNeighborDRPriorityPresent is FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborDRPriority (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborDRPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborDRPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborDRPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborDRPriorityValue,
                           sizeof (objpimNeighborDRPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborLanPruneDelayPresent
*
* @purpose Get 'pimNeighborLanPruneDelayPresent'
*
* @description [pimNeighborLanPruneDelayPresent] Evaluates to TRUE if this neighbor is using the LAN Prune Delay option.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborLanPruneDelayPresent (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborLanPruneDelayPresentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborLanPruneDelayPresentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborLanPruneDelayPresent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborLanPruneDelayPresentValue,
                           sizeof (objpimNeighborLanPruneDelayPresentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborTBit
*
* @purpose Get 'pimNeighborTBit'
*
* @description [pimNeighborTBit] Whether the T bit was set in the LAN Prune Delay option received from this neighbor. The T bit specifies the ability of the neighbor to disable join suppression. This object is always TRUE if pimNeighborLanPruneDelayPresent is FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborTBit (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborTBitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborTBitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborTBit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborTBitValue,
                           sizeof (objpimNeighborTBitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborPropagationDelay
*
* @purpose Get 'pimNeighborPropagationDelay'
*
* @description [pimNeighborPropagationDelay] The value of the Propagation_Delay field of the LAN Prune Delay option received from this neighbor. This object is always zero if pimNeighborLanPruneDelayPresent is FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborPropagationDelay (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborPropagationDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborPropagationDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborPropagationDelay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborPropagationDelayValue,
                           sizeof (objpimNeighborPropagationDelayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborOverrideInterval
*
* @purpose Get 'pimNeighborOverrideInterval'
*
* @description [pimNeighborOverrideInterval] The value of the Override_Interval field of the LAN Prune Delay option received from this neighbor. This object is always zero if pimNeighborLanPruneDelayPresent is FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborOverrideInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborOverrideIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborOverrideIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborOverrideInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborOverrideIntervalValue,
                           sizeof (objpimNeighborOverrideIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborBidirCapable
*
* @purpose Get 'pimNeighborBidirCapable'
*
* @description [pimNeighborBidirCapable] Evaluates to TRUE if this neighbor is using the Bidirectional-PIM Capable option.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborBidirCapable (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborBidirCapableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborBidirCapableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborBidirCapable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborBidirCapableValue,
                           sizeof (objpimNeighborBidirCapableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNeighborTable_pimNeighborSRCapable
*
* @purpose Get 'pimNeighborSRCapable'
*
* @description [pimNeighborSRCapable] Evaluates to TRUE if this neighbor is using the State Refresh Capable option. This object is used only by PIM-DM.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNeighborTable_pimNeighborSRCapable (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborIfIndexValue;
  fpObjWa_t kwapimNeighborAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimNeighborAddressTypeValue;
  fpObjWa_t kwapimNeighborAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNeighborSRCapableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNeighborIfIndex */
  kwapimNeighborIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborIfIndex,
                                            (xLibU8_t *) & keypimNeighborIfIndexValue,
                                            &kwapimNeighborIfIndex.len);
  if (kwapimNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborIfIndex);
    return kwapimNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborIfIndexValue, kwapimNeighborIfIndex.len);

  /* retrieve key: pimNeighborAddressType */
  kwapimNeighborAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddressType,
                   (xLibU8_t *) & keypimNeighborAddressTypeValue, &kwapimNeighborAddressType.len);
  if (kwapimNeighborAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddressType);
    return kwapimNeighborAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimNeighborAddressTypeValue, kwapimNeighborAddressType.len);

  /* retrieve key: pimNeighborAddress */
  kwapimNeighborAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNeighborTable_pimNeighborAddress,
                                            (xLibU8_t *) keypimNeighborAddressValue,
                                            &kwapimNeighborAddress.len);
  if (kwapimNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimNeighborAddress);
    return kwapimNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimNeighborAddressValue, kwapimNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimNeighborIfIndexValue,
                              keypimNeighborAddressTypeValue,
                              keypimNeighborAddressValue, &objpimNeighborSRCapableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimNeighborSRCapable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimNeighborSRCapableValue,
                           sizeof (objpimNeighborSRCapableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
