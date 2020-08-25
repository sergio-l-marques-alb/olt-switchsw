
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdHostCacheTable.c
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
#include "_xe_ipmcastmgmdHostCacheTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_igmp_api.h"
#include "usmdb_mib_igmp_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType
*
* @purpose Get 'mgmdHostCacheAddressType'
*
* @description [mgmdHostCacheAddressType] The address type of the mgmdHostCacheTable entry. This value applies to both the mgmdHostCacheAddress and the mgmdHostCacheLastReporter entries.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType (void *wap, void *bufp)
{
  xLibU32_t objmgmdHostCacheAddressTypeValue;
  xLibU32_t nextObjmgmdHostCacheAddressTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostCacheAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType,
                          (xLibU8_t *) & objmgmdHostCacheAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdHostCacheAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  
  else if (objmgmdHostCacheAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &nextObjmgmdHostCacheAddressTypeValue, owa.len);
    nextObjmgmdHostCacheAddressTypeValue = L7_INET_ADDR_TYPE_IPV6;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostCacheAddressTypeValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostCacheAddressTypeValue, owa.len);

  /* return the object value: mgmdHostCacheAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdHostCacheAddressTypeValue,
                           sizeof (nextObjmgmdHostCacheAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheAddress
*
* @purpose Get 'mgmdHostCacheAddress'
*
* @description [mgmdHostCacheAddress] The IP multicast group address for which this entry contains information. The InetAddressType, e.g. IPv4 or IPv6, is identified by the mgmdHostCacheAddressType.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheAddress (void *wap, void *bufp)
{

  xLibU32_t objmgmdHostCacheAddressTypeValue;
  xLibU32_t objmgmdHostCacheIfIndexValue;
  xLibU32_t nextObjmgmdHostCacheIfIndexValue;
  L7_inet_addr_t objmgmdHostCacheAddressValue;
  L7_inet_addr_t nextObjmgmdHostCacheAddressValue;
  L7_inet_addr_t inetTempIP;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  fpObjWa_t kwaType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwaIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: mgmdHostCacheAddressType */
  kwaType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType,
                          (xLibU8_t *) & objmgmdHostCacheAddressTypeValue, &kwaType.len);
  if (kwaType.rc != XLIBRC_SUCCESS)
  {
    kwaType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaType);
    return kwaType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostCacheAddressTypeValue, kwaType.len);

  /* retrieve key: mgmdHostCacheIfIndex */
  kwaIndex.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex,
                          (xLibU8_t *) & objmgmdHostCacheIfIndexValue, &kwaIndex.len);
  if (kwaIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIndex);
    return kwaIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostCacheIfIndexValue, kwaIndex.len);

  if (usmDbMgmdProxyInterfaceOperationalStateGet (L7_UNIT_CURRENT,
                                     objmgmdHostCacheAddressTypeValue,
                                     objmgmdHostCacheIfIndexValue) != L7_TRUE)
  {
    owa.rc = XLIBRC_MGMD_PROXY_NOT_OPERATIONAL;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 

   
  inetAddressZeroSet(objmgmdHostCacheAddressTypeValue, &nextObjmgmdHostCacheAddressValue);
  inetAddressZeroSet(objmgmdHostCacheAddressTypeValue, &objmgmdHostCacheAddressValue);

  nextObjmgmdHostCacheIfIndexValue = objmgmdHostCacheIfIndexValue;

  /* retrieve key: mgmdHostCacheAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddress,
                          (xLibU8_t *) &objmgmdHostCacheAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(objmgmdHostCacheAddressTypeValue, &inetTempIP);
    owa.l7rc = usmDbMgmdHostCacheEntryNextGet (L7_UNIT_CURRENT,
                                       objmgmdHostCacheAddressTypeValue,
                                       &inetTempIP,
                                       &nextObjmgmdHostCacheIfIndexValue);
    inetCopy(&nextObjmgmdHostCacheAddressValue, &inetTempIP);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostCacheAddressValue, owa.len);
    inetCopy(&inetTempIP, &objmgmdHostCacheAddressValue);
    owa.l7rc = usmDbMgmdHostCacheEntryNextGet (L7_UNIT_CURRENT,
                                       objmgmdHostCacheAddressTypeValue,
                                       &inetTempIP,
                                       &nextObjmgmdHostCacheIfIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  inetCopy(&nextObjmgmdHostCacheAddressValue, &inetTempIP);

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostCacheAddressValue, owa.len);

  /* return the object value: mgmdHostCacheAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdHostCacheAddressValue,
                           sizeof (nextObjmgmdHostCacheAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex
*
* @purpose Get 'mgmdHostCacheIfIndex'
*
* @description [mgmdHostCacheIfIndex] The interface for which this entry contains information for an IP multicast group address.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex (void *wap, void *bufp)
{

  xLibU32_t objmgmdHostCacheAddressTypeValue;
  xLibU32_t objmgmdHostCacheIfIndexValue;
  xLibU32_t nextObjmgmdHostCacheIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostCacheAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType,
                          (xLibU8_t *) & objmgmdHostCacheAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdHostCacheAddressTypeValue, owa.len);

  /* retrieve key: mgmdHostCacheIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex,
                          (xLibU8_t *) & objmgmdHostCacheIfIndexValue, &owa.len);

  if (owa.rc == XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  owa.l7rc = usmDbMgmdProxyInterfaceGet(L7_UNIT_CURRENT,
                                   objmgmdHostCacheAddressTypeValue,
                                   &nextObjmgmdHostCacheIfIndexValue);


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_MGMD_PROXY_NOT_ENBLD;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdHostCacheIfIndexValue, owa.len);

  /* return the object value: mgmdHostCacheIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdHostCacheIfIndexValue,
                           sizeof (objmgmdHostCacheIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheUpTime
*
* @purpose Get 'mgmdHostCacheUpTime'
*
* @description [mgmdHostCacheUpTime] The time elapsed since this entry was created.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheAddressTypeValue;
  fpObjWa_t kwamgmdHostCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdHostCacheAddressValue;
  fpObjWa_t kwamgmdHostCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostCacheUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostCacheAddressType */
  kwamgmdHostCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType,
                   (xLibU8_t *) & keymgmdHostCacheAddressTypeValue,
                   &kwamgmdHostCacheAddressType.len);
  if (kwamgmdHostCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddressType);
    return kwamgmdHostCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheAddressTypeValue,
                           kwamgmdHostCacheAddressType.len);

  /* retrieve key: mgmdHostCacheAddress */
  kwamgmdHostCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddress,
                   (xLibU8_t *) keymgmdHostCacheAddressValue, &kwamgmdHostCacheAddress.len);
  if (kwamgmdHostCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddress);
    return kwamgmdHostCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdHostCacheAddressValue, kwamgmdHostCacheAddress.len);

  /* retrieve key: mgmdHostCacheIfIndex */
  kwamgmdHostCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex,
                   (xLibU8_t *) & keymgmdHostCacheIfIndexValue, &kwamgmdHostCacheIfIndex.len);
  if (kwamgmdHostCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheIfIndex);
    return kwamgmdHostCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheIfIndexValue, kwamgmdHostCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostCacheUpTimeGet (L7_UNIT_CURRENT, 
                              keymgmdHostCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdHostCacheAddressValue,
                              keymgmdHostCacheIfIndexValue, &objmgmdHostCacheUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostCacheUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostCacheUpTimeValue,
                           sizeof (objmgmdHostCacheUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheLastReporter
*
* @purpose Get 'mgmdHostCacheLastReporter'
*
* @description [mgmdHostCacheLastReporter] The IP address of the source of the last membership report received for this IP Multicast group address on this interface.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheLastReporter (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheAddressTypeValue;
  fpObjWa_t kwamgmdHostCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdHostCacheAddressValue;
  fpObjWa_t kwamgmdHostCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_inet_addr_t objmgmdHostCacheLastReporterValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostCacheAddressType */
  kwamgmdHostCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType,
                   (xLibU8_t *) & keymgmdHostCacheAddressTypeValue,
                   &kwamgmdHostCacheAddressType.len);
  if (kwamgmdHostCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddressType);
    return kwamgmdHostCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheAddressTypeValue,
                           kwamgmdHostCacheAddressType.len);

  /* retrieve key: mgmdHostCacheAddress */
  kwamgmdHostCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddress,
                   (xLibU8_t *) keymgmdHostCacheAddressValue, &kwamgmdHostCacheAddress.len);
  if (kwamgmdHostCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddress);
    return kwamgmdHostCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdHostCacheAddressValue, kwamgmdHostCacheAddress.len);

  /* retrieve key: mgmdHostCacheIfIndex */
  kwamgmdHostCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex,
                   (xLibU8_t *) & keymgmdHostCacheIfIndexValue, &kwamgmdHostCacheIfIndex.len);
  if (kwamgmdHostCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheIfIndex);
    return kwamgmdHostCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheIfIndexValue, kwamgmdHostCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostCacheLastReporterGet (L7_UNIT_CURRENT, 
                              keymgmdHostCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdHostCacheAddressValue,
                              keymgmdHostCacheIfIndexValue, (L7_inet_addr_t *)&objmgmdHostCacheLastReporterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostCacheLastReporter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objmgmdHostCacheLastReporterValue,
                           sizeof (objmgmdHostCacheLastReporterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheSourceFilterMode
*
* @purpose Get 'mgmdHostCacheSourceFilterMode'
*
* @description [mgmdHostCacheSourceFilterMode] The state in which the interface is currently set. The value indicates the relevance of the corresponding source list entries in the HostSrcList Table for MGMDv3 interfaces.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostCacheSourceFilterMode (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheAddressTypeValue;
  fpObjWa_t kwamgmdHostCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdHostCacheAddressValue;
  fpObjWa_t kwamgmdHostCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostCacheSourceFilterModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostCacheAddressType */
  kwamgmdHostCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType,
                   (xLibU8_t *) & keymgmdHostCacheAddressTypeValue,
                   &kwamgmdHostCacheAddressType.len);
  if (kwamgmdHostCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddressType);
    return kwamgmdHostCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheAddressTypeValue,
                           kwamgmdHostCacheAddressType.len);

  /* retrieve key: mgmdHostCacheAddress */
  kwamgmdHostCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddress,
                   (xLibU8_t *) keymgmdHostCacheAddressValue, &kwamgmdHostCacheAddress.len);
  if (kwamgmdHostCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddress);
    return kwamgmdHostCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdHostCacheAddressValue, kwamgmdHostCacheAddress.len);

  /* retrieve key: mgmdHostCacheIfIndex */
  kwamgmdHostCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex,
                   (xLibU8_t *) & keymgmdHostCacheIfIndexValue, &kwamgmdHostCacheIfIndex.len);
  if (kwamgmdHostCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheIfIndex);
    return kwamgmdHostCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheIfIndexValue, kwamgmdHostCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostCacheGroupSourceFilterModeGet (L7_UNIT_CURRENT, 
                              keymgmdHostCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdHostCacheAddressValue,
                              keymgmdHostCacheIfIndexValue, &objmgmdHostCacheSourceFilterModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostCacheSourceFilterMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostCacheSourceFilterModeValue,
                           sizeof (objmgmdHostCacheSourceFilterModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostInterfaceSrcCount
*
* @purpose Get 'mgmdHostInterfaceSrcCount'
*
* @description [mgmdHostInterfaceSrcCount] Number of source records
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostInterfaceSrcCount (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheAddressTypeValue;
  fpObjWa_t kwamgmdHostCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdHostCacheAddressValue;
  fpObjWa_t kwamgmdHostCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostInterfaceSrcCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostCacheAddressType */
  kwamgmdHostCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType,
                   (xLibU8_t *) & keymgmdHostCacheAddressTypeValue,
                   &kwamgmdHostCacheAddressType.len);
  if (kwamgmdHostCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddressType);
    return kwamgmdHostCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheAddressTypeValue,
                           kwamgmdHostCacheAddressType.len);

  /* retrieve key: mgmdHostCacheAddress */
  kwamgmdHostCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddress,
                   (xLibU8_t *) keymgmdHostCacheAddressValue, &kwamgmdHostCacheAddress.len);
  if (kwamgmdHostCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddress);
    return kwamgmdHostCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdHostCacheAddressValue, kwamgmdHostCacheAddress.len);

  /* retrieve key: mgmdHostCacheIfIndex */
  kwamgmdHostCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex,
                   (xLibU8_t *) & keymgmdHostCacheIfIndexValue, &kwamgmdHostCacheIfIndex.len);
  if (kwamgmdHostCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheIfIndex);
    return kwamgmdHostCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheIfIndexValue, kwamgmdHostCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostInterfaceSrcsGet (L7_UNIT_CURRENT, keymgmdHostCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdHostCacheAddressValue,
                              keymgmdHostCacheIfIndexValue, &objmgmdHostInterfaceSrcCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostInterfaceSrcCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostInterfaceSrcCountValue,
                           sizeof (objmgmdHostInterfaceSrcCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostStatus
*
* @purpose Get 'mgmdHostStatus'
*
* @description [mgmdHostStatus] Mgmd Host Status
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostStatus (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheAddressTypeValue;
  fpObjWa_t kwamgmdHostCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keymgmdHostCacheAddressValue;
  fpObjWa_t kwamgmdHostCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostCacheAddressType */
  kwamgmdHostCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType,
                   (xLibU8_t *) & keymgmdHostCacheAddressTypeValue,
                   &kwamgmdHostCacheAddressType.len);
  if (kwamgmdHostCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddressType);
    return kwamgmdHostCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheAddressTypeValue,
                           kwamgmdHostCacheAddressType.len);

  /* retrieve key: mgmdHostCacheAddress */
  kwamgmdHostCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddress,
                   (xLibU8_t *) keymgmdHostCacheAddressValue, &kwamgmdHostCacheAddress.len);
  if (kwamgmdHostCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddress);
    return kwamgmdHostCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymgmdHostCacheAddressValue, kwamgmdHostCacheAddress.len);

  /* retrieve key: mgmdHostCacheIfIndex */
  kwamgmdHostCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex,
                   (xLibU8_t *) & keymgmdHostCacheIfIndexValue, &kwamgmdHostCacheIfIndex.len);
  if (kwamgmdHostCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheIfIndex);
    return kwamgmdHostCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheIfIndexValue, kwamgmdHostCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostCacheStatusGet (L7_UNIT_CURRENT, keymgmdHostCacheAddressTypeValue,
                              (L7_inet_addr_t *)keymgmdHostCacheAddressValue,
                              keymgmdHostCacheIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    objmgmdHostStatusValue = L7_DISABLE;  
  }
  else
  {
    objmgmdHostStatusValue = L7_ENABLE;    
  }

  /* return the object value: mgmdHostStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostStatusValue,
                           sizeof (objmgmdHostStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostGroupStatus
*
* @purpose Get 'mgmdHostGroupStatus'
*
* @description [mgmdHostGroupStatus] Mgmd Host Group Status
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdHostCacheTable_mgmdHostGroupStatus (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdHostCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheAddressTypeValue;
  fpObjWa_t kwamgmdHostCacheAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_inet_addr_t  keymgmdHostCacheAddressValue;
  fpObjWa_t kwamgmdHostCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdHostCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdHostGroupStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdHostCacheAddressType */
  kwamgmdHostCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddressType,
                   (xLibU8_t *) & keymgmdHostCacheAddressTypeValue,
                   &kwamgmdHostCacheAddressType.len);
  if (kwamgmdHostCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddressType);
    return kwamgmdHostCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheAddressTypeValue,
                           kwamgmdHostCacheAddressType.len);

  /* retrieve key: mgmdHostCacheAddress */
  kwamgmdHostCacheAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheAddress,
                   (xLibU8_t *) &keymgmdHostCacheAddressValue, &kwamgmdHostCacheAddress.len);
  if (kwamgmdHostCacheAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheAddress);
    return kwamgmdHostCacheAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheAddressValue, kwamgmdHostCacheAddress.len);

  /* retrieve key: mgmdHostCacheIfIndex */
  kwamgmdHostCacheIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdHostCacheTable_mgmdHostCacheIfIndex,
                   (xLibU8_t *) & keymgmdHostCacheIfIndexValue, &kwamgmdHostCacheIfIndex.len);
  if (kwamgmdHostCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdHostCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdHostCacheIfIndex);
    return kwamgmdHostCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdHostCacheIfIndexValue, kwamgmdHostCacheIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdHostGroupStatusGet (L7_UNIT_CURRENT, keymgmdHostCacheAddressTypeValue,
                              &keymgmdHostCacheAddressValue,
                              keymgmdHostCacheIfIndexValue, &objmgmdHostGroupStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdHostGroupStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdHostGroupStatusValue,
                           sizeof (objmgmdHostGroupStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

