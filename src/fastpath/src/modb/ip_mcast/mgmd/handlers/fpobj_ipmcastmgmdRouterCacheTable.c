
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdRouterCacheTable.c
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
#include "_xe_ipmcastmgmdRouterCacheTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"
#include "usmdb_mib_igmp_api.h"


/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType
*
* @purpose Get 'mgmdRouterCacheAddressType'
*
* @description [mgmdRouterCacheAddressType] The address type of the mgmdRouterCacheTable entry. This value applies to both the mgmdRouterCacheAddress and the mgmdRouterCacheLastReporter entries.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType (void *wap, void *bufp)
{

  xLibU32_t objmgmdRouterCacheAddressTypeValue;
  xLibU32_t nextObjmgmdRouterCacheAddressTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                          (xLibU8_t *) & objmgmdRouterCacheAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdRouterCacheAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.rc = XLIBRC_SUCCESS;
  }
  else if (objmgmdRouterCacheAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterCacheAddressTypeValue, owa.len);
    nextObjmgmdRouterCacheAddressTypeValue = L7_INET_ADDR_TYPE_IPV6;
    owa.rc = XLIBRC_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterCacheAddressTypeValue, owa.len);
    owa.rc = XLIBRC_FAILURE;
  }

  if ((owa.rc != XLIBRC_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterCacheAddressTypeValue, owa.len);

  /* return the object value: mgmdRouterCacheAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdRouterCacheAddressTypeValue,
                           sizeof (objmgmdRouterCacheAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress
*
* @purpose Get 'mgmdRouterCacheAddress'
*
* @description [mgmdRouterCacheAddress] The IP multicast group address for which this entry contains information. The InetAddressType, e.g. IPv4 or IPv6, is identified by the mgmdRouterCacheAddressType variable in the mgmdRouterCache table.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress (void *wap, void *bufp)
{

  xLibU32_t objmgmdRouterCacheAddressTypeValue;
  xLibU32_t nextObjmgmdRouterCacheIfIndexValue;
  L7_inet_addr_t objmgmdRouterCacheAddressValue;
  L7_inet_addr_t nextObjmgmdRouterCacheAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  fpObjWa_t kwaobjmgmdRouterCacheAddressTypeValue = FPOBJ_INIT_WA(sizeof(xLibU32_t));  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  kwaobjmgmdRouterCacheAddressTypeValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                          (xLibU8_t *) & objmgmdRouterCacheAddressTypeValue, &kwaobjmgmdRouterCacheAddressTypeValue.len);
  if (kwaobjmgmdRouterCacheAddressTypeValue.rc != XLIBRC_SUCCESS)
  {
    kwaobjmgmdRouterCacheAddressTypeValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaobjmgmdRouterCacheAddressTypeValue);
    return kwaobjmgmdRouterCacheAddressTypeValue.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterCacheAddressTypeValue, kwaobjmgmdRouterCacheAddressTypeValue.len);

  /* retrieve key: mgmdRouterCacheAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress,
                          (xLibU8_t *) &objmgmdRouterCacheAddressValue, &owa.len);

  nextObjmgmdRouterCacheIfIndexValue = 0;

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(objmgmdRouterCacheAddressTypeValue, &nextObjmgmdRouterCacheAddressValue);
    owa.l7rc = usmDbMgmdCacheEntryNextGet (L7_UNIT_CURRENT,
                                       objmgmdRouterCacheAddressTypeValue,
                                       &nextObjmgmdRouterCacheAddressValue,                                     
                                       &nextObjmgmdRouterCacheIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterCacheAddressValue, owa.len);
    inetCopy(&nextObjmgmdRouterCacheAddressValue, &objmgmdRouterCacheAddressValue);
    do
    {
       owa.l7rc = usmDbMgmdCacheEntryNextGet (L7_UNIT_CURRENT,
                                       objmgmdRouterCacheAddressTypeValue,
                                       &nextObjmgmdRouterCacheAddressValue,                                     
                                       &nextObjmgmdRouterCacheIfIndexValue);
    }while((L7_INET_ADDR_COMPARE(&nextObjmgmdRouterCacheAddressValue, &objmgmdRouterCacheAddressValue) ==0)
           &&(owa.l7rc == L7_SUCCESS));
  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterCacheAddressValue, sizeof(nextObjmgmdRouterCacheAddressValue));

  /* return the object value: mgmdRouterCacheAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdRouterCacheAddressValue,
                           sizeof(nextObjmgmdRouterCacheAddressValue) );
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex
*
* @purpose Get 'mgmdRouterCacheIfIndex'
*
* @description [mgmdRouterCacheIfIndex] The interface for which this entry contains information for an IP multicast group address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex (void *wap, void *bufp)
{
  xLibU32_t objmgmdRouterCacheAddressTypeValue;
  xLibU32_t objmgmdRouterCacheIfIndexValue;
  xLibU32_t nextObjmgmdRouterCacheIfIndexValue;
  L7_inet_addr_t objmgmdRouterCacheAddressValue;
  L7_inet_addr_t nextObjmgmdRouterCacheAddressValue;


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaobjmgmdRouterCacheAddressTypeValue = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaobjmgmdRouterCacheAddressValue = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  kwaobjmgmdRouterCacheAddressTypeValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                          (xLibU8_t *) & objmgmdRouterCacheAddressTypeValue, &kwaobjmgmdRouterCacheAddressTypeValue.len);
  if (kwaobjmgmdRouterCacheAddressTypeValue.rc != XLIBRC_SUCCESS)
  {
    kwaobjmgmdRouterCacheAddressTypeValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaobjmgmdRouterCacheAddressTypeValue);
    return kwaobjmgmdRouterCacheAddressTypeValue.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterCacheAddressTypeValue, kwaobjmgmdRouterCacheAddressTypeValue.len);

  inetAddressZeroSet( objmgmdRouterCacheAddressTypeValue, &objmgmdRouterCacheAddressValue);
  /* retrieve key: mgmdRouterCacheAddress */
  kwaobjmgmdRouterCacheAddressValue.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress,
                          (xLibU8_t *) & objmgmdRouterCacheAddressValue, &kwaobjmgmdRouterCacheAddressValue.len);
  if (kwaobjmgmdRouterCacheAddressValue.rc != XLIBRC_SUCCESS)
  {
    kwaobjmgmdRouterCacheAddressValue.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaobjmgmdRouterCacheAddressValue);
    return kwaobjmgmdRouterCacheAddressValue.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterCacheAddressValue, kwaobjmgmdRouterCacheAddressValue.len);



  /* retrieve key: mgmdRouterCacheIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex,
                          (xLibU8_t *) & objmgmdRouterCacheIfIndexValue, &owa.len);


  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdRouterCacheIfIndexValue = 0;
    inetCopy(&nextObjmgmdRouterCacheAddressValue, &objmgmdRouterCacheAddressValue);
    owa.l7rc = usmDbMgmdCacheEntryNextGet (L7_UNIT_CURRENT,
                                     objmgmdRouterCacheAddressTypeValue,
                                     &nextObjmgmdRouterCacheAddressValue,                                     
                                     &nextObjmgmdRouterCacheIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdRouterCacheIfIndexValue, owa.len);
    nextObjmgmdRouterCacheIfIndexValue = objmgmdRouterCacheIfIndexValue;
    inetCopy(&nextObjmgmdRouterCacheAddressValue, &objmgmdRouterCacheAddressValue);
    do 
    {
       owa.l7rc = usmDbMgmdCacheEntryNextGet(L7_UNIT_CURRENT, objmgmdRouterCacheAddressTypeValue,
                                             &nextObjmgmdRouterCacheAddressValue,
                                             &nextObjmgmdRouterCacheIfIndexValue);
    }while ((owa.l7rc == L7_SUCCESS) &&
            (L7_INET_ADDR_COMPARE(&nextObjmgmdRouterCacheAddressValue, &objmgmdRouterCacheAddressValue) == 0) &&
            (nextObjmgmdRouterCacheIfIndexValue == objmgmdRouterCacheIfIndexValue));

    if(L7_INET_ADDR_COMPARE(&nextObjmgmdRouterCacheAddressValue, &objmgmdRouterCacheAddressValue) != 0)
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
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdRouterCacheIfIndexValue, sizeof(nextObjmgmdRouterCacheIfIndexValue));

  /* return the object value: mgmdRouterCacheIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdRouterCacheIfIndexValue,
                           sizeof (nextObjmgmdRouterCacheIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheLastReporter
*
* @purpose Get 'mgmdRouterCacheLastReporter'
*
* @description [mgmdRouterCacheLastReporter] The IP address of the source of the last membership report received for this IP Multicast group address on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheLastReporter (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheAddressTypeValue;
  fpObjWa_t kwamgmdRouterCacheAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keymgmdRouterCacheAddressValue;
  fpObjWa_t kwamgmdRouterCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objmgmdRouterCacheLastReporterValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  kwamgmdRouterCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                   (xLibU8_t *) & keymgmdRouterCacheAddressTypeValue,
                   &kwamgmdRouterCacheAddressType.len);
  if (kwamgmdRouterCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddressType);
    return kwamgmdRouterCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheAddressTypeValue,
                           kwamgmdRouterCacheAddressType.len);

  /* retrieve key: mgmdRouterCacheAddress */
  kwamgmdRouterCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress,
                   (xLibU8_t *) &keymgmdRouterCacheAddressValue, &kwamgmdRouterCacheAddress.len);
  if (kwamgmdRouterCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddress);
    return kwamgmdRouterCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheAddressValue, kwamgmdRouterCacheAddress.len);

  /* retrieve key: mgmdRouterCacheIfIndex */
  kwamgmdRouterCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex,
                   (xLibU8_t *) & keymgmdRouterCacheIfIndexValue, &kwamgmdRouterCacheIfIndex.len);
  if (kwamgmdRouterCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheIfIndex);
    return kwamgmdRouterCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheIfIndexValue, kwamgmdRouterCacheIfIndex.len);


  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheLastReporterGet (L7_UNIT_CURRENT, keymgmdRouterCacheAddressTypeValue,
                              &keymgmdRouterCacheAddressValue,
                              keymgmdRouterCacheIfIndexValue, &objmgmdRouterCacheLastReporterValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterCacheLastReporter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)& objmgmdRouterCacheLastReporterValue,
                           sizeof (objmgmdRouterCacheLastReporterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheUpTime
*
* @purpose Get 'mgmdRouterCacheUpTime'
*
* @description [mgmdRouterCacheUpTime] The time elapsed since this entry was created.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheAddressTypeValue;
  fpObjWa_t kwamgmdRouterCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdRouterCacheAddressValue;
  fpObjWa_t kwamgmdRouterCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterCacheUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  kwamgmdRouterCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                   (xLibU8_t *) & keymgmdRouterCacheAddressTypeValue,
                   &kwamgmdRouterCacheAddressType.len);
  if (kwamgmdRouterCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddressType);
    return kwamgmdRouterCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheAddressTypeValue,
                           kwamgmdRouterCacheAddressType.len);

  /* retrieve key: mgmdRouterCacheAddress */
  kwamgmdRouterCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress,
                   (xLibU8_t *) keymgmdRouterCacheAddressValue, &kwamgmdRouterCacheAddress.len);
  if (kwamgmdRouterCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddress);
    return kwamgmdRouterCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdRouterCacheAddressValue, kwamgmdRouterCacheAddress.len);

  /* retrieve key: mgmdRouterCacheIfIndex */
  kwamgmdRouterCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex,
                   (xLibU8_t *) & keymgmdRouterCacheIfIndexValue, &kwamgmdRouterCacheIfIndex.len);
  if (kwamgmdRouterCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheIfIndex);
    return kwamgmdRouterCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheIfIndexValue, kwamgmdRouterCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheUpTimeGet (L7_UNIT_CURRENT, keymgmdRouterCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdRouterCacheAddressValue,
                              keymgmdRouterCacheIfIndexValue, &objmgmdRouterCacheUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterCacheUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterCacheUpTimeValue,
                           sizeof (objmgmdRouterCacheUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheExpiryTime
*
* @purpose Get 'mgmdRouterCacheExpiryTime'
*
* @description [mgmdRouterCacheExpiryTime] The minimum amount of time remaining before this entry will be aged out. The value must always be greater than 0.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheExpiryTime (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheAddressTypeValue;
  fpObjWa_t kwamgmdRouterCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdRouterCacheAddressValue;
  fpObjWa_t kwamgmdRouterCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterCacheExpiryTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  kwamgmdRouterCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                   (xLibU8_t *) & keymgmdRouterCacheAddressTypeValue,
                   &kwamgmdRouterCacheAddressType.len);
  if (kwamgmdRouterCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddressType);
    return kwamgmdRouterCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheAddressTypeValue,
                           kwamgmdRouterCacheAddressType.len);

  /* retrieve key: mgmdRouterCacheAddress */
  kwamgmdRouterCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress,
                   (xLibU8_t *) keymgmdRouterCacheAddressValue, &kwamgmdRouterCacheAddress.len);
  if (kwamgmdRouterCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddress);
    return kwamgmdRouterCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdRouterCacheAddressValue, kwamgmdRouterCacheAddress.len);

  /* retrieve key: mgmdRouterCacheIfIndex */
  kwamgmdRouterCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex,
                   (xLibU8_t *) & keymgmdRouterCacheIfIndexValue, &kwamgmdRouterCacheIfIndex.len);
  if (kwamgmdRouterCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheIfIndex);
    return kwamgmdRouterCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheIfIndexValue, kwamgmdRouterCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheExpiryTimeGet (L7_UNIT_CURRENT, keymgmdRouterCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdRouterCacheAddressValue,
                              keymgmdRouterCacheIfIndexValue, &objmgmdRouterCacheExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterCacheExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterCacheExpiryTimeValue,
                           sizeof (objmgmdRouterCacheExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheVersion1HostTimer
*
* @purpose Get 'mgmdRouterCacheVersion1HostTimer'
*
* @description [mgmdRouterCacheVersion1HostTimer] The time remaining until the local router will assume that there are no longer any MGMD version 1 members on the IP subnet attached to this interface. This entry only applies to IGMPv1 hosts, and is not implemented for MLD
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheVersion1HostTimer (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t kwamgmdRouterCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheAddressTypeValue;
  fpObjWa_t kwamgmdRouterCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdRouterCacheAddressValue;
  fpObjWa_t kwamgmdRouterCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterCacheVersion1HostTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  kwamgmdRouterCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                   (xLibU8_t *) & keymgmdRouterCacheAddressTypeValue,
                   &kwamgmdRouterCacheAddressType.len);
  if (kwamgmdRouterCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddressType);
    return kwamgmdRouterCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheAddressTypeValue,
                           kwamgmdRouterCacheAddressType.len);

  /* retrieve key: mgmdRouterCacheAddress */
  kwamgmdRouterCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress,
                   (xLibU8_t *) keymgmdRouterCacheAddressValue, &kwamgmdRouterCacheAddress.len);
  if (kwamgmdRouterCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddress);
    return kwamgmdRouterCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdRouterCacheAddressValue, kwamgmdRouterCacheAddress.len);

  /* retrieve key: mgmdRouterCacheIfIndex */
  kwamgmdRouterCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex,
                   (xLibU8_t *) & keymgmdRouterCacheIfIndexValue, &kwamgmdRouterCacheIfIndex.len);
  if (kwamgmdRouterCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheIfIndex);
    return kwamgmdRouterCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheIfIndexValue, kwamgmdRouterCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheVersion1HostTimerGet (L7_UNIT_CURRENT, keymgmdRouterCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdRouterCacheAddressValue,
                              keymgmdRouterCacheIfIndexValue,
                              &objmgmdRouterCacheVersion1HostTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterCacheVersion1HostTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterCacheVersion1HostTimerValue,
                           sizeof (objmgmdRouterCacheVersion1HostTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheVersion2HostTimer
*
* @purpose Get 'mgmdRouterCacheVersion2HostTimer'
*
* @description [mgmdRouterCacheVersion2HostTimer] The time remaining until the local router will assume that there are no longer any MGMD version 2 members on the IP subnet attached to this interface. This entry applies to both IGMP and MLD hosts.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheVersion2HostTimer (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t kwamgmdRouterCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheAddressTypeValue;
  fpObjWa_t kwamgmdRouterCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdRouterCacheAddressValue;
  fpObjWa_t kwamgmdRouterCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterCacheVersion2HostTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  kwamgmdRouterCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                   (xLibU8_t *) & keymgmdRouterCacheAddressTypeValue,
                   &kwamgmdRouterCacheAddressType.len);
  if (kwamgmdRouterCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddressType);
    return kwamgmdRouterCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheAddressTypeValue,
                           kwamgmdRouterCacheAddressType.len);

  /* retrieve key: mgmdRouterCacheAddress */
  kwamgmdRouterCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress,
                   (xLibU8_t *) keymgmdRouterCacheAddressValue, &kwamgmdRouterCacheAddress.len);
  if (kwamgmdRouterCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddress);
    return kwamgmdRouterCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdRouterCacheAddressValue, kwamgmdRouterCacheAddress.len);

  /* retrieve key: mgmdRouterCacheIfIndex */
  kwamgmdRouterCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex,
                   (xLibU8_t *) & keymgmdRouterCacheIfIndexValue, &kwamgmdRouterCacheIfIndex.len);
  if (kwamgmdRouterCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheIfIndex);
    return kwamgmdRouterCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheIfIndexValue, kwamgmdRouterCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheVersion2HostTimerGet (L7_UNIT_CURRENT, keymgmdRouterCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdRouterCacheAddressValue,
                              keymgmdRouterCacheIfIndexValue,
                              &objmgmdRouterCacheVersion2HostTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterCacheVersion2HostTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterCacheVersion2HostTimerValue,
                           sizeof (objmgmdRouterCacheVersion2HostTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheSourceFilterMode
*
* @purpose Get 'mgmdRouterCacheSourceFilterMode'
*
* @description [mgmdRouterCacheSourceFilterMode] The state in which the interface is currently set. The value indicates the relevance of the corresponding source list entries in the RouterSrcList Table for MGMDv3 interfaces.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheSourceFilterMode (void *wap,
                                                                               void *bufp)
{

  fpObjWa_t kwamgmdRouterCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheAddressTypeValue;
  fpObjWa_t kwamgmdRouterCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdRouterCacheAddressValue;
  fpObjWa_t kwamgmdRouterCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterCacheSourceFilterModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  kwamgmdRouterCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                   (xLibU8_t *) & keymgmdRouterCacheAddressTypeValue,
                   &kwamgmdRouterCacheAddressType.len);
  if (kwamgmdRouterCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddressType);
    return kwamgmdRouterCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheAddressTypeValue,
                           kwamgmdRouterCacheAddressType.len);

  /* retrieve key: mgmdRouterCacheAddress */
  kwamgmdRouterCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress,
                   (xLibU8_t *) keymgmdRouterCacheAddressValue, &kwamgmdRouterCacheAddress.len);
  if (kwamgmdRouterCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddress);
    return kwamgmdRouterCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdRouterCacheAddressValue, kwamgmdRouterCacheAddress.len);

  /* retrieve key: mgmdRouterCacheIfIndex */
  kwamgmdRouterCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex,
                   (xLibU8_t *) & keymgmdRouterCacheIfIndexValue, &kwamgmdRouterCacheIfIndex.len);
  if (kwamgmdRouterCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheIfIndex);
    return kwamgmdRouterCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheIfIndexValue, kwamgmdRouterCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheGroupSourceFilterModeGet (L7_UNIT_CURRENT, keymgmdRouterCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdRouterCacheAddressValue,
                              keymgmdRouterCacheIfIndexValue,
                              &objmgmdRouterCacheSourceFilterModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterCacheSourceFilterMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterCacheSourceFilterModeValue,
                           sizeof (objmgmdRouterCacheSourceFilterModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheGroupCompatMode
*
* @purpose Get 'mgmdRouterCacheGroupCompatMode'
*
* @description [mgmdRouterCacheGroupCompatMode] The compatibility mode (v1, v2 or v3) for the specified group on the specified interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdRouterCacheTable_mgmdRouterCacheGroupCompatMode (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdRouterCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheAddressTypeValue;
  fpObjWa_t kwamgmdRouterCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdRouterCacheAddressValue;
  fpObjWa_t kwamgmdRouterCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdRouterCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdRouterCacheGroupCompatModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdRouterCacheAddressType */
  kwamgmdRouterCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddressType,
                   (xLibU8_t *) & keymgmdRouterCacheAddressTypeValue,
                   &kwamgmdRouterCacheAddressType.len);
  if (kwamgmdRouterCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddressType);
    return kwamgmdRouterCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheAddressTypeValue,
                           kwamgmdRouterCacheAddressType.len);

  /* retrieve key: mgmdRouterCacheAddress */
  kwamgmdRouterCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheAddress,
                   (xLibU8_t *) keymgmdRouterCacheAddressValue, &kwamgmdRouterCacheAddress.len);
  if (kwamgmdRouterCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheAddress);
    return kwamgmdRouterCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdRouterCacheAddressValue, kwamgmdRouterCacheAddress.len);

  /* retrieve key: mgmdRouterCacheIfIndex */
  kwamgmdRouterCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdRouterCacheTable_mgmdRouterCacheIfIndex,
                   (xLibU8_t *) & keymgmdRouterCacheIfIndexValue, &kwamgmdRouterCacheIfIndex.len);
  if (kwamgmdRouterCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdRouterCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdRouterCacheIfIndex);
    return kwamgmdRouterCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdRouterCacheIfIndexValue, kwamgmdRouterCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheGroupCompatModeGet (L7_UNIT_CURRENT, keymgmdRouterCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdRouterCacheAddressValue,
                              keymgmdRouterCacheIfIndexValue,
                              &objmgmdRouterCacheGroupCompatModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdRouterCacheGroupCompatMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdRouterCacheGroupCompatModeValue,
                           sizeof (objmgmdRouterCacheGroupCompatModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
