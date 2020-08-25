
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimStarGTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  11 May 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimStarGTable_obj.h"
#include "usmdb_mib_pimsm_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGAddressType
*
* @purpose Get 'pimStarGAddressType'
*
* @description [pimStarGAddressType] The address type of this multicast group.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGAddressTypeValue;
  xLibU32_t nextObjpimStarGAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                          (xLibU8_t *) & objpimStarGAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimStarGAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStarGAddressTypeValue, owa.len);

    if (objpimStarGAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
    {
      nextObjpimStarGAddressTypeValue = L7_INET_ADDR_TYPE_IPV6;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      owa.l7rc = L7_FAILURE;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStarGAddressTypeValue, owa.len);

  /* return the object value: pimStarGAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimStarGAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGGrpAddress
*
* @purpose Get 'pimStarGGrpAddress'
*
* @description [pimStarGGrpAddress] The multicast group address. The InetAddressType is given by the pimStarGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGGrpAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStarGGrpAddressValue;
  L7_inet_addr_t nextObjpimStarGGrpAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                          (xLibU8_t *) &objpimStarGGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memset(&nextObjpimStarGGrpAddressValue, 0, sizeof(L7_inet_addr_t));
    owa.l7rc = usmDbPimsmStarGEntryNextGet (keypimStarGAddressTypeValue,&nextObjpimStarGGrpAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStarGGrpAddressValue, owa.len);

    memcpy(&nextObjpimStarGGrpAddressValue, &objpimStarGGrpAddressValue, sizeof(L7_inet_addr_t));
    
    owa.l7rc = usmDbPimsmStarGEntryNextGet (keypimStarGAddressTypeValue,&nextObjpimStarGGrpAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStarGGrpAddressValue, owa.len);

  /* return the object value: pimStarGGrpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimStarGGrpAddressValue,
                           sizeof (objpimStarGGrpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGUpTime
*
* @purpose Get 'pimStarGUpTime'
*
* @description [pimStarGUpTime] The time since this entry was created by the local router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGUpTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGUpTimeGet (keypimStarGAddressTypeValue,
                                                                &keypimStarGGrpAddressValue, &objpimStarGUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGUpTimeValue,
                           sizeof (objpimStarGUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGPimMode
*
* @purpose Get 'pimStarGPimMode'
*
* @description [pimStarGPimMode] Whether this entry represents an ASM (Any Source Multicast, used with PIM-SM) or BIDIR-PIM group.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGPimMode (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGPimModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGPimModeGet (keypimStarGAddressTypeValue,
                                                                  &keypimStarGGrpAddressValue, &objpimStarGPimModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGPimMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGPimModeValue,
                           sizeof (objpimStarGPimModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPAddressType
*
* @purpose Get 'pimStarGRPAddressType'
*
* @description [pimStarGRPAddressType] The address type of the Rendezvous Point (RP), or unknown(0) if the RP address is unknown.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPAddressType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGRPAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimStarGAddressTypeValue,
                              keypimStarGGrpAddressValue, &objpimStarGRPAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGRPAddressTypeValue,
                           sizeof (objpimStarGRPAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPAddress
*
* @purpose Get 'pimStarGRPAddress'
*
* @description [pimStarGRPAddress] The address type of the Rendezvous Point (RP), or unknown(0) if the RP address is unknown.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStarGRPAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPAddrGet (keypimStarGAddressTypeValue,
                                                                &keypimStarGGrpAddressValue, &objpimStarGRPAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimStarGRPAddressValue,
                           sizeof (objpimStarGRPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGPimModeOrigin
*
* @purpose Get 'pimStarGPimModeOrigin'
*
* @description [pimStarGPimModeOrigin] The mechanism by which the PIM mode and RP for the group were learned.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGPimModeOrigin (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGPimModeOriginValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPOriginGet (keypimStarGAddressTypeValue,
                                                                  &keypimStarGGrpAddressValue, &objpimStarGPimModeOriginValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGPimModeOrigin */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGPimModeOriginValue,
                           sizeof (objpimStarGPimModeOriginValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPIsLocal
*
* @purpose Get 'pimStarGRPIsLocal'
*
* @description [pimStarGRPIsLocal] Whether the local router is the RP for the group.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPIsLocal (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGRPIsLocalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPIsLocalGet (keypimStarGAddressTypeValue,
                                                                  &keypimStarGGrpAddressValue, &objpimStarGRPIsLocalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPIsLocal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGRPIsLocalValue,
                           sizeof (objpimStarGRPIsLocalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGUpstreamJoinState
*
* @purpose Get 'pimStarGUpstreamJoinState'
*
* @description [pimStarGUpstreamJoinState] Whether the local router should join the RP tree for the group. This corresponds to the state of the upstream (*,G) state machine in the PIM-SM specification.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGUpstreamJoinState (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGUpstreamJoinStateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGUpstreamJoinStateGet (keypimStarGAddressTypeValue,
                                                                                &keypimStarGGrpAddressValue, &objpimStarGUpstreamJoinStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGUpstreamJoinState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGUpstreamJoinStateValue,
                           sizeof (objpimStarGUpstreamJoinStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGUpstreamJoinTimer
*
* @purpose Get 'pimStarGUpstreamJoinTimer'
*
* @description [pimStarGUpstreamJoinTimer] The time remaining before the local router next sends a periodic (*,G) Join message on pimStarGRPFIfIndex. This timer is called the (*,G) Upstream Join Timer in the PIM-SM specification. This object is zero if the timer is not running.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGUpstreamJoinTimer (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGUpstreamJoinTimerValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGUpstreamJoinTimerGet (keypimStarGAddressTypeValue,
                                                                                &keypimStarGGrpAddressValue, &objpimStarGUpstreamJoinTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGUpstreamJoinTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGUpstreamJoinTimerValue,
                           sizeof (objpimStarGUpstreamJoinTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGUpstreamNeighborType
*
* @purpose Get 'pimStarGUpstreamNeighborType'
*
* @description [pimStarGUpstreamNeighborType] The primary address type of the upstream neighbor, or unknown(0) if the upstream neighbor address is unknown or is not a PIM neighbor.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGUpstreamNeighborType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGUpstreamNeighborTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimStarGAddressTypeValue,
                              keypimStarGGrpAddressValue, &objpimStarGUpstreamNeighborTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGUpstreamNeighborType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGUpstreamNeighborTypeValue,
                           sizeof (objpimStarGUpstreamNeighborTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGUpstreamNeighbor
*
* @purpose Get 'pimStarGUpstreamNeighbor'
*
* @description [pimStarGUpstreamNeighbor] The primary address of the neighbor on pimStarGRPFIfIndex that the local router is sending periodic (*,G) Join messages to. The InetAddressType is given by the pimStarGUpstreamNeighborType object. This address is called RPF'(*,G) in the PIM-SM specification.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGUpstreamNeighbor (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStarGUpstreamNeighborValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGUpstreamNbrGet (keypimStarGAddressTypeValue,
                                                                        &keypimStarGGrpAddressValue, &objpimStarGUpstreamNeighborValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGUpstreamNeighbor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimStarGUpstreamNeighborValue,
                           sizeof (objpimStarGUpstreamNeighborValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPFIfIndex
*
* @purpose Get 'pimStarGRPFIfIndex'
*
* @description [pimStarGRPFIfIndex] The value of ifIndex for the Reverse Path Forwarding (RPF) interface towards the RP, or zero if the RPF interface is unknown.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPFIfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGRPFIfIndexValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPFIfIndexGet (keypimStarGAddressTypeValue,
                                                                     &keypimStarGGrpAddressValue, &objpimStarGRPFIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPFIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGRPFIfIndexValue,
                           sizeof (objpimStarGRPFIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPFNextHopType
*
* @purpose Get 'pimStarGRPFNextHopType'
*
* @description [pimStarGRPFNextHopType] The address type of the RPF next hop towards the RP, or unknown(0) if the RPF next hop is unknown.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPFNextHopType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGRPFNextHopTypeValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimStarGAddressTypeValue,
                              keypimStarGGrpAddressValue, &objpimStarGRPFNextHopTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPFNextHopType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGRPFNextHopTypeValue,
                           sizeof (objpimStarGRPFNextHopTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPFNextHop
*
* @purpose Get 'pimStarGRPFNextHop'
*
* @description [pimStarGRPFNextHop] The address of the RPF next hop towards the RP. The InetAddressType is given by the pimStarGRPFNextHopType object. This address is called MRIB.next_hop(RP(G)) in the PIM-SM specification.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPFNextHop (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStarGRPFNextHopValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPFNextHopGet (keypimStarGAddressTypeValue,
                                                                      &keypimStarGGrpAddressValue, &objpimStarGRPFNextHopValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPFNextHop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)& objpimStarGRPFNextHopValue,
                           sizeof (objpimStarGRPFNextHopValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPFRouteProtocol
*
* @purpose Get 'pimStarGRPFRouteProtocol'
*
* @description [pimStarGRPFRouteProtocol] The routing mechanism via which the route used to find the RPF interface towards the RP was learned.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPFRouteProtocol (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGRPFRouteProtocolValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPFRouteProtocolGet (keypimStarGAddressTypeValue,
                                                                              &keypimStarGGrpAddressValue, &objpimStarGRPFRouteProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPFRouteProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGRPFRouteProtocolValue,
                           sizeof (objpimStarGRPFRouteProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPFRouteAddress
*
* @purpose Get 'pimStarGRPFRouteAddress'
*
* @description [pimStarGRPFRouteAddress] The IP address that, when combined with the corresponding value of pimStarGRPFRoutePrefixLength, identifies the route used to find the RPF interface towards the RP. The InetAddressType is given by the pimStarGRPFNextHopType object. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPFRouteAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStarGRPFRouteAddressValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPFRouteAddrGet (keypimStarGAddressTypeValue,
                                                                         &keypimStarGGrpAddressValue, &objpimStarGRPFRouteAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPFRouteAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimStarGRPFRouteAddressValue,
                           sizeof (objpimStarGRPFRouteAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPFRoutePrefixLength
*
* @purpose Get 'pimStarGRPFRoutePrefixLength'
*
* @description [pimStarGRPFRoutePrefixLength] The prefix length that, when combined with the corresponding value of pimStarGRPFRouteAddress, identifies the route used to find the RPF interface towards the RP. The InetAddressType is given by the pimStarGRPFNextHopType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPFRoutePrefixLength (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGRPFRoutePrefixLengthValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPFRoutePrefixLenGet (keypimStarGAddressTypeValue,
                                                                                &keypimStarGGrpAddressValue, &objpimStarGRPFRoutePrefixLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPFRoutePrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGRPFRoutePrefixLengthValue,
                           sizeof (objpimStarGRPFRoutePrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPFRouteMetricPref
*
* @purpose Get 'pimStarGRPFRouteMetricPref'
*
* @description [pimStarGRPFRouteMetricPref] The metric preference of the route used to find the RPF interface towards the RP.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPFRouteMetricPref (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGRPFRouteMetricPrefValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPFRouteMetricPrefGet (keypimStarGAddressTypeValue,
                                                                                &keypimStarGGrpAddressValue, &objpimStarGRPFRouteMetricPrefValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPFRouteMetricPref */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGRPFRouteMetricPrefValue,
                           sizeof (objpimStarGRPFRouteMetricPrefValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStarGTable_pimStarGRPFRouteMetric
*
* @purpose Get 'pimStarGRPFRouteMetric'
*
* @description [pimStarGRPFRouteMetric] The routing metric of the route used to find the RPF interface towards the RP.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStarGTable_pimStarGRPFRouteMetric (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGRPFRouteMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStarGTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStarGRPFRouteMetricGet (keypimStarGAddressTypeValue,
                                                                           &keypimStarGGrpAddressValue, &objpimStarGRPFRouteMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStarGRPFRouteMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGRPFRouteMetricValue,
                           sizeof (objpimStarGRPFRouteMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
