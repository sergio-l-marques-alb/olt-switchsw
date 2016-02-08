
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastipMcastInterfaceTable.c
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
#include "_xe_ipmcastipMcastInterfaceTable_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_mcast_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion
*
* @purpose Get 'ipMcastInterfaceIPVersion'
 *@description  [ipMcastInterfaceIPVersion] The IP version of this row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion (void *wap, void *bufp)
{

  xLibU32_t objipMcastInterfaceIPVersionValue;
  xLibU32_t nextObjipMcastInterfaceIPVersionValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIPVersion */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                          (xLibU8_t *) & objipMcastInterfaceIPVersionValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjipMcastInterfaceIPVersionValue = 4; /* Not sure what IP version means in this MIB */
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastInterfaceIPVersionValue, owa.len);

    if(objipMcastInterfaceIPVersionValue == 4)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjipMcastInterfaceIPVersionValue = 4; /* Not sure what IP version means in this MIB */
      owa.l7rc = L7_SUCCESS;
    }
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastInterfaceIPVersionValue, owa.len);

  /* return the object value: ipMcastInterfaceIPVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastInterfaceIPVersionValue,
                           sizeof (objipMcastInterfaceIPVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex
*
* @purpose Get 'ipMcastInterfaceIfIndex'
 *@description  [ipMcastInterfaceIfIndex] The index value that uniquely
* identifies the interface to which this entry is applicable. The interface
* identified by a particular value of this index is the same
* interface as identified by the same value of the IF-MIB's ifIndex.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex (void *wap, void *bufp)
{

  xLibU32_t objipMcastInterfaceIfIndexValue;
  xLibU32_t nextObjipMcastInterfaceIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                          (xLibU8_t *) & objipMcastInterfaceIfIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objipMcastInterfaceIfIndexValue, 0, sizeof (objipMcastInterfaceIfIndexValue));
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF,
                                            0, &nextObjipMcastInterfaceIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastInterfaceIfIndexValue, owa.len);

    nextObjipMcastInterfaceIfIndexValue = 0;
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF, 0,
                    objipMcastInterfaceIfIndexValue, &nextObjipMcastInterfaceIfIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastInterfaceIfIndexValue, owa.len);

  /* return the object value: ipMcastInterfaceIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastInterfaceIfIndexValue,
                           sizeof (objipMcastInterfaceIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceTtl
*
* @purpose Get 'ipMcastInterfaceTtl'
 *@description  [ipMcastInterfaceTtl] The datagram Time to Live (TTL) threshold
* for the interface. Any IP multicast datagrams with a TTL (IPv4)
* or Hop Limit (IPv6) less than this threshold will not be forwarded
* out the interface. The default value of 0 means all multicast
* packets are forwarded out the interface. A value of 256 means that
* no multicast packets are forwarded out the interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceTtl (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastInterfaceTtlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteInterfaceTtlGet (L7_UNIT_CURRENT, keyipMcastInterfaceIfIndexValue,
                                                &objipMcastInterfaceTtlValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastInterfaceTtl */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastInterfaceTtlValue,
                           sizeof (objipMcastInterfaceTtlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastInterfaceTable_ipMcastInterfaceTtl
*
* @purpose Set 'ipMcastInterfaceTtl'
 *@description  [ipMcastInterfaceTtl] The datagram Time to Live (TTL) threshold
* for the interface. Any IP multicast datagrams with a TTL (IPv4)
* or Hop Limit (IPv6) less than this threshold will not be forwarded
* out the interface. The default value of 0 means all multicast
* packets are forwarded out the interface. A value of 256 means that
* no multicast packets are forwarded out the interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastInterfaceTable_ipMcastInterfaceTtl (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastInterfaceTtlValue;

  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastInterfaceTtl */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastInterfaceTtlValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastInterfaceTtlValue, owa.len);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbMcastIpMRouteInterfaceTtlSet (L7_UNIT_CURRENT, keyipMcastInterfaceIfIndexValue,
                              objipMcastInterfaceTtlValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceProtocol
*
* @purpose Get 'ipMcastRouteInterfaceProtocol'
 *@description  [ipMcastRouteInterfaceProtocol] The routing protocol running on
* this interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceProtocol (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteInterfaceProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteInterfaceProtocolGet (L7_UNIT_CURRENT, keyipMcastInterfaceIfIndexValue,
                              &objipMcastRouteInterfaceProtocolValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteInterfaceProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteInterfaceProtocolValue,
                           sizeof (objipMcastRouteInterfaceProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceRateLimit
*
* @purpose Get 'ipMcastInterfaceRateLimit'
 *@description  [ipMcastInterfaceRateLimit] The rate-limit, in kilobits per
* second, of forwarded multicast traffic on the interface. A rate-limit
* of 0 indicates that no rate limiting is done.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceRateLimit (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastInterfaceRateLimitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastInterfaceIPVersionValue,
                              keyipMcastInterfaceIfIndexValue, &objipMcastInterfaceRateLimitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastInterfaceRateLimit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastInterfaceRateLimitValue,
                           sizeof (objipMcastInterfaceRateLimitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastInterfaceTable_ipMcastInterfaceRateLimit
*
* @purpose Set 'ipMcastInterfaceRateLimit'
 *@description  [ipMcastInterfaceRateLimit] The rate-limit, in kilobits per
* second, of forwarded multicast traffic on the interface. A rate-limit
* of 0 indicates that no rate limiting is done.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastInterfaceTable_ipMcastInterfaceRateLimit (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastInterfaceRateLimitValue;

  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastInterfaceRateLimit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastInterfaceRateLimitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastInterfaceRateLimitValue, owa.len);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyipMcastInterfaceIPVersionValue,
                              keyipMcastInterfaceIfIndexValue, objipMcastInterfaceRateLimitValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceInMcastOctets
*
* @purpose Get 'ipMcastRouteInterfaceInMcastOctets'
 *@description  [ipMcastRouteInterfaceInMcastOctets] The number of octets of
* multicast packets that have arrived on the interface, including
* framing characters. This object is similar to ifInOctets in the
* Interfaces MIB, except that only multicast packets are counted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceInMcastOctets (void *wap,
                                                                                   void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteInterfaceInMcastOctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastInterfaceIPVersionValue,
                              keyipMcastInterfaceIfIndexValue,
                              &objipMcastRouteInterfaceInMcastOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteInterfaceInMcastOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteInterfaceInMcastOctetsValue,
                           sizeof (objipMcastRouteInterfaceInMcastOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceOutMcastOctets
*
* @purpose Get 'ipMcastRouteInterfaceOutMcastOctets'
 *@description  [ipMcastRouteInterfaceOutMcastOctets] The number of octets of
* multicast packets that have been sent on the interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceOutMcastOctets (void *wap,
                                                                                    void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteInterfaceOutMcastOctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastInterfaceIPVersionValue,
                              keyipMcastInterfaceIfIndexValue,
                              &objipMcastRouteInterfaceOutMcastOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteInterfaceOutMcastOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteInterfaceOutMcastOctetsValue,
                           sizeof (objipMcastRouteInterfaceOutMcastOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceHCInMcastOctets
*
* @purpose Get 'ipMcastRouteInterfaceHCInMcastOctets'
 *@description  [ipMcastRouteInterfaceHCInMcastOctets] The number of octets of
* multicast packets that have arrived on the interface, including
* framing characters. This object is a 64-bit version of
* ipMcastRouteInterfaceInMcastOctets. It is similar to ifHCInOctets in the
* Interfaces MIB, except that only multicast packets are counted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceHCInMcastOctets (void *wap,
                                                                                     void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteInterfaceHCInMcastOctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastInterfaceIPVersionValue,
                              keyipMcastInterfaceIfIndexValue,
                              &objipMcastRouteInterfaceHCInMcastOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteInterfaceHCInMcastOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteInterfaceHCInMcastOctetsValue,
                           sizeof (objipMcastRouteInterfaceHCInMcastOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceHCOutMcastOctets
*
* @purpose Get 'ipMcastRouteInterfaceHCOutMcastOctets'
 *@description  [ipMcastRouteInterfaceHCOutMcastOctets] The number of octets of
* multicast packets that have been sent on the interface. This
* object is a 64-bit version of ipMcastRouteInterfaceOutMcastOctets.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastRouteInterfaceHCOutMcastOctets (void *wap,
                                                                                      void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteInterfaceHCOutMcastOctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastInterfaceIPVersionValue,
                              keyipMcastInterfaceIfIndexValue,
                              &objipMcastRouteInterfaceHCOutMcastOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteInterfaceHCOutMcastOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteInterfaceHCOutMcastOctetsValue,
                           sizeof (objipMcastRouteInterfaceHCOutMcastOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceStorageType
*
* @purpose Get 'ipMcastInterfaceStorageType'
 *@description  [ipMcastInterfaceStorageType] The storage type for this row.
* Rows having the value 'permanent' need not allow write-access to any
* columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastInterfaceTable_ipMcastInterfaceStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastInterfaceStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastInterfaceIPVersionValue,
                              keyipMcastInterfaceIfIndexValue,
                              &objipMcastInterfaceStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastInterfaceStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastInterfaceStorageTypeValue,
                           sizeof (objipMcastInterfaceStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastInterfaceTable_ipMcastInterfaceStorageType
*
* @purpose Set 'ipMcastInterfaceStorageType'
 *@description  [ipMcastInterfaceStorageType] The storage type for this row.
* Rows having the value 'permanent' need not allow write-access to any
* columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastInterfaceTable_ipMcastInterfaceStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastInterfaceStorageTypeValue;

  fpObjWa_t kwaipMcastInterfaceIPVersion = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIPVersionValue;
  fpObjWa_t kwaipMcastInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastInterfaceIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastInterfaceStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastInterfaceStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastInterfaceStorageTypeValue, owa.len);

  /* retrieve key: ipMcastInterfaceIPVersion */
  kwaipMcastInterfaceIPVersion.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIPVersion,
                   (xLibU8_t *) & keyipMcastInterfaceIPVersionValue,
                   &kwaipMcastInterfaceIPVersion.len);
  if (kwaipMcastInterfaceIPVersion.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIPVersion.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIPVersion);
    return kwaipMcastInterfaceIPVersion.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIPVersionValue,
                           kwaipMcastInterfaceIPVersion.len);

  /* retrieve key: ipMcastInterfaceIfIndex */
  kwaipMcastInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastInterfaceTable_ipMcastInterfaceIfIndex,
                   (xLibU8_t *) & keyipMcastInterfaceIfIndexValue, &kwaipMcastInterfaceIfIndex.len);
  if (kwaipMcastInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastInterfaceIfIndex);
    return kwaipMcastInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastInterfaceIfIndexValue, kwaipMcastInterfaceIfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyipMcastInterfaceIPVersionValue,
                              keyipMcastInterfaceIfIndexValue, objipMcastInterfaceStorageTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}
