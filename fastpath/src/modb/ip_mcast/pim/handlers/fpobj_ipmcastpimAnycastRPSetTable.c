
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimAnycastRPSetTable.c
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
#include "_xe_ipmcastpimAnycastRPSetTable_obj.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAddressType
*
* @purpose Get 'pimAnycastRPSetAddressType'
*
* @description [pimAnycastRPSetAddressType] The address type of the Anycast-RP address and router address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAddressType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimAnycastRPSetAddressTypeValue;
  xLibU32_t nextObjpimAnycastRPSetAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimAnycastRPSetAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAddressType,
                          (xLibU8_t *) & objpimAnycastRPSetAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjpimAnycastRPSetAddressTypeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimAnycastRPSetAddressTypeValue, owa.len);
    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT, objpimAnycastRPSetAddressTypeValue,
                                    &nextObjpimAnycastRPSetAddressTypeValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimAnycastRPSetAddressTypeValue, owa.len);

  /* return the object value: pimAnycastRPSetAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimAnycastRPSetAddressTypeValue,
                           sizeof (objpimAnycastRPSetAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAnycastAddress
*
* @purpose Get 'pimAnycastRPSetAnycastAddress'
*
* @description [pimAnycastRPSetAnycastAddress] The Anycast-RP address. The InetAddressType is given by the pimAnycastRPSetAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAnycastAddress (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  xLibStr256_t objpimAnycastRPSetAnycastAddressValue;
  xLibStr256_t nextObjpimAnycastRPSetAnycastAddressValue;
  xLibStr256_t objpimAnycastRPSetRouterAddressValue;
  xLibStr256_t nextObjpimAnycastRPSetRouterAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimAnycastRPSetAnycastAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAnycastAddress,
                          (xLibU8_t *) objpimAnycastRPSetAnycastAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objpimAnycastRPSetAnycastAddressValue = objpimAnycastRPSetRouterAddressValue = 0;
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objpimAnycastRPSetAnycastAddressValue,
                                     objpimAnycastRPSetRouterAddressValue,
                                     nextobjpimAnycastRPSetAnycastAddressValue,
                                     nextobjpimAnycastRPSetRouterAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpimAnycastRPSetAnycastAddressValue, owa.len);
    objpimAnycastRPSetRouterAddressValue = 0;
    do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objpimAnycastRPSetAnycastAddressValue,
                                      objpimAnycastRPSetRouterAddressValue,
                                      nextobjpimAnycastRPSetAnycastAddressValue,
                                      nextobjpimAnycastRPSetRouterAddressValue);
    }
    while ((objpimAnycastRPSetAnycastAddressValue == nextObjpimAnycastRPSetAnycastAddressValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpimAnycastRPSetAnycastAddressValue, owa.len);

  /* return the object value: pimAnycastRPSetAnycastAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpimAnycastRPSetAnycastAddressValue,
                           sizeof (objpimAnycastRPSetAnycastAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRouterAddress
*
* @purpose Get 'pimAnycastRPSetRouterAddress'
*
* @description [pimAnycastRPSetRouterAddress] The address of a router that is a member of the Anycast-RP set.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRouterAddress (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  xLibStr256_t objpimAnycastRPSetAnycastAddressValue;
  xLibStr256_t nextObjpimAnycastRPSetAnycastAddressValue;
  xLibStr256_t objpimAnycastRPSetRouterAddressValue;
  xLibStr256_t nextObjpimAnycastRPSetRouterAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimAnycastRPSetAnycastAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAnycastAddress,
                          (xLibU8_t *) objpimAnycastRPSetAnycastAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objpimAnycastRPSetAnycastAddressValue, owa.len);

  /* retrieve key: pimAnycastRPSetRouterAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRouterAddress,
                          (xLibU8_t *) objpimAnycastRPSetRouterAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objpimAnycastRPSetRouterAddressValue = 0;
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     objpimAnycastRPSetAnycastAddressValue,
                                     objpimAnycastRPSetRouterAddressValue,
                                     nextobjpimAnycastRPSetAnycastAddressValue,
                                     nextobjpimAnycastRPSetRouterAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpimAnycastRPSetRouterAddressValue, owa.len);

    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                    objpimAnycastRPSetAnycastAddressValue,
                                    objpimAnycastRPSetRouterAddressValue,
                                    nextobjpimAnycastRPSetAnycastAddressValue,
                                    nextobjpimAnycastRPSetRouterAddressValue);

  }

  if ((objpimAnycastRPSetAnycastAddressValue != nextObjpimAnycastRPSetAnycastAddressValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpimAnycastRPSetRouterAddressValue, owa.len);

  /* return the object value: pimAnycastRPSetRouterAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpimAnycastRPSetRouterAddressValue,
                           sizeof (objpimAnycastRPSetRouterAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetLocalRouter
*
* @purpose Get 'pimAnycastRPSetLocalRouter'
*
* @description [pimAnycastRPSetLocalRouter] Whether this entry corresponds to the local router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetLocalRouter (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimAnycastRPSetAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimAnycastRPSetAddressTypeValue;
  fpObjWa_t kwapimAnycastRPSetAnycastAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetAnycastAddressValue;
  fpObjWa_t kwapimAnycastRPSetRouterAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetRouterAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimAnycastRPSetLocalRouterValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimAnycastRPSetAddressType */
  kwapimAnycastRPSetAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAddressType,
                   (xLibU8_t *) & keypimAnycastRPSetAddressTypeValue,
                   &kwapimAnycastRPSetAddressType.len);
  if (kwapimAnycastRPSetAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAddressType);
    return kwapimAnycastRPSetAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimAnycastRPSetAddressTypeValue,
                           kwapimAnycastRPSetAddressType.len);

  /* retrieve key: pimAnycastRPSetAnycastAddress */
  kwapimAnycastRPSetAnycastAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAnycastAddress,
                   (xLibU8_t *) keypimAnycastRPSetAnycastAddressValue,
                   &kwapimAnycastRPSetAnycastAddress.len);
  if (kwapimAnycastRPSetAnycastAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAnycastAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAnycastAddress);
    return kwapimAnycastRPSetAnycastAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetAnycastAddressValue,
                           kwapimAnycastRPSetAnycastAddress.len);

  /* retrieve key: pimAnycastRPSetRouterAddress */
  kwapimAnycastRPSetRouterAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRouterAddress,
                   (xLibU8_t *) keypimAnycastRPSetRouterAddressValue,
                   &kwapimAnycastRPSetRouterAddress.len);
  if (kwapimAnycastRPSetRouterAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetRouterAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetRouterAddress);
    return kwapimAnycastRPSetRouterAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetRouterAddressValue,
                           kwapimAnycastRPSetRouterAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimAnycastRPSetAddressTypeValue,
                              keypimAnycastRPSetAnycastAddressValue,
                              keypimAnycastRPSetRouterAddressValue,
                              &objpimAnycastRPSetLocalRouterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimAnycastRPSetLocalRouter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimAnycastRPSetLocalRouterValue,
                           sizeof (objpimAnycastRPSetLocalRouterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRowStatus
*
* @purpose Get 'pimAnycastRPSetRowStatus'
*
* @description [pimAnycastRPSetRowStatus] The status of this row.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRowStatus (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimAnycastRPSetAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimAnycastRPSetAddressTypeValue;
  fpObjWa_t kwapimAnycastRPSetAnycastAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetAnycastAddressValue;
  fpObjWa_t kwapimAnycastRPSetRouterAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetRouterAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimAnycastRPSetRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimAnycastRPSetAddressType */
  kwapimAnycastRPSetAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAddressType,
                   (xLibU8_t *) & keypimAnycastRPSetAddressTypeValue,
                   &kwapimAnycastRPSetAddressType.len);
  if (kwapimAnycastRPSetAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAddressType);
    return kwapimAnycastRPSetAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimAnycastRPSetAddressTypeValue,
                           kwapimAnycastRPSetAddressType.len);

  /* retrieve key: pimAnycastRPSetAnycastAddress */
  kwapimAnycastRPSetAnycastAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAnycastAddress,
                   (xLibU8_t *) keypimAnycastRPSetAnycastAddressValue,
                   &kwapimAnycastRPSetAnycastAddress.len);
  if (kwapimAnycastRPSetAnycastAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAnycastAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAnycastAddress);
    return kwapimAnycastRPSetAnycastAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetAnycastAddressValue,
                           kwapimAnycastRPSetAnycastAddress.len);

  /* retrieve key: pimAnycastRPSetRouterAddress */
  kwapimAnycastRPSetRouterAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRouterAddress,
                   (xLibU8_t *) keypimAnycastRPSetRouterAddressValue,
                   &kwapimAnycastRPSetRouterAddress.len);
  if (kwapimAnycastRPSetRouterAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetRouterAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetRouterAddress);
    return kwapimAnycastRPSetRouterAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetRouterAddressValue,
                           kwapimAnycastRPSetRouterAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimAnycastRPSetAddressTypeValue,
                              keypimAnycastRPSetAnycastAddressValue,
                              keypimAnycastRPSetRouterAddressValue,
                              &objpimAnycastRPSetRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimAnycastRPSetRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimAnycastRPSetRowStatusValue,
                           sizeof (objpimAnycastRPSetRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRowStatus
*
* @purpose Set 'pimAnycastRPSetRowStatus'
*
* @description [pimAnycastRPSetRowStatus] The status of this row.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRowStatus (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimAnycastRPSetRowStatusValue;

  fpObjWa_t kwapimAnycastRPSetAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimAnycastRPSetAddressTypeValue;
  fpObjWa_t kwapimAnycastRPSetAnycastAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetAnycastAddressValue;
  fpObjWa_t kwapimAnycastRPSetRouterAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetRouterAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimAnycastRPSetRowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimAnycastRPSetRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimAnycastRPSetRowStatusValue, owa.len);

  /* retrieve key: pimAnycastRPSetAddressType */
  kwapimAnycastRPSetAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAddressType,
                   (xLibU8_t *) & keypimAnycastRPSetAddressTypeValue,
                   &kwapimAnycastRPSetAddressType.len);
  if (kwapimAnycastRPSetAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAddressType);
    return kwapimAnycastRPSetAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimAnycastRPSetAddressTypeValue,
                           kwapimAnycastRPSetAddressType.len);

  /* retrieve key: pimAnycastRPSetAnycastAddress */
  kwapimAnycastRPSetAnycastAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAnycastAddress,
                   (xLibU8_t *) keypimAnycastRPSetAnycastAddressValue,
                   &kwapimAnycastRPSetAnycastAddress.len);
  if (kwapimAnycastRPSetAnycastAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAnycastAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAnycastAddress);
    return kwapimAnycastRPSetAnycastAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetAnycastAddressValue,
                           kwapimAnycastRPSetAnycastAddress.len);

  /* retrieve key: pimAnycastRPSetRouterAddress */
  kwapimAnycastRPSetRouterAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRouterAddress,
                   (xLibU8_t *) keypimAnycastRPSetRouterAddressValue,
                   &kwapimAnycastRPSetRouterAddress.len);
  if (kwapimAnycastRPSetRouterAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetRouterAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetRouterAddress);
    return kwapimAnycastRPSetRouterAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetRouterAddressValue,
                           kwapimAnycastRPSetRouterAddress.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimAnycastRPSetAddressTypeValue,
                              keypimAnycastRPSetAnycastAddressValue,
                              keypimAnycastRPSetRouterAddressValue,
                              objpimAnycastRPSetRowStatusValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetStorageType
*
* @purpose Get 'pimAnycastRPSetStorageType'
*
* @description [pimAnycastRPSetStorageType] The storage type for this row. Rows having the value 'permanent' need not allow write-access to any columnar objects in the row.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimAnycastRPSetAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimAnycastRPSetAddressTypeValue;
  fpObjWa_t kwapimAnycastRPSetAnycastAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetAnycastAddressValue;
  fpObjWa_t kwapimAnycastRPSetRouterAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetRouterAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimAnycastRPSetStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimAnycastRPSetAddressType */
  kwapimAnycastRPSetAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAddressType,
                   (xLibU8_t *) & keypimAnycastRPSetAddressTypeValue,
                   &kwapimAnycastRPSetAddressType.len);
  if (kwapimAnycastRPSetAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAddressType);
    return kwapimAnycastRPSetAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimAnycastRPSetAddressTypeValue,
                           kwapimAnycastRPSetAddressType.len);

  /* retrieve key: pimAnycastRPSetAnycastAddress */
  kwapimAnycastRPSetAnycastAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAnycastAddress,
                   (xLibU8_t *) keypimAnycastRPSetAnycastAddressValue,
                   &kwapimAnycastRPSetAnycastAddress.len);
  if (kwapimAnycastRPSetAnycastAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAnycastAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAnycastAddress);
    return kwapimAnycastRPSetAnycastAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetAnycastAddressValue,
                           kwapimAnycastRPSetAnycastAddress.len);

  /* retrieve key: pimAnycastRPSetRouterAddress */
  kwapimAnycastRPSetRouterAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRouterAddress,
                   (xLibU8_t *) keypimAnycastRPSetRouterAddressValue,
                   &kwapimAnycastRPSetRouterAddress.len);
  if (kwapimAnycastRPSetRouterAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetRouterAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetRouterAddress);
    return kwapimAnycastRPSetRouterAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetRouterAddressValue,
                           kwapimAnycastRPSetRouterAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimAnycastRPSetAddressTypeValue,
                              keypimAnycastRPSetAnycastAddressValue,
                              keypimAnycastRPSetRouterAddressValue,
                              &objpimAnycastRPSetStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimAnycastRPSetStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimAnycastRPSetStorageTypeValue,
                           sizeof (objpimAnycastRPSetStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetStorageType
*
* @purpose Set 'pimAnycastRPSetStorageType'
*
* @description [pimAnycastRPSetStorageType] The storage type for this row. Rows having the value 'permanent' need not allow write-access to any columnar objects in the row.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimAnycastRPSetTable_pimAnycastRPSetStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimAnycastRPSetStorageTypeValue;

  fpObjWa_t kwapimAnycastRPSetAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimAnycastRPSetAddressTypeValue;
  fpObjWa_t kwapimAnycastRPSetAnycastAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetAnycastAddressValue;
  fpObjWa_t kwapimAnycastRPSetRouterAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimAnycastRPSetRouterAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimAnycastRPSetStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimAnycastRPSetStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimAnycastRPSetStorageTypeValue, owa.len);

  /* retrieve key: pimAnycastRPSetAddressType */
  kwapimAnycastRPSetAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAddressType,
                   (xLibU8_t *) & keypimAnycastRPSetAddressTypeValue,
                   &kwapimAnycastRPSetAddressType.len);
  if (kwapimAnycastRPSetAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAddressType);
    return kwapimAnycastRPSetAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimAnycastRPSetAddressTypeValue,
                           kwapimAnycastRPSetAddressType.len);

  /* retrieve key: pimAnycastRPSetAnycastAddress */
  kwapimAnycastRPSetAnycastAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetAnycastAddress,
                   (xLibU8_t *) keypimAnycastRPSetAnycastAddressValue,
                   &kwapimAnycastRPSetAnycastAddress.len);
  if (kwapimAnycastRPSetAnycastAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetAnycastAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetAnycastAddress);
    return kwapimAnycastRPSetAnycastAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetAnycastAddressValue,
                           kwapimAnycastRPSetAnycastAddress.len);

  /* retrieve key: pimAnycastRPSetRouterAddress */
  kwapimAnycastRPSetRouterAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimAnycastRPSetTable_pimAnycastRPSetRouterAddress,
                   (xLibU8_t *) keypimAnycastRPSetRouterAddressValue,
                   &kwapimAnycastRPSetRouterAddress.len);
  if (kwapimAnycastRPSetRouterAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimAnycastRPSetRouterAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimAnycastRPSetRouterAddress);
    return kwapimAnycastRPSetRouterAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimAnycastRPSetRouterAddressValue,
                           kwapimAnycastRPSetRouterAddress.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimAnycastRPSetAddressTypeValue,
                              keypimAnycastRPSetAnycastAddressValue,
                              keypimAnycastRPSetRouterAddressValue,
                              objpimAnycastRPSetStorageTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
