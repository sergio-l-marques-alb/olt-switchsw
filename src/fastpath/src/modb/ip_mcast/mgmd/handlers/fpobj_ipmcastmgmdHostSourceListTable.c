
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdHostSourceListTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  01 May 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastmgmdHostSourceListTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"
#include "usmdb_mib_igmp_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddressType
*
* @purpose Get 'mgmdHostSrcListAddressType'
*
* @description [mgmdHostSrcListAddressType] The address type of the InetAddress variables in this table. This value applies to the mgmdHostSrcListHostAddress and mgmdHostSrcListAddress entries.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddressType (void *wap, void *bufp)
{

  xLibU32_t objmgmdHostSrcListAddressTypeValue;
  xLibU32_t nextObjmgmdHostSrcListAddressTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostSrcListAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddressType,
                          (xLibU8_t *) & objmgmdHostSrcListAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdHostSrcListAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else if (objmgmdHostSrcListAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListAddressTypeValue, owa.len);
    nextObjmgmdHostSrcListAddressTypeValue = L7_INET_ADDR_TYPE_IPV6;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListAddressTypeValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }
    
  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostSrcListAddressTypeValue, owa.len);

  /* return the object value: mgmdHostSrcListAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdHostSrcListAddressTypeValue,
                           sizeof (objmgmdHostSrcListAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddress
*
* @purpose Get 'mgmdHostSrcListAddress'
*
* @description [mgmdHostSrcListAddress] The IP multicast group address for which this entry contains information.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddress (void *wap, void *bufp)
{

  xLibU32_t objmgmdHostSrcListAddressTypeValue;
  xLibU32_t objmgmdHostSrcListIfIndexValue;
  xLibU32_t nextObjmgmdHostSrcListIfIndexValue;
  L7_inet_addr_t objmgmdHostSrcListAddressValue;
  L7_inet_addr_t nextObjmgmdHostSrcListAddressValue;
  L7_inet_addr_t inetTempIP;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  fpObjWa_t kwaType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostSrcListAddressType */
  kwaType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddressType,
                          (xLibU8_t *) & objmgmdHostSrcListAddressTypeValue, &kwaType.len);
  if (kwaType.rc != XLIBRC_SUCCESS)
  {
    kwaType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaType);
    return kwaType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListAddressTypeValue, kwaType.len);

  owa.l7rc = usmDbMgmdProxyInterfaceGet(L7_UNIT_CURRENT,
                                 objmgmdHostSrcListAddressTypeValue,
                                 &objmgmdHostSrcListIfIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_MGMD_PROXY_NOT_ENBLD;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListIfIndexValue, kwaIndex.len);

  if (usmDbMgmdProxyInterfaceOperationalStateGet (L7_UNIT_CURRENT,
                                     objmgmdHostSrcListAddressTypeValue,
                                     objmgmdHostSrcListIfIndexValue) != L7_TRUE)
  {
    owa.rc = XLIBRC_MGMD_PROXY_NOT_OPERATIONAL;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  inetAddressZeroSet(objmgmdHostSrcListAddressTypeValue, &nextObjmgmdHostSrcListAddressValue);
  inetAddressZeroSet(objmgmdHostSrcListAddressTypeValue, &objmgmdHostSrcListAddressValue);

  nextObjmgmdHostSrcListIfIndexValue = objmgmdHostSrcListIfIndexValue;

  /* retrieve key: mgmdHostSrcListAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddress,
                          (xLibU8_t *) &objmgmdHostSrcListAddressValue, &owa.len);



  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(objmgmdHostSrcListAddressTypeValue, &inetTempIP);
    owa.l7rc = usmDbMgmdHostCacheEntryNextGet (L7_UNIT_CURRENT,
                                               objmgmdHostSrcListAddressTypeValue,
                                               &inetTempIP,
                                               &nextObjmgmdHostSrcListIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListAddressValue, owa.len);
    inetCopy(&inetTempIP, &objmgmdHostSrcListAddressValue);
    owa.l7rc = usmDbMgmdHostCacheEntryNextGet (L7_UNIT_CURRENT,
                                               objmgmdHostSrcListAddressTypeValue,
                                               &inetTempIP,
                                               &nextObjmgmdHostSrcListIfIndexValue);
  }



  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  inetCopy(&nextObjmgmdHostSrcListAddressValue, &inetTempIP);

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostSrcListAddressValue, owa.len);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostSrcListAddressValue, owa.len);

  /* return the object value: mgmdHostSrcListAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdHostSrcListAddressValue,
                           sizeof (objmgmdHostSrcListAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListIfIndex
*
* @purpose Get 'mgmdHostSrcListIfIndex'
*
* @description [mgmdHostSrcListIfIndex] The interface for which this entry contains information for an IP multicast group address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListIfIndex (void *wap, void *bufp)
{

  xLibU32_t objmgmdHostSrcListAddressTypeValue;
  xLibU32_t objmgmdHostSrcListIfIndexValue;
  xLibU32_t nextObjmgmdHostSrcListIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostSrcListAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddressType,
                          (xLibU8_t *) & objmgmdHostSrcListAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListAddressTypeValue, owa.len);

  /* retrieve key: mgmdHostSrcListIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListIfIndex,
                          (xLibU8_t *) & objmgmdHostSrcListIfIndexValue, &owa.len);


  if (owa.rc == XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  owa.l7rc = usmDbMgmdProxyInterfaceGet(L7_UNIT_CURRENT,
                                 objmgmdHostSrcListAddressTypeValue,
                                 &nextObjmgmdHostSrcListIfIndexValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_MGMD_PROXY_NOT_ENBLD;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostSrcListIfIndexValue, owa.len);

  /* return the object value: mgmdHostSrcListIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdHostSrcListIfIndexValue,
                           sizeof (objmgmdHostSrcListIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListHostAddress
*
* @purpose Get 'mgmdHostSrcListHostAddress'
*
* @description [mgmdHostSrcListHostAddress] The host address to which this entry corresponds. The mgmdHostCacheSourceFilterMode value for this Group address and interface indicates whether this Host address is included or excluded.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListHostAddress (void *wap, void *bufp)
{

  xLibU32_t objmgmdHostSrcListAddressTypeValue;
  xLibU32_t objmgmdHostSrcListIfIndexValue;
  L7_inet_addr_t objmgmdHostSrcListAddressValue;
  L7_inet_addr_t objmgmdHostSrcListHostAddressValue;
  L7_inet_addr_t nextObjmgmdHostSrcListHostAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  fpObjWa_t kwaType = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  fpObjWa_t kwaIndex = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  fpObjWa_t kwaListAddressValue = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t srcCount = 0;

  /* retrieve key: mgmdHostSrcListAddressType */
  kwaType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddressType,
                          (xLibU8_t *) & objmgmdHostSrcListAddressTypeValue, &kwaType.len);
  if (kwaType.rc != XLIBRC_SUCCESS)
  {
    kwaType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaType);
    return kwaType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListAddressTypeValue, kwaType.len);

  /* retrieve key: mgmdHostSrcListIfIndex */
  kwaIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListIfIndex,
                          (xLibU8_t *) & objmgmdHostSrcListIfIndexValue, &kwaIndex.len);
  if (kwaIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIndex);
    return kwaIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListIfIndexValue, kwaIndex.len);

  /* retrieve key: mgmdHostSrcListAddress */
  kwaListAddressValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddress,
                          (xLibU8_t *) &objmgmdHostSrcListAddressValue, &kwaListAddressValue.len);
  if (kwaListAddressValue.rc != XLIBRC_SUCCESS)
  {
    kwaListAddressValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaListAddressValue);
    return kwaListAddressValue.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListAddressValue, kwaListAddressValue.len);


  inetAddressZeroSet(objmgmdHostSrcListAddressTypeValue, &nextObjmgmdHostSrcListHostAddressValue);

  /* retrieve key: mgmdHostSrcListHostAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListHostAddress,
                          (xLibU8_t *) &objmgmdHostSrcListHostAddressValue, &owa.len);


  /* get the value from application */
  owa.l7rc = usmDbMgmdHostInterfaceSrcsGet (L7_UNIT_CURRENT, objmgmdHostSrcListAddressTypeValue,
                              (L7_inet_addr_t *)&objmgmdHostSrcListAddressValue,
                              objmgmdHostSrcListIfIndexValue, &srcCount);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (srcCount == L7_NULL)
  {
    if (owa.rc != XLIBRC_SUCCESS)
    {
      memset(&nextObjmgmdHostSrcListHostAddressValue, 0, sizeof(nextObjmgmdHostSrcListHostAddressValue));
      owa.l7rc = L7_SUCCESS;

      /* return the object value: mgmdHostSrcListHostAddressValue */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdHostSrcListHostAddressValue,
                            sizeof (objmgmdHostSrcListHostAddressValue));

      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else
    {
      memset(&nextObjmgmdHostSrcListHostAddressValue, 0, sizeof(nextObjmgmdHostSrcListHostAddressValue));

      /* return the object value: mgmdHostSrcListHostAddressValue */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdHostSrcListHostAddressValue,
                            sizeof (objmgmdHostSrcListHostAddressValue));

      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbMgmdHostSrcListEntryGetNext (L7_UNIT_CURRENT,
                                     objmgmdHostSrcListAddressTypeValue,
                                     &objmgmdHostSrcListAddressValue,
                                     &objmgmdHostSrcListIfIndexValue,
                                     &nextObjmgmdHostSrcListHostAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostSrcListHostAddressValue, owa.len);
    inetCopy(&nextObjmgmdHostSrcListHostAddressValue, &objmgmdHostSrcListHostAddressValue);
    owa.l7rc = usmDbMgmdHostSrcListEntryGetNext (L7_UNIT_CURRENT,
                                     objmgmdHostSrcListAddressTypeValue,
                                     &objmgmdHostSrcListAddressValue,
                                     &objmgmdHostSrcListIfIndexValue,
                                     &nextObjmgmdHostSrcListHostAddressValue);
  }


  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostSrcListHostAddressValue, owa.len);

  /* return the object value: mgmdHostSrcListHostAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdHostSrcListHostAddressValue,
                           sizeof (objmgmdHostSrcListHostAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListExpire
*
* @purpose Get 'mgmdHostSrcListExpire'
*
* @description [mgmdHostSrcListExpire] The Expiry time of the Source List Entry.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostSourceListTable_mgmdHostSrcListExpire (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostSrcListAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostSrcListAddressTypeValue;
  fpObjWa_t kwamgmdHostSrcListAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_inet_addr_t  keymgmdHostSrcListAddressValue;
  fpObjWa_t kwamgmdHostSrcListIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostSrcListIfIndexValue;
  fpObjWa_t kwamgmdHostSrcListHostAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_inet_addr_t  keymgmdHostSrcListHostAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostSrcListExpireValue;
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: mgmdHostSrcListAddressType */
  kwamgmdHostSrcListAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddressType,
                   (xLibU8_t *) & keymgmdHostSrcListAddressTypeValue,
                   &kwamgmdHostSrcListAddressType.len);
  if (kwamgmdHostSrcListAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostSrcListAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostSrcListAddressType);
    return kwamgmdHostSrcListAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostSrcListAddressTypeValue,
                           kwamgmdHostSrcListAddressType.len);

  /* retrieve key: mgmdHostSrcListAddress */
  kwamgmdHostSrcListAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListAddress,
                   (xLibU8_t *) &keymgmdHostSrcListAddressValue, &kwamgmdHostSrcListAddress.len);
  if (kwamgmdHostSrcListAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostSrcListAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostSrcListAddress);
    return kwamgmdHostSrcListAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostSrcListAddressValue, kwamgmdHostSrcListAddress.len);

  /* retrieve key: mgmdHostSrcListIfIndex */
  kwamgmdHostSrcListIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListIfIndex,
                   (xLibU8_t *) & keymgmdHostSrcListIfIndexValue, &kwamgmdHostSrcListIfIndex.len);
  if (kwamgmdHostSrcListIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostSrcListIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostSrcListIfIndex);
    return kwamgmdHostSrcListIfIndex.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostSrcListIfIndexValue, kwamgmdHostSrcListIfIndex.len);

  /* retrieve key: mgmdHostSrcListHostAddress */
  kwamgmdHostSrcListHostAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostSourceListTable_mgmdHostSrcListHostAddress,
                   (xLibU8_t *) &keymgmdHostSrcListHostAddressValue,
                   &kwamgmdHostSrcListHostAddress.len);
  if (kwamgmdHostSrcListHostAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostSrcListHostAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostSrcListHostAddress);
    return kwamgmdHostSrcListHostAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostSrcListHostAddressValue,
                           kwamgmdHostSrcListHostAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostSrcListExpiryTimeGet (L7_UNIT_CURRENT, keymgmdHostSrcListAddressTypeValue,
                              keymgmdHostSrcListIfIndexValue,
                              &keymgmdHostSrcListAddressValue,
                              &keymgmdHostSrcListHostAddressValue, &objmgmdHostSrcListExpireValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostSrcListExpire */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostSrcListExpireValue,
                           sizeof (objmgmdHostSrcListExpireValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

