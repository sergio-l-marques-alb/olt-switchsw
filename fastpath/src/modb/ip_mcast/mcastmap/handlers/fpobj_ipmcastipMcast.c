
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastipMcast.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  16 May 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastipMcast_obj.h"
#include "usmdb_mib_mcast_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastipMcast_ipMcastRouteEntryCount
*
* @purpose Get 'ipMcastRouteEntryCount'
 *@description  [ipMcastRouteEntryCount] The number of rows in the
* ipMcastRouteTable. This can be used to check for multicast routing activity,
* and to monitor the multicast routing table size.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcast_ipMcastRouteEntryCount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteEntryCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteEntryCountGet (L7_UNIT_CURRENT, &objipMcastRouteEntryCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastRouteEntryCountValue,
                     sizeof (objipMcastRouteEntryCountValue));

  /* return the object value: ipMcastRouteEntryCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteEntryCountValue,
                           sizeof (objipMcastRouteEntryCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcast_ipMcastEnabled
*
* @purpose Get 'ipMcastEnabled'
 *@description  [ipMcastEnabled] The enabled status of IP Multicast function on
* this system. The storage type of this object is determined by
* ipMcastDeviceConfigStorageType.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcast_ipMcastEnabled (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastEnabledValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMcastAdminModeGet (L7_UNIT_CURRENT, &objipMcastEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastEnabledValue, sizeof (objipMcastEnabledValue));

  /* return the object value: ipMcastEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastEnabledValue,
                           sizeof (objipMcastEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcast_ipMcastEnabled
*
* @purpose Set 'ipMcastEnabled'
 *@description  [ipMcastEnabled] The enabled status of IP Multicast function on
* this system. The storage type of this object is determined by
* ipMcastDeviceConfigStorageType.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcast_ipMcastEnabled (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastEnabledValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastEnabled */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastEnabledValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastEnabledValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbMcastAdminModeSet (L7_UNIT_CURRENT, objipMcastEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcast_ipMcastRouteHighestEntryCount
*
* @purpose Get 'ipMcastRouteHighestEntryCount'
 *@description  [ipMcastRouteHighestEntryCount] The IP Multicast route table
* Highest entry count.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcast_ipMcastRouteHighestEntryCount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteHighestEntryCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteHighestEntryCountGet (L7_UNIT_CURRENT, &objipMcastRouteHighestEntryCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastRouteHighestEntryCountValue,
                     sizeof (objipMcastRouteHighestEntryCountValue));

  /* return the object value: ipMcastRouteHighestEntryCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteHighestEntryCountValue,
                           sizeof (objipMcastRouteHighestEntryCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcast_ipMcastRouteTableMaxSize
*
* @purpose Get 'ipMcastRouteTableMaxSize'
 *@description  [ipMcastRouteTableMaxSize] The IP Multicast route table Maximum
* Size.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcast_ipMcastRouteTableMaxSize (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteTableMaxSizeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteTableMaxSizeGet (L7_UNIT_CURRENT, &objipMcastRouteTableMaxSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastRouteTableMaxSizeValue,
                     sizeof (objipMcastRouteTableMaxSizeValue));

  /* return the object value: ipMcastRouteTableMaxSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteTableMaxSizeValue,
                           sizeof (objipMcastRouteTableMaxSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_ipmcastipMcast_ipMcastRouteAddressType
*
* @purpose Get 'ipMcastRouteAddressType'
 *@description  [ipMcastRouteAddressType] A value indicating the address family
* of the address.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcast_ipMcastRouteAddressType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteAddressTypeValue;
  xLibU32_t nextObjipMcastRouteAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcast_ipMcastRouteAddressType,
                          (xLibU8_t *) & objipMcastRouteAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjipMcastRouteAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteAddressTypeValue, owa.len);
    if(objipMcastRouteAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjipMcastRouteAddressTypeValue = objipMcastRouteAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastRouteAddressTypeValue, owa.len);

  /* return the object value: ipMcastRouteAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastRouteAddressTypeValue,
                           sizeof (nextObjipMcastRouteAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_ipmcastipMcast_ipMcastRouteCurrentMcastProtocol
*
* @purpose Get 'ipMcastRouteCurrentMcastProtocol'
 *@description  [ipMcastRouteCurrentMcastProtocol] Current Multicast protocol
* running in the router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcast_ipMcastRouteCurrentMcastProtocol (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteCurrentMcastProtocolValue;

  fpObjWa_t kwaipMcastRouteAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteAddressTypeValue;
  xLibU32_t val;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteAddressType */
  kwaipMcastRouteAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcast_ipMcastRouteAddressType,
                                                 (xLibU8_t *) & keyipMcastRouteAddressTypeValue,
                                                 &kwaipMcastRouteAddressType.len);
  if (kwaipMcastRouteAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteAddressType);
    return kwaipMcastRouteAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteAddressTypeValue, kwaipMcastRouteAddressType.len);


  /* get the value from application */
  owa.l7rc = usmDbMcastIpProtocolGet (L7_UNIT_CURRENT, 
                                                          keyipMcastRouteAddressTypeValue,
                                                          &val);

  if(owa.l7rc == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_MCAST_IANA_MROUTE_DVMRP:
      objipMcastRouteCurrentMcastProtocolValue = L7_XUI_MCAST_IANA_MROUTE_DVMRP;
      break;
    case L7_MCAST_IANA_MROUTE_PIM_DM:
      objipMcastRouteCurrentMcastProtocolValue = L7_XUI_MCAST_IANA_MROUTE_PIM_DM;
      break;
    case L7_MCAST_IANA_MROUTE_PIM_SM:
      objipMcastRouteCurrentMcastProtocolValue = L7_XUI_MCAST_IANA_MROUTE_PIM_SM;
      break;
    case L7_MCAST_IANA_MROUTE_IGMP_PROXY:
      objipMcastRouteCurrentMcastProtocolValue = L7_XUI_MCAST_IANA_MROUTE_IGMP_PROXY;
      break;
    default:
      objipMcastRouteCurrentMcastProtocolValue = L7_XUI_MCAST_IANA_MROUTE_NO_PROTOCOL;
      break;
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastRouteCurrentMcastProtocolValue,
                     sizeof (objipMcastRouteCurrentMcastProtocolValue));

  /* return the object value: ipMcastRouteCurrentMcastProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteCurrentMcastProtocolValue,
                           sizeof (objipMcastRouteCurrentMcastProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcast_ipMcastRouteMcastOperationalState
*
* @purpose Get 'ipMcastRouteMcastOperationalState'
 *@description  [ipMcastRouteMcastOperationalState] Whether multicast component
* is operational.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcast_ipMcastRouteMcastOperationalState (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteMcastOperationalStateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  objipMcastRouteMcastOperationalStateValue = usmDbMcastOperationalStateGet(L7_UNIT_CURRENT);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastRouteMcastOperationalStateValue,
                     sizeof (objipMcastRouteMcastOperationalStateValue));

  /* return the object value: ipMcastRouteMcastOperationalState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteMcastOperationalStateValue,
                           sizeof (objipMcastRouteMcastOperationalStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcast_ipMcastDeviceConfigStorageType
*
* @purpose Get 'ipMcastDeviceConfigStorageType'
 *@description  [ipMcastDeviceConfigStorageType] The storage type used for the
* global IP multicast configuration of this device, comprised of the
* objects listed below. If this storage type takes the value
* 'permanent', write-access to the listed objects need not be allowed.
* The objects described by this storage type are: ipMcastEnabled.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcast_ipMcastDeviceConfigStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastDeviceConfigStorageTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objipMcastDeviceConfigStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objipMcastDeviceConfigStorageTypeValue,
                     sizeof (objipMcastDeviceConfigStorageTypeValue));

  /* return the object value: ipMcastDeviceConfigStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastDeviceConfigStorageTypeValue,
                           sizeof (objipMcastDeviceConfigStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcast_ipMcastDeviceConfigStorageType
*
* @purpose Set 'ipMcastDeviceConfigStorageType'
 *@description  [ipMcastDeviceConfigStorageType] The storage type used for the
* global IP multicast configuration of this device, comprised of the
* objects listed below. If this storage type takes the value
* 'permanent', write-access to the listed objects need not be allowed.
* The objects described by this storage type are: ipMcastEnabled.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcast_ipMcastDeviceConfigStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastDeviceConfigStorageTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastDeviceConfigStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastDeviceConfigStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastDeviceConfigStorageTypeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objipMcastDeviceConfigStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}
