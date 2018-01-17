
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdInverseHostCacheTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  11 June 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastmgmdInverseHostCacheTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_mib_igmp_api.h"
/*******************************************************************************
* @function fpObjGet_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheIfIndex
*
* @purpose Get 'mgmdInverseHostCacheIfIndex'
 *@description  [mgmdInverseHostCacheIfIndex] The interface for which this entry
* contains information.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheIfIndex (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t kwamgmdInverseHostCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdInverseHostCacheAddressTypeValue;

  xLibU32_t objmgmdInverseHostCacheIfIndexValue;
  xLibU32_t nextObjmgmdInverseHostCacheIfIndexValue;
  
  L7_inet_addr_t objmgmdInverseHostCacheAddressValue;
  L7_inet_addr_t nextObjmgmdInverseHostCacheAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdInverseHostCacheAddressType */
  kwamgmdInverseHostCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheAddressType,
                   (xLibU8_t *) & keymgmdInverseHostCacheAddressTypeValue,
                   &kwamgmdInverseHostCacheAddressType.len);
  if (kwamgmdInverseHostCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdInverseHostCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdInverseHostCacheAddressType);
    return kwamgmdInverseHostCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdInverseHostCacheAddressTypeValue,
                           kwamgmdInverseHostCacheAddressType.len);

  /* retrieve key: mgmdInverseHostCacheIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheIfIndex,
                          (xLibU8_t *) & objmgmdInverseHostCacheIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdInverseHostCacheIfIndexValue = 0;
    memset (&nextObjmgmdInverseHostCacheAddressValue, 0, sizeof (nextObjmgmdInverseHostCacheAddressValue));

    owa.l7rc = usmDbMgmdHostInverseCacheEntryGetNext (L7_UNIT_CURRENT, keymgmdInverseHostCacheAddressTypeValue,
                                                                                     &nextObjmgmdInverseHostCacheIfIndexValue,
                                                                                     &nextObjmgmdInverseHostCacheAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdInverseHostCacheIfIndexValue, owa.len);
    nextObjmgmdInverseHostCacheIfIndexValue = objmgmdInverseHostCacheIfIndexValue;
    memset (&nextObjmgmdInverseHostCacheAddressValue, 0, sizeof (nextObjmgmdInverseHostCacheAddressValue));
    
    do
    {
      owa.l7rc = usmDbMgmdHostInverseCacheEntryGetNext (L7_UNIT_CURRENT, keymgmdInverseHostCacheAddressTypeValue,
                                                                                       &nextObjmgmdInverseHostCacheIfIndexValue,
                                                                                       &nextObjmgmdInverseHostCacheAddressValue);

      memcpy (&objmgmdInverseHostCacheAddressValue, &nextObjmgmdInverseHostCacheAddressValue,
               sizeof (nextObjmgmdInverseHostCacheAddressValue));
    }
    while ((objmgmdInverseHostCacheIfIndexValue == nextObjmgmdInverseHostCacheIfIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdInverseHostCacheIfIndexValue, owa.len);

  /* return the object value: mgmdInverseHostCacheIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdInverseHostCacheIfIndexValue,
                           sizeof (nextObjmgmdInverseHostCacheIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheAddressType
*
* @purpose Get 'mgmdInverseHostCacheAddressType'
 *@description  [mgmdInverseHostCacheAddressType] The address type of the
* mgmdInverseHostCacheTable entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheAddressType (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdInverseHostCacheAddressTypeValue;
  xLibU32_t nextObjmgmdInverseHostCacheAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdInverseHostCacheAddressType */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheAddressType,
                   (xLibU8_t *) & objmgmdInverseHostCacheAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdInverseHostCacheAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdInverseHostCacheAddressTypeValue, owa.len);
    if (objmgmdInverseHostCacheAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjmgmdInverseHostCacheAddressTypeValue = objmgmdInverseHostCacheAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdInverseHostCacheAddressTypeValue, owa.len);

  /* return the object value: mgmdInverseHostCacheAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdInverseHostCacheAddressTypeValue,
                           sizeof (nextObjmgmdInverseHostCacheAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheAddress
*
* @purpose Get 'mgmdInverseHostCacheAddress'
 *@description  [mgmdInverseHostCacheAddress] The IP multicast group address for
* which this entry contains information about an interface. The
* InetAddressType, e.g. IPv4 or IPv6, is identified by the
* mgmdInverseHostCacheAddressType.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheAddress (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t kwamgmdInverseHostCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdInverseHostCacheAddressTypeValue;

  xLibU32_t objmgmdInverseHostCacheIfIndexValue;
  xLibU32_t nextObjmgmdInverseHostCacheIfIndexValue;
  
  L7_inet_addr_t objmgmdInverseHostCacheAddressValue;
  L7_inet_addr_t nextObjmgmdInverseHostCacheAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdInverseHostCacheAddressType */
  kwamgmdInverseHostCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheAddressType,
                   (xLibU8_t *) & keymgmdInverseHostCacheAddressTypeValue,
                   &kwamgmdInverseHostCacheAddressType.len);
  if (kwamgmdInverseHostCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdInverseHostCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdInverseHostCacheAddressType);
    return kwamgmdInverseHostCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdInverseHostCacheAddressTypeValue,
                           kwamgmdInverseHostCacheAddressType.len);

  /* retrieve key: mgmdInverseHostCacheIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheIfIndex,
                          (xLibU8_t *) & objmgmdInverseHostCacheIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdInverseHostCacheIfIndexValue, owa.len);

  /* retrieve key: mgmdInverseHostCacheAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseHostCacheTable_mgmdInverseHostCacheAddress,
                          (xLibU8_t *)& objmgmdInverseHostCacheAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjmgmdInverseHostCacheIfIndexValue = objmgmdInverseHostCacheIfIndexValue;
    memset (&nextObjmgmdInverseHostCacheAddressValue, 0, sizeof (nextObjmgmdInverseHostCacheAddressValue));

    owa.l7rc = usmDbMgmdHostInverseCacheEntryGetNext (L7_UNIT_CURRENT, keymgmdInverseHostCacheAddressTypeValue,
                                                                                     &nextObjmgmdInverseHostCacheIfIndexValue,
                                                                                     &nextObjmgmdInverseHostCacheAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdInverseHostCacheAddressValue, owa.len);

    nextObjmgmdInverseHostCacheIfIndexValue = objmgmdInverseHostCacheIfIndexValue;
    memcpy (&nextObjmgmdInverseHostCacheAddressValue, 
                  &objmgmdInverseHostCacheAddressValue, 
                  sizeof (nextObjmgmdInverseHostCacheAddressValue));

    owa.l7rc = usmDbMgmdHostInverseCacheEntryGetNext (L7_UNIT_CURRENT, keymgmdInverseHostCacheAddressTypeValue,
                                                                                     &nextObjmgmdInverseHostCacheIfIndexValue,
                                                                                     &nextObjmgmdInverseHostCacheAddressValue);

  }

  if ((objmgmdInverseHostCacheIfIndexValue != nextObjmgmdInverseHostCacheIfIndexValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdInverseHostCacheAddressValue, owa.len);

  /* return the object value: mgmdInverseHostCacheAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdInverseHostCacheAddressValue,
                           sizeof (&nextObjmgmdInverseHostCacheAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
