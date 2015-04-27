
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimSGITable.c
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
#include "_xe_ipmcastpimSGITable_obj.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGAddressType
*
* @purpose Get 'pimSGAddressType'
*
* @description [pimSGAddressType] The address type of the source and multicast group for this entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGAddressTypeValue;
  xLibU32_t nextObjpimSGAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                          (xLibU8_t *) & objpimSGAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimSGAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGAddressTypeValue, owa.len);

    if (objpimSGAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimSGAddressTypeValue = objpimSGAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGAddressTypeValue, owa.len);

  /* return the object value: pimStarGAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimSGAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGGrpAddress
*
* @purpose Get 'pimSGGrpAddress'
*
* @description [pimSGGrpAddress] The multicast group address for this entry. The InetAddressType is given by the pimSGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGGrpAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  xLibU32_t nextObjpimSGIIfIndexValue;
  
  L7_inet_addr_t objpimSGGrpAddressValue;
  L7_inet_addr_t nextObjpimSGGrpAddressValue;
  
  L7_inet_addr_t nextObjpimSGSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                          (xLibU8_t *) &objpimSGGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjpimSGIIfIndexValue = 0;

    memset(&nextObjpimSGGrpAddressValue, 0, sizeof(L7_inet_addr_t));
    memset(&nextObjpimSGSrcAddressValue, 0, sizeof(L7_inet_addr_t));
    
    owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                   &nextObjpimSGGrpAddressValue,
                                                                   &nextObjpimSGSrcAddressValue,
                                                                   &nextObjpimSGIIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGGrpAddressValue, owa.len);

    nextObjpimSGIIfIndexValue = 0;
    memcpy(&nextObjpimSGGrpAddressValue, &objpimSGGrpAddressValue, sizeof(L7_inet_addr_t));
    memset(&nextObjpimSGSrcAddressValue, 0, sizeof(L7_inet_addr_t));
    
    do
    {
      owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                     &nextObjpimSGGrpAddressValue,
                                                                     &nextObjpimSGSrcAddressValue,
                                                                     &nextObjpimSGIIfIndexValue);
    }
    while (!memcmp(&objpimSGGrpAddressValue, &nextObjpimSGGrpAddressValue, sizeof(L7_inet_addr_t)) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGGrpAddressValue, owa.len);

  /* return the object value: pimSGGrpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimSGGrpAddressValue,
                           sizeof (objpimSGGrpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGSrcAddress
*
* @purpose Get 'pimSGSrcAddress'
*
* @description [pimSGSrcAddress] The source address for this entry. The InetAddressType is given by the pimSGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGSrcAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  L7_inet_addr_t objpimSGGrpAddressValue;
  L7_inet_addr_t nextObjpimSGGrpAddressValue;

  L7_inet_addr_t objpimSGSrcAddressValue;
  L7_inet_addr_t nextObjpimSGSrcAddressValue;

  xLibU32_t nextObjpimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                          (xLibU8_t *) &objpimSGGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGGrpAddressValue, owa.len);

  /* retrieve key: pimSGSrcAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                          (xLibU8_t *) &objpimSGSrcAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjpimSGIIfIndexValue = 0;
    memset(&nextObjpimSGSrcAddressValue, 0, sizeof(L7_inet_addr_t));    
    
    owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                   &nextObjpimSGGrpAddressValue,
                                                                   &nextObjpimSGSrcAddressValue,
                                                                   &nextObjpimSGIIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGSrcAddressValue, owa.len);

    nextObjpimSGIIfIndexValue = 0;
    memcpy(&nextObjpimSGSrcAddressValue, &objpimSGSrcAddressValue, sizeof(L7_inet_addr_t));
    
    do
    {
      owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                     &nextObjpimSGGrpAddressValue,
                                                                     &nextObjpimSGSrcAddressValue,
                                                                     &nextObjpimSGIIfIndexValue);
    }
    while (!memcmp(&objpimSGGrpAddressValue, &nextObjpimSGGrpAddressValue,sizeof(L7_inet_addr_t))
           && (!memcmp(&objpimSGSrcAddressValue, &nextObjpimSGSrcAddressValue, sizeof(L7_inet_addr_t)) && (owa.l7rc == L7_SUCCESS)));
  }

  if (memcmp(&objpimSGGrpAddressValue, &nextObjpimSGGrpAddressValue,sizeof(L7_inet_addr_t)) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGSrcAddressValue, owa.len);

  /* return the object value: pimSGSrcAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimSGSrcAddressValue,
                           sizeof (objpimSGSrcAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIIfIndex
*
* @purpose Get 'pimSGIIfIndex'
*
* @description [pimSGIIfIndex] The ifIndex of the interface that this entry corresponds to.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIIfIndex (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  L7_inet_addr_t objpimSGGrpAddressValue;
  L7_inet_addr_t nextObjpimSGGrpAddressValue;
  
  L7_inet_addr_t objpimSGSrcAddressValue;
  L7_inet_addr_t nextObjpimSGSrcAddressValue;

  xLibU32_t objpimSGIIfIndexValue;
  xLibU32_t nextObjpimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                          (xLibU8_t *) &objpimSGGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGGrpAddressValue, owa.len);

  /* retrieve key: pimSGSrcAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                          (xLibU8_t *) &objpimSGSrcAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGSrcAddressValue, owa.len);

  /* retrieve key: pimSGIIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                          (xLibU8_t *) & objpimSGIIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimSGIIfIndexValue = 0;

    owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                   &nextObjpimSGGrpAddressValue,
                                                                   &nextObjpimSGSrcAddressValue,
                                                                   &nextObjpimSGIIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGIIfIndexValue, owa.len);
    nextObjpimSGIIfIndexValue = objpimSGIIfIndexValue;

    owa.l7rc = usmDbPimSGIEntryNextGet (keypimSGAddressTypeValue,
                                                                   &nextObjpimSGGrpAddressValue,
                                                                   &nextObjpimSGSrcAddressValue,
                                                                   &nextObjpimSGIIfIndexValue);
  }

  if ((memcmp(&objpimSGGrpAddressValue, &nextObjpimSGGrpAddressValue, sizeof(L7_inet_addr_t)))
      || (memcmp(&objpimSGSrcAddressValue, &nextObjpimSGSrcAddressValue,sizeof(L7_inet_addr_t))) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGIIfIndexValue, owa.len);

  /* return the object value: pimSGIIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimSGIIfIndexValue,
                           sizeof (objpimSGIIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIUpTime
*
* @purpose Get 'pimSGIUpTime'
*
* @description [pimSGIUpTime] The time since this entry was created by the local router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;
  
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;
  
  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIUpTimeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGIUpTimeGet (keypimSGAddressTypeValue,
                                                            &keypimSGGrpAddressValue,
                                                            &keypimSGSrcAddressValue,
                                                            keypimSGIIfIndexValue, &objpimSGIUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIUpTimeValue,
                           sizeof (objpimSGIUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGILocalMembership
*
* @purpose Get 'pimSGILocalMembership'
*
* @description [pimSGILocalMembership] Whether the local router has (S,G) local membership on this interface (resulting from a mechanism such as IGMP or MLD).
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGILocalMembership (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;
  
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;
  
  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGILocalMembershipValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGILocalMembershipGet (keypimSGAddressTypeValue,
                                                                          &keypimSGGrpAddressValue,
                                                                          &keypimSGSrcAddressValue,
                                                                          keypimSGIIfIndexValue, &objpimSGILocalMembershipValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGILocalMembership */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGILocalMembershipValue,
                           sizeof (objpimSGILocalMembershipValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIJoinPruneState
*
* @purpose Get 'pimSGIJoinPruneState'
*
* @description [pimSGIJoinPruneState] The state resulting from (S,G) Join/Prune messages received on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIJoinPruneState (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;
  
  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;
  
  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIJoinPruneStateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGIJoinPruneStateGet (keypimSGAddressTypeValue,
                                                                        &keypimSGGrpAddressValue,
                                                                        &keypimSGSrcAddressValue,
                                                                        keypimSGIIfIndexValue, &objpimSGIJoinPruneStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIJoinPruneState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIJoinPruneStateValue,
                           sizeof (objpimSGIJoinPruneStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIPrunePendingTimer
*
* @purpose Get 'pimSGIPrunePendingTimer'
*
* @description [pimSGIPrunePendingTimer] The time remaining before the local router acts on an (S,G) Prune message received on this interface, during which the router is waiting to see whether another downstream router will override the Prune message.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIPrunePendingTimer (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIPrunePendingTimerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *)& keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGIPrunePendingTimerGet (keypimSGAddressTypeValue,
                                                                            &keypimSGGrpAddressValue,
                                                                            &keypimSGSrcAddressValue,
                                                                            keypimSGIIfIndexValue, &objpimSGIPrunePendingTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIPrunePendingTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIPrunePendingTimerValue,
                           sizeof (objpimSGIPrunePendingTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIJoinExpiryTimer
*
* @purpose Get 'pimSGIJoinExpiryTimer'
*
* @description [pimSGIJoinExpiryTimer] The time remaining before (S,G) Join state for this interface expires.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIJoinExpiryTimer (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIJoinExpiryTimerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGIJoinExpiryTimerGet (keypimSGAddressTypeValue,
                                                                        &keypimSGGrpAddressValue,
                                                                        &keypimSGSrcAddressValue,
                                                                        keypimSGIIfIndexValue, &objpimSGIJoinExpiryTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIJoinExpiryTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIJoinExpiryTimerValue,
                           sizeof (objpimSGIJoinExpiryTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIAssertState
*
* @purpose Get 'pimSGIAssertState'
*
* @description [pimSGIAssertState] The (S,G) Assert state for this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIAssertState (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIAssertStateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *)& keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *)& keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGIAssertStateGet (keypimSGAddressTypeValue,
                                                                  &keypimSGGrpAddressValue,
                                                                  &keypimSGSrcAddressValue,
                                                                  keypimSGIIfIndexValue, &objpimSGIAssertStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIAssertState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIAssertStateValue,
                           sizeof (objpimSGIAssertStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIAssertTimer
*
* @purpose Get 'pimSGIAssertTimer'
*
* @description [pimSGIAssertTimer] If pimSGIAssertState is 'iAmAssertWinner', this is the time remaining before the local router next sends a (S,G) Assert message on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIAssertTimer (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIAssertTimerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGIAssertTimerGet (keypimSGAddressTypeValue,
                                                                    &keypimSGGrpAddressValue,
                                                                    &keypimSGSrcAddressValue,
                                                                    keypimSGIIfIndexValue, &objpimSGIAssertTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIAssertTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIAssertTimerValue,
                           sizeof (objpimSGIAssertTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIAssertWinnerAddressType
*
* @purpose Get 'pimSGIAssertWinnerAddressType'
*
* @description [pimSGIAssertWinnerAddressType] If pimSGIAssertState is 'iAmAssertLoser', this is the address type of the assert winner; otherwise, this object is unknown(0).
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIAssertWinnerAddressType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIAssertWinnerAddressTypeValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimSGAddressTypeValue,
                              &keypimSGGrpAddressValue,
                              &keypimSGSrcAddressValue,
                              keypimSGIIfIndexValue, &objpimSGIAssertWinnerAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIAssertWinnerAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIAssertWinnerAddressTypeValue,
                           sizeof (objpimSGIAssertWinnerAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIAssertWinnerAddress
*
* @purpose Get 'pimSGIAssertWinnerAddress'
*
* @description [pimSGIAssertWinnerAddress] If pimSGIAssertState is 'iAmAssertLoser', this is the address of the assert winner. The InetAddressType is given by the pimSGIAssertWinnerAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIAssertWinnerAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimSGIAssertWinnerAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGIAssertWinnerAddrGet (keypimSGAddressTypeValue,
                                                                            &keypimSGGrpAddressValue,
                                                                            &keypimSGSrcAddressValue,
                                                                            keypimSGIIfIndexValue, &objpimSGIAssertWinnerAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIAssertWinnerAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimSGIAssertWinnerAddressValue,
                           sizeof (objpimSGIAssertWinnerAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIAssertWinnerMetricPref
*
* @purpose Get 'pimSGIAssertWinnerMetricPref'
*
* @description [pimSGIAssertWinnerMetricPref] If pimSGIAssertState is 'iAmAssertLoser', this is the metric preference of the route to the source advertised by the assert winner; otherwise, this object is zero.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIAssertWinnerMetricPref (void *wap, void *bufp)
{

  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;
  
  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIAssertWinnerMetricPrefValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGIRPFRouteMetricPrefGet (keypimSGAddressTypeValue,
                                                                              &keypimSGGrpAddressValue,
                                                                              &keypimSGSrcAddressValue,
                                                                              keypimSGIIfIndexValue, &objpimSGIAssertWinnerMetricPrefValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIAssertWinnerMetricPref */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIAssertWinnerMetricPrefValue,
                           sizeof (objpimSGIAssertWinnerMetricPrefValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGITable_pimSGIAssertWinnerMetric
*
* @purpose Get 'pimSGIAssertWinnerMetric'
*
* @description [pimSGIAssertWinnerMetric] If pimSGIAssertState is 'iAmAssertLoser', this is the routing metric of the route to the source advertised by the assert winner; otherwise, this object is zero.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGITable_pimSGIAssertWinnerMetric (void *wap, void *bufp)
{
  fpObjWa_t kwapimSGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGAddressTypeValue;

  fpObjWa_t kwapimSGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGGrpAddressValue;

  fpObjWa_t kwapimSGSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGSrcAddressValue;

  fpObjWa_t kwapimSGIIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimSGIIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIAssertWinnerMetricValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimSGAddressType */
  kwapimSGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGAddressType,
                                          (xLibU8_t *) & keypimSGAddressTypeValue,
                                          &kwapimSGAddressType.len);
  if (kwapimSGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimSGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGAddressType);
    return kwapimSGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGAddressTypeValue, kwapimSGAddressType.len);

  /* retrieve key: pimSGGrpAddress */
  kwapimSGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGGrpAddress,
                                         (xLibU8_t *) &keypimSGGrpAddressValue,
                                         &kwapimSGGrpAddress.len);
  if (kwapimSGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGGrpAddress);
    return kwapimSGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGGrpAddressValue, kwapimSGGrpAddress.len);

  /* retrieve key: pimSGSrcAddress */
  kwapimSGSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGSrcAddress,
                                         (xLibU8_t *) &keypimSGSrcAddressValue,
                                         &kwapimSGSrcAddress.len);
  if (kwapimSGSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGSrcAddress);
    return kwapimSGSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGSrcAddressValue, kwapimSGSrcAddress.len);

  /* retrieve key: pimSGIIfIndex */
  kwapimSGIIfIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGITable_pimSGIIfIndex,
                                       (xLibU8_t *) & keypimSGIIfIndexValue, &kwapimSGIIfIndex.len);
  if (kwapimSGIIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimSGIIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGIIfIndex);
    return kwapimSGIIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGIIfIndexValue, kwapimSGIIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbPimSGIRPFRouteMetricPrefGet (keypimSGAddressTypeValue,
                                                                              &keypimSGGrpAddressValue,
                                                                              &keypimSGSrcAddressValue,
                                                                              keypimSGIIfIndexValue, &objpimSGIAssertWinnerMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGIAssertWinnerMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIAssertWinnerMetricValue,
                           sizeof (objpimSGIAssertWinnerMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
