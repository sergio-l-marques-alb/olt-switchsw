
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimSGRptITable.c
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
#include "_xe_ipmcastpimSGRptITable_obj.h"
#include "usmdb_mib_pimsm_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptITable_pimStarGAddressType
*
* @purpose Get 'pimStarGAddressType'
*
* @description [pimStarGAddressType] The address type of this multicast group.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptITable_pimStarGAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGAddressTypeValue;
  xLibU32_t nextObjpimStarGAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGAddressType,
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

    if (objpimStarGAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimStarGAddressTypeValue = objpimStarGAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStarGAddressTypeValue, owa.len);

  /* return the object value: pimStaticRPAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimStarGAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptITable_pimStarGGrpAddress
*
* @purpose Get 'pimStarGGrpAddress'
*
* @description [pimStarGGrpAddress] The multicast group address. The InetAddressType is given by the pimStarGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptITable_pimStarGGrpAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStarGGrpAddressValue;
  L7_inet_addr_t nextObjpimStarGGrpAddressValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGAddressType,
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
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGGrpAddress,
                          (xLibU8_t *) &objpimStarGGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&nextObjpimStarGGrpAddressValue, 0, sizeof(L7_inet_addr_t));    
    
    owa.l7rc = usmDbPimsmStarGEntryNextGet (keypimStarGAddressTypeValue,
                                                                      &nextObjpimStarGGrpAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStarGGrpAddressValue, owa.len);

    memcpy(&nextObjpimStarGGrpAddressValue, &objpimStarGGrpAddressValue, sizeof(L7_inet_addr_t));
    owa.l7rc = usmDbPimsmStarGEntryNextGet (keypimStarGAddressTypeValue,
                                                                      &nextObjpimStarGGrpAddressValue);
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
* @function fpObjGet_ipmcastpimSGRptITable_pimSGRptSrcAddress
*
* @purpose Get 'pimSGRptSrcAddress'
*
* @description [pimSGRptSrcAddress] The source address for this entry. The InetAddressType is given by the pimStarGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptITable_pimSGRptSrcAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;
  
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  L7_inet_addr_t objpimSGRptSrcAddressValue;
  L7_inet_addr_t nextObjpimSGRptSrcAddressValue;
  
  xLibU32_t nextObjpimSGRptIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimSGRptSrcAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptSrcAddress,
                          (xLibU8_t *) &objpimSGRptSrcAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjpimSGRptIIfIndexValue = 0;
    memset(&nextObjpimSGRptSrcAddressValue, 0, sizeof(L7_inet_addr_t));

    
    owa.l7rc = usmDbPimsmSGRptIEntryNextGet (keypimStarGAddressTypeValue,
                                                                       &keypimStarGGrpAddressValue,
                                                                       &nextObjpimSGRptSrcAddressValue,
                                                                       &nextObjpimSGRptIIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGRptSrcAddressValue, owa.len);

    nextObjpimSGRptIIfIndexValue = 0;
    memcpy(&nextObjpimSGRptSrcAddressValue, &objpimSGRptSrcAddressValue, sizeof(L7_inet_addr_t));
    
    do
    {
      owa.l7rc = usmDbPimsmSGRptIEntryNextGet (keypimStarGAddressTypeValue,
                                                                         &keypimStarGGrpAddressValue,
                                                                         &nextObjpimSGRptSrcAddressValue,
                                                                         &nextObjpimSGRptIIfIndexValue);
    }
    while (!memcmp(&objpimSGRptSrcAddressValue, &nextObjpimSGRptSrcAddressValue, sizeof(L7_inet_addr_t))
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGRptSrcAddressValue, owa.len);

  /* return the object value: pimSGRptSrcAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimSGRptSrcAddressValue,
                           sizeof (objpimSGRptSrcAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptITable_pimSGRptIIfIndex
*
* @purpose Get 'pimSGRptIIfIndex'
*
* @description [pimSGRptIIfIndex] The ifIndex of the interface that this entry corresponds to.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptITable_pimSGRptIIfIndex (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;
  
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  L7_inet_addr_t objpimSGRptSrcAddressValue;
  L7_inet_addr_t nextObjpimSGRptSrcAddressValue;

  xLibU32_t objpimSGRptIIfIndexValue;
  xLibU32_t nextObjpimSGRptIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimSGRptSrcAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptSrcAddress,
                          (xLibU8_t *) &objpimSGRptSrcAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGRptSrcAddressValue, owa.len);

  /* retrieve key: pimSGRptIIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptIIfIndex,
                          (xLibU8_t *) & objpimSGRptIIfIndexValue, &owa.len);

  memcpy(&nextObjpimSGRptSrcAddressValue, &objpimSGRptSrcAddressValue, sizeof(L7_inet_addr_t));

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimSGRptIIfIndexValue = 0;
    
    owa.l7rc = usmDbPimsmSGRptIEntryNextGet (keypimStarGAddressTypeValue,
                                                                       &keypimStarGGrpAddressValue,
                                                                       &nextObjpimSGRptSrcAddressValue,
                                                                       &nextObjpimSGRptIIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGRptIIfIndexValue, owa.len);
    nextObjpimSGRptIIfIndexValue = objpimSGRptIIfIndexValue;

    owa.l7rc = usmDbPimsmSGRptIEntryNextGet (keypimStarGAddressTypeValue,
                                                                       &keypimStarGGrpAddressValue,
                                                                       &nextObjpimSGRptSrcAddressValue,
                                                                       &nextObjpimSGRptIIfIndexValue);
  }

  if (memcmp(&objpimSGRptSrcAddressValue, &nextObjpimSGRptSrcAddressValue,sizeof(L7_inet_addr_t)) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGRptIIfIndexValue, owa.len);

  /* return the object value: pimSGRptIIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimSGRptIIfIndexValue,
                           sizeof (objpimSGRptIIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptITable_pimSGRptIUpTime
*
* @purpose Get 'pimSGRptIUpTime'
*
* @description [pimSGRptIUpTime] The time since this entry was created by the local router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptITable_pimSGRptIUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimSGRptSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGRptSrcAddressValue;

  fpObjWa_t kwapimSGRptIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGRptIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptIUpTimeValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGAddressType,
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
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimSGRptSrcAddress */
  kwapimSGRptSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptSrcAddress,
                                            (xLibU8_t *) &keypimSGRptSrcAddressValue,
                                            &kwapimSGRptSrcAddress.len);
  if (kwapimSGRptSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptSrcAddress);
    return kwapimSGRptSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptSrcAddressValue, kwapimSGRptSrcAddress.len);

  /* retrieve key: pimSGRptIIfIndex */
  kwapimSGRptIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptIIfIndex,
                                          (xLibU8_t *) & keypimSGRptIIfIndexValue,
                                          &kwapimSGRptIIfIndex.len);
  if (kwapimSGRptIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptIIfIndex);
    return kwapimSGRptIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptIIfIndexValue, kwapimSGRptIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGRptIUpTimeGet (keypimStarGAddressTypeValue,
                                                                  &keypimStarGGrpAddressValue,
                                                                  &keypimSGRptSrcAddressValue,
                                                                  keypimSGRptIIfIndexValue, &objpimSGRptIUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRptIUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptIUpTimeValue,
                           sizeof (objpimSGRptIUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptITable_pimSGRptILocalMembership
*
* @purpose Get 'pimSGRptILocalMembership'
*
* @description [pimSGRptILocalMembership] Whether the local router has both (*,G) include local membership and (S,G) exclude local membership on this interface (resulting from a mechanism such as IGMP or MLD). This corresponds to local_receiver_exclude(S,G,I) in the PIM-SM specification.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptITable_pimSGRptILocalMembership (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimSGRptSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGRptSrcAddressValue;

  fpObjWa_t kwapimSGRptIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGRptIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptILocalMembershipValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGAddressType,
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
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimSGRptSrcAddress */
  kwapimSGRptSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptSrcAddress,
                                            (xLibU8_t *) &keypimSGRptSrcAddressValue,
                                            &kwapimSGRptSrcAddress.len);
  if (kwapimSGRptSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptSrcAddress);
    return kwapimSGRptSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptSrcAddressValue, kwapimSGRptSrcAddress.len);

  /* retrieve key: pimSGRptIIfIndex */
  kwapimSGRptIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptIIfIndex,
                                          (xLibU8_t *) & keypimSGRptIIfIndexValue,
                                          &kwapimSGRptIIfIndex.len);
  if (kwapimSGRptIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptIIfIndex);
    return kwapimSGRptIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptIIfIndexValue, kwapimSGRptIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGRptILocalMembershipGet (keypimStarGAddressTypeValue,
                                                                                &keypimStarGGrpAddressValue,
                                                                                &keypimSGRptSrcAddressValue,
                                                                                keypimSGRptIIfIndexValue, &objpimSGRptILocalMembershipValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRptILocalMembership */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptILocalMembershipValue,
                           sizeof (objpimSGRptILocalMembershipValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptITable_pimSGRptIJoinPruneState
*
* @purpose Get 'pimSGRptIJoinPruneState'
*
* @description [pimSGRptIJoinPruneState] The state resulting from (S,G,rpt) Join/Prune messages received on this interface. This corresponds to the state of the downstream per-interface (S,G,rpt) state machine in the PIM-SM specification.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptITable_pimSGRptIJoinPruneState (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimSGRptSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGRptSrcAddressValue;

  fpObjWa_t kwapimSGRptIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGRptIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptIJoinPruneStateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGAddressType,
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
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimSGRptSrcAddress */
  kwapimSGRptSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptSrcAddress,
                                            (xLibU8_t *) &keypimSGRptSrcAddressValue,
                                            &kwapimSGRptSrcAddress.len);
  if (kwapimSGRptSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptSrcAddress);
    return kwapimSGRptSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptSrcAddressValue, kwapimSGRptSrcAddress.len);

  /* retrieve key: pimSGRptIIfIndex */
  kwapimSGRptIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptIIfIndex,
                                          (xLibU8_t *) & keypimSGRptIIfIndexValue,
                                          &kwapimSGRptIIfIndex.len);
  if (kwapimSGRptIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptIIfIndex);
    return kwapimSGRptIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptIIfIndexValue, kwapimSGRptIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGRptIJoinPruneStateGet (keypimStarGAddressTypeValue,
                                                                            &keypimStarGGrpAddressValue,
                                                                            &keypimSGRptSrcAddressValue,
                                                                            keypimSGRptIIfIndexValue, &objpimSGRptIJoinPruneStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRptIJoinPruneState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptIJoinPruneStateValue,
                           sizeof (objpimSGRptIJoinPruneStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptITable_pimSGRptIPrunePendingTimer
*
* @purpose Get 'pimSGRptIPrunePendingTimer'
*
* @description [pimSGRptIPrunePendingTimer] The time remaining before the local router starts pruning this source off the RP tree. This timer is called the (S,G,rpt) Prune-Pending Timer in the PIM-SM specification. This object is zero if the timer is not running.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptITable_pimSGRptIPrunePendingTimer (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimSGRptSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGRptSrcAddressValue;

  fpObjWa_t kwapimSGRptIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGRptIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptIPrunePendingTimerValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGAddressType,
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
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimSGRptSrcAddress */
  kwapimSGRptSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptSrcAddress,
                                            (xLibU8_t *) &keypimSGRptSrcAddressValue,
                                            &kwapimSGRptSrcAddress.len);
  if (kwapimSGRptSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptSrcAddress);
    return kwapimSGRptSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptSrcAddressValue, kwapimSGRptSrcAddress.len);

  /* retrieve key: pimSGRptIIfIndex */
  kwapimSGRptIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptIIfIndex,
                                          (xLibU8_t *) & keypimSGRptIIfIndexValue,
                                          &kwapimSGRptIIfIndex.len);
  if (kwapimSGRptIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptIIfIndex);
    return kwapimSGRptIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptIIfIndexValue, kwapimSGRptIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGRptIPrunePendingTimerGet (keypimStarGAddressTypeValue,
                                                                                  &keypimStarGGrpAddressValue,
                                                                                  &keypimSGRptSrcAddressValue,
                                                                                  keypimSGRptIIfIndexValue, &objpimSGRptIPrunePendingTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRptIPrunePendingTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptIPrunePendingTimerValue,
                           sizeof (objpimSGRptIPrunePendingTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptITable_pimSGRptIPruneExpiryTimer
*
* @purpose Get 'pimSGRptIPruneExpiryTimer'
*
* @description [pimSGRptIPruneExpiryTimer] The time remaining before (S,G,rpt) Prune state for this interface expires. This timer is called the (S,G,rpt) Prune Expiry Timer in the PIM-SM specification. This object is zero if the timer is not running. A value of 'FFFFFFFF'h indicates an infinite expiry time.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptITable_pimSGRptIPruneExpiryTimer (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t kwapimSGRptSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGRptSrcAddressValue;

  fpObjWa_t kwapimSGRptIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGRptIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptIPruneExpiryTimerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGAddressType,
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
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimSGRptSrcAddress */
  kwapimSGRptSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptSrcAddress,
                                            (xLibU8_t *) &keypimSGRptSrcAddressValue,
                                            &kwapimSGRptSrcAddress.len);
  if (kwapimSGRptSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptSrcAddress);
    return kwapimSGRptSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptSrcAddressValue, kwapimSGRptSrcAddress.len);;

  /* retrieve key: pimSGRptIIfIndex */
  kwapimSGRptIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptITable_pimSGRptIIfIndex,
                                          (xLibU8_t *) & keypimSGRptIIfIndexValue,
                                          &kwapimSGRptIIfIndex.len);
  if (kwapimSGRptIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptIIfIndex);
    return kwapimSGRptIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptIIfIndexValue, kwapimSGRptIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGRptIJoinExpiryTimerGet (keypimStarGAddressTypeValue,
                                                                              &keypimStarGGrpAddressValue,
                                                                              &keypimSGRptSrcAddressValue,
                                                                              keypimSGRptIIfIndexValue, &objpimSGRptIPruneExpiryTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRptIPruneExpiryTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptIPruneExpiryTimerValue,
                           sizeof (objpimSGRptIPruneExpiryTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
