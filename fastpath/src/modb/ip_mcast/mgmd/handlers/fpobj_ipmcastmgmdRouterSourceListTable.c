
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdRouterSourceListTable.c
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
#include "_xe_ipmcastmgmdRouterSourceListTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"
#include "usmdb_mib_igmp_api.h"
#include "l7_mgmd_api.h"
/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddressType
*
* @purpose Get 'mgmdRouterSrcListAddressType'
*
* @description [mgmdRouterSrcListAddressType] The address type of the InetAddress variables in this table. This value applies to the mgmdRouterSrcListHostAddress and mgmdRouterSrcListAddress entries.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddressType (void *wap,
                                                                                 void *bufp)
{

  xLibU32_t objmgmdRouterSrcListAddressTypeValue;
  xLibU32_t nextObjmgmdRouterSrcListAddressTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterSrcListAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddressType,
                          (xLibU8_t *) & objmgmdRouterSrcListAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdRouterSrcListAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else if (objmgmdRouterSrcListAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListAddressTypeValue, owa.len);
    nextObjmgmdRouterSrcListAddressTypeValue = L7_INET_ADDR_TYPE_IPV6;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListAddressTypeValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }
  

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterSrcListAddressTypeValue, owa.len);

  /* return the object value: mgmdRouterSrcListAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdRouterSrcListAddressTypeValue,
                           sizeof (objmgmdRouterSrcListAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddress
*
* @purpose Get 'mgmdRouterSrcListAddress'
*
* @description [mgmdRouterSrcListAddress] The IP multicast group address for which this entry contains information.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddress (void *wap, void *bufp)
{

  xLibU32_t objmgmdRouterSrcListAddressTypeValue;
  xLibU32_t nextObjmgmdRouterSrcListIfIndexValue;
  L7_inet_addr_t objmgmdRouterSrcListAddressValue;
  L7_inet_addr_t nextObjmgmdRouterSrcListAddressValue;
  L7_inet_addr_t nextObjmgmdRouterSrcListHostAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  fpObjWa_t kwaobjmgmdRouterSrcListAddressTypeValue = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterSrcListAddressType */
  kwaobjmgmdRouterSrcListAddressTypeValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddressType,
                          (xLibU8_t *) & objmgmdRouterSrcListAddressTypeValue, &kwaobjmgmdRouterSrcListAddressTypeValue.len);
  if (kwaobjmgmdRouterSrcListAddressTypeValue.rc != XLIBRC_SUCCESS)
  {
    kwaobjmgmdRouterSrcListAddressTypeValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaobjmgmdRouterSrcListAddressTypeValue);
    return kwaobjmgmdRouterSrcListAddressTypeValue.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListAddressTypeValue, kwaobjmgmdRouterSrcListAddressTypeValue.len);

  /* retrieve key: mgmdRouterSrcListAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddress,
                          (xLibU8_t *) &objmgmdRouterSrcListAddressValue, &owa.len);
  

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(objmgmdRouterSrcListAddressTypeValue,&nextObjmgmdRouterSrcListAddressValue);
    nextObjmgmdRouterSrcListIfIndexValue = 0;
    inetAddressZeroSet(objmgmdRouterSrcListAddressTypeValue,&nextObjmgmdRouterSrcListHostAddressValue);
    owa.l7rc =
      usmDbMgmdSrcListEntryNextGet (L7_UNIT_CURRENT, objmgmdRouterSrcListAddressTypeValue,
                            &nextObjmgmdRouterSrcListAddressValue,
                            &nextObjmgmdRouterSrcListIfIndexValue,
                            &nextObjmgmdRouterSrcListHostAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListAddressValue, owa.len);
    nextObjmgmdRouterSrcListIfIndexValue = 0;
    inetAddressZeroSet(objmgmdRouterSrcListAddressTypeValue,&nextObjmgmdRouterSrcListHostAddressValue);
    do
    {
    owa.l7rc =
      usmDbMgmdSrcListEntryNextGet (L7_UNIT_CURRENT, objmgmdRouterSrcListAddressTypeValue,
                            &nextObjmgmdRouterSrcListAddressValue,
                            &nextObjmgmdRouterSrcListIfIndexValue,
                            &nextObjmgmdRouterSrcListHostAddressValue);
    }
    while((owa.l7rc == L7_SUCCESS) &&
          (L7_INET_ADDR_COMPARE(&nextObjmgmdRouterSrcListAddressValue,&objmgmdRouterSrcListAddressValue) == 0));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterSrcListAddressValue, owa.len);

  /* return the object value: mgmdRouterSrcListAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdRouterSrcListAddressValue,
                           sizeof (objmgmdRouterSrcListAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListIfIndex
*
* @purpose Get 'mgmdRouterSrcListIfIndex'
*
* @description [mgmdRouterSrcListIfIndex] The interface for which this entry contains information for an IP multicast group address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListIfIndex (void *wap, void *bufp)
{

  xLibU32_t objmgmdRouterSrcListAddressTypeValue;
  xLibU32_t objmgmdRouterSrcListIfIndexValue;
  xLibU32_t nextObjmgmdRouterSrcListIfIndexValue;
  L7_inet_addr_t objmgmdRouterSrcListAddressValue;
  L7_inet_addr_t nextObjmgmdRouterSrcListAddressValue;
  L7_inet_addr_t nextObjmgmdRouterSrcListHostAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaobjmgmdRouterSrcListIfIndexValue = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaobjmgmdRouterSrcListAddrValue = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterSrcListAddressType */
  kwaobjmgmdRouterSrcListIfIndexValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddressType,
                          (xLibU8_t *) & objmgmdRouterSrcListAddressTypeValue, &kwaobjmgmdRouterSrcListIfIndexValue.len);
  if (kwaobjmgmdRouterSrcListIfIndexValue.rc != XLIBRC_SUCCESS)
  {
    kwaobjmgmdRouterSrcListIfIndexValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaobjmgmdRouterSrcListIfIndexValue);
    return kwaobjmgmdRouterSrcListIfIndexValue.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListAddressTypeValue, owa.len);

  /* retrieve key: mgmdRouterSrcListAddress */
  kwaobjmgmdRouterSrcListAddrValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddress,
                          (xLibU8_t *) &objmgmdRouterSrcListAddressValue, &owa.len);
  if (kwaobjmgmdRouterSrcListAddrValue.rc != XLIBRC_SUCCESS)
  {
    kwaobjmgmdRouterSrcListAddrValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaobjmgmdRouterSrcListAddrValue);
    return kwaobjmgmdRouterSrcListAddrValue.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListAddressValue, owa.len);

  /* retrieve key: mgmdRouterSrcListIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListIfIndex,
                          (xLibU8_t *) & objmgmdRouterSrcListIfIndexValue, &owa.len);

           
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdRouterSrcListIfIndexValue = 0;
    inetCopy(&nextObjmgmdRouterSrcListAddressValue,&objmgmdRouterSrcListAddressValue);
    inetAddressZeroSet(objmgmdRouterSrcListAddressTypeValue,&nextObjmgmdRouterSrcListHostAddressValue);
    owa.l7rc =
      usmDbMgmdSrcListEntryNextGet (L7_UNIT_CURRENT, objmgmdRouterSrcListAddressTypeValue,
                            &nextObjmgmdRouterSrcListAddressValue,
                            &nextObjmgmdRouterSrcListIfIndexValue,
                            & nextObjmgmdRouterSrcListHostAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListIfIndexValue, owa.len);
    inetCopy(&nextObjmgmdRouterSrcListAddressValue,&objmgmdRouterSrcListAddressValue);
    nextObjmgmdRouterSrcListIfIndexValue = objmgmdRouterSrcListIfIndexValue;
    inetAddressZeroSet(objmgmdRouterSrcListAddressTypeValue,&nextObjmgmdRouterSrcListHostAddressValue);
    do
    {
      owa.l7rc =
        usmDbMgmdSrcListEntryNextGet (L7_UNIT_CURRENT, objmgmdRouterSrcListAddressTypeValue,
                              &nextObjmgmdRouterSrcListAddressValue,
                              &nextObjmgmdRouterSrcListIfIndexValue,
                              &nextObjmgmdRouterSrcListHostAddressValue);
    }
    while ((owa.l7rc == L7_SUCCESS) &&
           (L7_INET_ADDR_COMPARE(&nextObjmgmdRouterSrcListAddressValue, &objmgmdRouterSrcListAddressValue) == 0) &&
           (objmgmdRouterSrcListIfIndexValue == nextObjmgmdRouterSrcListIfIndexValue));
    if (L7_INET_ADDR_COMPARE(&nextObjmgmdRouterSrcListAddressValue, &objmgmdRouterSrcListAddressValue) != 0)
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterSrcListIfIndexValue, owa.len);

  /* return the object value: mgmdRouterSrcListIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdRouterSrcListIfIndexValue,
                           sizeof (objmgmdRouterSrcListIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListHostAddress
*
* @purpose Get 'mgmdRouterSrcListHostAddress'
*
* @description [mgmdRouterSrcListHostAddress] The host address to which this entry corresponds. The mgmdRouterCacheSourceFilterMode value for this Group address and interface indicates whether this Host address is included or excluded.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListHostAddress (void *wap,
                                                                                 void *bufp)
{

  xLibU32_t objmgmdRouterSrcListAddressTypeValue;
  xLibU32_t objmgmdRouterSrcListIfIndexValue;
  xLibU32_t nextObjmgmdRouterSrcListIfIndexValue;
  L7_inet_addr_t objmgmdRouterSrcListAddressValue;
  L7_inet_addr_t nextObjmgmdRouterSrcListAddressValue;
  L7_inet_addr_t objmgmdRouterSrcListHostAddressValue;
  L7_inet_addr_t nextObjmgmdRouterSrcListHostAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  fpObjWa_t kwaobjmgmdRouterSrcListAddressValue = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  fpObjWa_t kwaobjmgmdRouterSrcListIfIndexValue = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaobjmgmdRouterSrcListAddressTypeValue = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  xLibU32_t objmgmdRouterSrcListGrpCompatModeValue = 0;

  /* retrieve key: mgmdRouterSrcListAddressType */
  kwaobjmgmdRouterSrcListAddressTypeValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddressType,
                          (xLibU8_t *) & objmgmdRouterSrcListAddressTypeValue, &kwaobjmgmdRouterSrcListAddressTypeValue.len);
  if (kwaobjmgmdRouterSrcListAddressTypeValue.rc != XLIBRC_SUCCESS)
  {
    kwaobjmgmdRouterSrcListAddressTypeValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaobjmgmdRouterSrcListAddressTypeValue);
    return kwaobjmgmdRouterSrcListAddressTypeValue.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListAddressTypeValue, kwaobjmgmdRouterSrcListAddressTypeValue.len);

  /* retrieve key: mgmdRouterSrcListIfIndex */
  kwaobjmgmdRouterSrcListIfIndexValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListIfIndex,
                          (xLibU8_t *) & objmgmdRouterSrcListIfIndexValue, &kwaobjmgmdRouterSrcListIfIndexValue.len);
  if (kwaobjmgmdRouterSrcListIfIndexValue.rc != XLIBRC_SUCCESS)
  {
    kwaobjmgmdRouterSrcListIfIndexValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaobjmgmdRouterSrcListIfIndexValue);
    return kwaobjmgmdRouterSrcListIfIndexValue.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListIfIndexValue, kwaobjmgmdRouterSrcListIfIndexValue.len);

  memset(&objmgmdRouterSrcListAddressValue, 0, sizeof(objmgmdRouterSrcListAddressValue));
  /* retrieve key: mgmdRouterSrcListAddress */
  kwaobjmgmdRouterSrcListAddressValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddress,
                          (xLibU8_t *)& objmgmdRouterSrcListAddressValue, &kwaobjmgmdRouterSrcListAddressValue.len);
  if (kwaobjmgmdRouterSrcListAddressValue.rc != XLIBRC_SUCCESS)
  {
    kwaobjmgmdRouterSrcListAddressValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaobjmgmdRouterSrcListAddressValue);
    return kwaobjmgmdRouterSrcListAddressValue.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListAddressValue, kwaobjmgmdRouterSrcListAddressValue.len);

  /* retrieve key: mgmdRouterSrcListHostAddress */
  memset(&objmgmdRouterSrcListHostAddressValue, 0, sizeof(objmgmdRouterSrcListHostAddressValue));
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListHostAddress,
                          (xLibU8_t *) &objmgmdRouterSrcListHostAddressValue, &owa.len);
  nextObjmgmdRouterSrcListIfIndexValue = objmgmdRouterSrcListIfIndexValue;


  owa.l7rc = usmDbMgmdCacheGroupCompatModeGet (L7_UNIT_CURRENT, 
                               objmgmdRouterSrcListAddressTypeValue,
                               &objmgmdRouterSrcListAddressValue,
                               objmgmdRouterSrcListIfIndexValue,
                               &objmgmdRouterSrcListGrpCompatModeValue);

  
  if (((objmgmdRouterSrcListAddressTypeValue == L7_AF_INET) &&
       (objmgmdRouterSrcListGrpCompatModeValue < L7_MGMD_VERSION_3)) ||
      ((objmgmdRouterSrcListAddressTypeValue == L7_AF_INET6) &&
       (objmgmdRouterSrcListGrpCompatModeValue < L7_MGMD_VERSION_2)))
   
  {
    if (owa.rc != XLIBRC_SUCCESS)
    {
      memset(&nextObjmgmdRouterSrcListHostAddressValue, 0, sizeof(nextObjmgmdRouterSrcListHostAddressValue));
      owa.l7rc = L7_SUCCESS;
 
      /* return the object value: mgmdRouterSrcListHostAddress */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdRouterSrcListHostAddressValue,
                            sizeof (objmgmdRouterSrcListHostAddressValue));
  
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else 
    {
      memset(&nextObjmgmdRouterSrcListHostAddressValue, 0, sizeof(nextObjmgmdRouterSrcListHostAddressValue));
      /* return the object value: mgmdRouterSrcListHostAddress */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdRouterSrcListHostAddressValue,
                            sizeof (objmgmdRouterSrcListHostAddressValue));

      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memcpy(&nextObjmgmdRouterSrcListAddressValue, &objmgmdRouterSrcListAddressValue, sizeof(nextObjmgmdRouterSrcListAddressValue));
    memset(&nextObjmgmdRouterSrcListHostAddressValue, 0, sizeof(objmgmdRouterSrcListHostAddressValue ));

    owa.l7rc =
      usmDbMgmdSrcListEntryNextGet (L7_UNIT_CURRENT, objmgmdRouterSrcListAddressTypeValue,
                            &nextObjmgmdRouterSrcListAddressValue,
                            &nextObjmgmdRouterSrcListIfIndexValue,
                            &nextObjmgmdRouterSrcListHostAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterSrcListHostAddressValue, owa.len);
    memcpy(&nextObjmgmdRouterSrcListAddressValue, &objmgmdRouterSrcListAddressValue, sizeof(nextObjmgmdRouterSrcListAddressValue));
    memcpy(&nextObjmgmdRouterSrcListHostAddressValue, &objmgmdRouterSrcListHostAddressValue, sizeof(nextObjmgmdRouterSrcListHostAddressValue));
    do
    {
    
    owa.l7rc =
      usmDbMgmdSrcListEntryNextGet (L7_UNIT_CURRENT, objmgmdRouterSrcListAddressTypeValue,
                            & nextObjmgmdRouterSrcListAddressValue,
                            &nextObjmgmdRouterSrcListIfIndexValue,
                            & nextObjmgmdRouterSrcListHostAddressValue);
    }
    while ((owa.l7rc == L7_SUCCESS) &&
           (L7_INET_ADDR_COMPARE(&nextObjmgmdRouterSrcListAddressValue, &objmgmdRouterSrcListAddressValue) == 0) &&
           (objmgmdRouterSrcListIfIndexValue == nextObjmgmdRouterSrcListIfIndexValue) &&
           (L7_INET_ADDR_COMPARE(&nextObjmgmdRouterSrcListHostAddressValue, &objmgmdRouterSrcListHostAddressValue) == 0));
    if (L7_INET_ADDR_COMPARE(&nextObjmgmdRouterSrcListAddressValue, &objmgmdRouterSrcListAddressValue) != 0 ||
        (objmgmdRouterSrcListIfIndexValue != nextObjmgmdRouterSrcListIfIndexValue) )
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

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterSrcListHostAddressValue, owa.len);

  /* return the object value: mgmdRouterSrcListHostAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdRouterSrcListHostAddressValue,
                           sizeof (objmgmdRouterSrcListHostAddressValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListExpiryTime
*
* @purpose Get 'mgmdRouterSrcListExpiryTime'
*
* @description [mgmdRouterSrcListExpiryTime] Router source list expiry time
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListExpiryTime (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t kwamgmdRouterSrcListAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterSrcListAddressTypeValue;
  fpObjWa_t kwamgmdRouterSrcListAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdRouterSrcListAddressValue;
  fpObjWa_t kwamgmdRouterSrcListIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterSrcListIfIndexValue;
  fpObjWa_t kwamgmdRouterSrcListHostAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdRouterSrcListHostAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterSrcListExpiryTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterSrcListAddressType */
  kwamgmdRouterSrcListAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddressType,
                   (xLibU8_t *) & keymgmdRouterSrcListAddressTypeValue,
                   &kwamgmdRouterSrcListAddressType.len);
  if (kwamgmdRouterSrcListAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterSrcListAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterSrcListAddressType);
    return kwamgmdRouterSrcListAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterSrcListAddressTypeValue,
                           kwamgmdRouterSrcListAddressType.len);

  /* retrieve key: mgmdRouterSrcListAddress */
  kwamgmdRouterSrcListAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListAddress,
                   (xLibU8_t *) keymgmdRouterSrcListAddressValue, &kwamgmdRouterSrcListAddress.len);
  if (kwamgmdRouterSrcListAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterSrcListAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterSrcListAddress);
    return kwamgmdRouterSrcListAddress.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdRouterSrcListAddressValue, kwamgmdRouterSrcListAddress.len);

  /* retrieve key: mgmdRouterSrcListIfIndex */
  kwamgmdRouterSrcListIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListIfIndex,
                   (xLibU8_t *) & keymgmdRouterSrcListIfIndexValue,
                   &kwamgmdRouterSrcListIfIndex.len);
  if (kwamgmdRouterSrcListIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterSrcListIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterSrcListIfIndex);
    return kwamgmdRouterSrcListIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterSrcListIfIndexValue,
                           kwamgmdRouterSrcListIfIndex.len);

  /* retrieve key: mgmdRouterSrcListHostAddress */
  kwamgmdRouterSrcListHostAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterSourceListTable_mgmdRouterSrcListHostAddress,
                   (xLibU8_t *) keymgmdRouterSrcListHostAddressValue,
                   &kwamgmdRouterSrcListHostAddress.len);
  if (kwamgmdRouterSrcListHostAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterSrcListHostAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterSrcListHostAddress);
    return kwamgmdRouterSrcListHostAddress.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdRouterSrcListHostAddressValue,
                           kwamgmdRouterSrcListHostAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdSrcExpiryTimeGet (L7_UNIT_CURRENT, keymgmdRouterSrcListAddressTypeValue,
                              (L7_inet_addr_t *) keymgmdRouterSrcListAddressValue,
                              keymgmdRouterSrcListIfIndexValue,
                              (L7_inet_addr_t *) keymgmdRouterSrcListHostAddressValue,
                              &objmgmdRouterSrcListExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterSrcListExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterSrcListExpiryTimeValue,
                           sizeof (objmgmdRouterSrcListExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

