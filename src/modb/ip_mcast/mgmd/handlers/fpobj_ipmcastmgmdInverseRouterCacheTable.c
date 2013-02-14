
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastmgmdInverseRouterCacheTable.c
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
#include "_xe_ipmcastmgmdInverseRouterCacheTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "usmdb_mib_igmp_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheIfIndex
*
* @purpose Get 'mgmdInverseRouterCacheIfIndex'
 *@description  [mgmdInverseRouterCacheIfIndex] The interface for which this entry
* contains information.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheIfIndex (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t kwamgmdInverseRouterCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdInverseRouterCacheAddressTypeValue;

  xLibU32_t objmgmdInverseRouterCacheIfIndexValue;
  xLibU32_t nextObjmgmdInverseRouterCacheIfIndexValue;
  
  L7_inet_addr_t objmgmdInverseRouterCacheAddressValue;
  L7_inet_addr_t nextObjmgmdInverseRouterCacheAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdInverseRouterCacheAddressType */
  kwamgmdInverseRouterCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheAddressType,
                   (xLibU8_t *) & keymgmdInverseRouterCacheAddressTypeValue,
                   &kwamgmdInverseRouterCacheAddressType.len);
  if (kwamgmdInverseRouterCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdInverseRouterCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdInverseRouterCacheAddressType);
    return kwamgmdInverseRouterCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdInverseRouterCacheAddressTypeValue,
                           kwamgmdInverseRouterCacheAddressType.len);

  /* retrieve key: mgmdInverseRouterCacheIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheIfIndex,
                          (xLibU8_t *) & objmgmdInverseRouterCacheIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdInverseRouterCacheIfIndexValue = 0;
    memset (&nextObjmgmdInverseRouterCacheAddressValue, 0, sizeof (nextObjmgmdInverseRouterCacheAddressValue));

    owa.l7rc = usmDbMgmdCacheIntfEntryNextGet (L7_UNIT_CURRENT, keymgmdInverseRouterCacheAddressTypeValue,
                                                                                     &nextObjmgmdInverseRouterCacheIfIndexValue,
                                                                                     &nextObjmgmdInverseRouterCacheAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdInverseRouterCacheIfIndexValue, owa.len);
    nextObjmgmdInverseRouterCacheIfIndexValue = objmgmdInverseRouterCacheIfIndexValue;
    memset (&nextObjmgmdInverseRouterCacheAddressValue, 0, sizeof (nextObjmgmdInverseRouterCacheAddressValue));
    
    do
    {
      owa.l7rc = usmDbMgmdCacheIntfEntryNextGet (L7_UNIT_CURRENT, keymgmdInverseRouterCacheAddressTypeValue,
                                                                                       &nextObjmgmdInverseRouterCacheIfIndexValue,
                                                                                       &nextObjmgmdInverseRouterCacheAddressValue);

      memcpy (&objmgmdInverseRouterCacheAddressValue, &nextObjmgmdInverseRouterCacheAddressValue,
               sizeof (nextObjmgmdInverseRouterCacheAddressValue));
    }
    while ((objmgmdInverseRouterCacheIfIndexValue == nextObjmgmdInverseRouterCacheIfIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdInverseRouterCacheIfIndexValue, owa.len);

  /* return the object value: mgmdInverseRouterCacheIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdInverseRouterCacheIfIndexValue,
                           sizeof (nextObjmgmdInverseRouterCacheIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheAddressType
*
* @purpose Get 'mgmdInverseRouterCacheAddressType'
 *@description  [mgmdInverseRouterCacheAddressType] The address type of the
* mgmdInverseRouterCacheTable entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheAddressType (void *wap,
                                                                                    void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdInverseRouterCacheAddressTypeValue;
  xLibU32_t nextObjmgmdInverseRouterCacheAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdInverseRouterCacheAddressType */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheAddressType,
                   (xLibU8_t *) & objmgmdInverseRouterCacheAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdInverseRouterCacheAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdInverseRouterCacheAddressTypeValue, owa.len);
    if (objmgmdInverseRouterCacheAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjmgmdInverseRouterCacheAddressTypeValue = objmgmdInverseRouterCacheAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdInverseRouterCacheAddressTypeValue, owa.len);

  /* return the object value: mgmdInverseRouterCacheAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdInverseRouterCacheAddressTypeValue,
                           sizeof (nextObjmgmdInverseRouterCacheAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheAddress
*
* @purpose Get 'mgmdInverseRouterCacheAddress'
 *@description  [mgmdInverseRouterCacheAddress] The IP multicast group address for
* which this entry contains information about an interface. The
* InetAddressType, e.g. IPv4 or IPv6, is identified by the
* mgmdInverseRouterCacheAddressType.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheAddress (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t kwamgmdInverseRouterCacheAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdInverseRouterCacheAddressTypeValue;

  xLibU32_t objmgmdInverseRouterCacheIfIndexValue;
  xLibU32_t nextObjmgmdInverseRouterCacheIfIndexValue;
  
  L7_inet_addr_t objmgmdInverseRouterCacheAddressValue;
  L7_inet_addr_t nextObjmgmdInverseRouterCacheAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdInverseRouterCacheAddressType */
  kwamgmdInverseRouterCacheAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheAddressType,
                   (xLibU8_t *) & keymgmdInverseRouterCacheAddressTypeValue,
                   &kwamgmdInverseRouterCacheAddressType.len);
  if (kwamgmdInverseRouterCacheAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdInverseRouterCacheAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdInverseRouterCacheAddressType);
    return kwamgmdInverseRouterCacheAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdInverseRouterCacheAddressTypeValue,
                           kwamgmdInverseRouterCacheAddressType.len);

  /* retrieve key: mgmdInverseRouterCacheIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheIfIndex,
                          (xLibU8_t *) & objmgmdInverseRouterCacheIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdInverseRouterCacheIfIndexValue, owa.len);

  /* retrieve key: mgmdInverseRouterCacheAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdInverseRouterCacheTable_mgmdInverseRouterCacheAddress,
                          (xLibU8_t *)& objmgmdInverseRouterCacheAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjmgmdInverseRouterCacheIfIndexValue = objmgmdInverseRouterCacheIfIndexValue;
    memset (&nextObjmgmdInverseRouterCacheAddressValue, 0, sizeof (nextObjmgmdInverseRouterCacheAddressValue));

    owa.l7rc = usmDbMgmdCacheIntfEntryNextGet (L7_UNIT_CURRENT, keymgmdInverseRouterCacheAddressTypeValue,
                                                                                     &nextObjmgmdInverseRouterCacheIfIndexValue,
                                                                                     &nextObjmgmdInverseRouterCacheAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdInverseRouterCacheAddressValue, owa.len);

    nextObjmgmdInverseRouterCacheIfIndexValue = objmgmdInverseRouterCacheIfIndexValue;
    memcpy (&nextObjmgmdInverseRouterCacheAddressValue, 
                  &objmgmdInverseRouterCacheAddressValue, 
                  sizeof (nextObjmgmdInverseRouterCacheAddressValue));

    owa.l7rc = usmDbMgmdCacheIntfEntryNextGet (L7_UNIT_CURRENT, keymgmdInverseRouterCacheAddressTypeValue,
                                                                                     &nextObjmgmdInverseRouterCacheIfIndexValue,
                                                                                     &nextObjmgmdInverseRouterCacheAddressValue);

  }

  if ((objmgmdInverseRouterCacheIfIndexValue != nextObjmgmdInverseRouterCacheIfIndexValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdInverseRouterCacheAddressValue, owa.len);

  /* return the object value: mgmdInverseRouterCacheAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdInverseRouterCacheAddressValue,
                           sizeof (&nextObjmgmdInverseRouterCacheAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
