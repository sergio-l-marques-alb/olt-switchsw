
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastipMcastBoundaryTable.c
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
#include "_xe_ipmcastipMcastBoundaryTable_obj.h"
#include "usmdb_mib_mcast_api.h"
#include "usmdb_util_api.h"
#include "usmdb_ip_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex
*
* @purpose Get 'ipMcastBoundaryIfIndex'
 *@description  [ipMcastBoundaryIfIndex] The IfIndex value for the interface to
* which this boundary applies. Packets with a destination address
* in the associated address/mask range will not be forwarded over
* this interface. For IPv4, zone boundaries cut through links.
* Therefore, this is an external interface. This may be either a physical
* or virtual interface (tunnel, encapsulation, and so forth.) For
* IPv6, zone boundaries cut through nodes. Therefore, this is a
* virtual interface within the node. This is not an external
* interface, either real or virtual. Packets crossing this interface neither
* arrive at nor leave the node, but only move between zones within
* the node.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex (void *wap, void *bufp)
{

  xLibU32_t objipMcastBoundaryIfIndexValue;
  xLibU32_t nextObjipMcastBoundaryIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastBoundaryIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                          (xLibU8_t *) & objipMcastBoundaryIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    objipMcastBoundaryIfIndexValue = 0;
    nextObjipMcastBoundaryIfIndexValue = 0;
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF,
                                            0, &nextObjipMcastBoundaryIfIndexValue);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryIfIndexValue, owa.len);
    nextObjipMcastBoundaryIfIndexValue = 0;
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF, 0,
                    objipMcastBoundaryIfIndexValue, &nextObjipMcastBoundaryIfIndexValue);
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastBoundaryIfIndexValue, owa.len);

  /* return the object value: ipMcastBoundaryIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastBoundaryIfIndexValue,
                           sizeof (objipMcastBoundaryIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType
*
* @purpose Get 'ipMcastBoundaryAddressType'
 *@description  [ipMcastBoundaryAddressType] A value indicating the address
* family of the address contained in ipMcastBoundaryAddress. Legal
* values correspond to the subset of address families for which
* multicast forwarding is supported.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType (void *wap, void *bufp)
{

  xLibU32_t objipMcastBoundaryIfIndexValue;
  xLibU32_t objipMcastBoundaryAddressTypeValue;
  xLibU32_t nextObjipMcastBoundaryAddressTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: ipMcastBoundaryIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                          (xLibU8_t *) & objipMcastBoundaryIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryIfIndexValue, owa.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                          (xLibU8_t *) & objipMcastBoundaryAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjipMcastBoundaryAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryAddressTypeValue, owa.len);
    if(objipMcastBoundaryAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjipMcastBoundaryAddressTypeValue = objipMcastBoundaryAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastBoundaryAddressTypeValue, owa.len);

  /* return the object value: ipMcastBoundaryAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastBoundaryAddressTypeValue,
                           sizeof (objipMcastBoundaryAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress
*
* @purpose Get 'ipMcastBoundaryAddress'
 *@description  [ipMcastBoundaryAddress] The group address which, when combined
* with the corresponding value of
* ipMcastBoundaryAddressPrefixLength, identifies the group range for which the scoped boundary
* exists. Scoped IPv4 multicast address ranges must be prefixed by
* 239.0.0.0/8. Scoped IPv6 multicast address ranges are FF0x::/16, where
* x is a valid RFC 4291 multicast scope. An IPv6 address prefixed
* by FF1x::/16 is a non-permanently- assigned address. An IPv6
* address prefixed by FF3x::/16 is a unicast-prefix-based multicast
* addresses. A zone boundary for FF0x::/16 implies an identical
* boundary for these other prefixes. No separate FF1x::/16 or FF3x::/16
* entries exist in this table. This address object is only
* significant up to ipMcastBoundaryAddressPrefixLength bits. The remaining
* address bits are set to zero. This is especially important for
* this index field, which is part of the index of this entry. Any
* non-zero bits would signify an entirely different entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress (void *wap, void *bufp)
{

  xLibU32_t objipMcastBoundaryIfIndexValue;
  xLibU32_t nextObjipMcastBoundaryIfIndexValue;
  xLibU32_t objipMcastBoundaryAddressTypeValue;
  xLibU32_t nextObjipMcastBoundaryAddressTypeValue;
  L7_inet_addr_t objipMcastBoundaryAddressValue;
  L7_inet_addr_t nextObjipMcastBoundaryAddressValue;
  L7_inet_addr_t nextObjipMcastBoundaryMaskValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: ipMcastBoundaryIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                          (xLibU8_t *) & objipMcastBoundaryIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryIfIndexValue, owa.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                          (xLibU8_t *) & objipMcastBoundaryAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryAddressTypeValue, owa.len);

  /* retrieve key: ipMcastBoundaryAddress */
  owa.len = sizeof (L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress,
                          (xLibU8_t *) &objipMcastBoundaryAddressValue, &owa.len);
  nextObjipMcastBoundaryIfIndexValue = objipMcastBoundaryIfIndexValue;
  nextObjipMcastBoundaryAddressTypeValue = objipMcastBoundaryAddressTypeValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(objipMcastBoundaryAddressTypeValue, &nextObjipMcastBoundaryAddressValue);
    inetAddressZeroSet(objipMcastBoundaryAddressTypeValue, &nextObjipMcastBoundaryMaskValue);
    owa.l7rc =
      usmDbMcastMrouteBoundaryEntryNextGet (L7_UNIT_CURRENT, &nextObjipMcastBoundaryIfIndexValue,
                            &nextObjipMcastBoundaryAddressValue,
                            &nextObjipMcastBoundaryMaskValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryAddressValue, owa.len);
    inetCopy(&nextObjipMcastBoundaryAddressValue, &objipMcastBoundaryAddressValue);
    inetAddressZeroSet(objipMcastBoundaryAddressTypeValue, &nextObjipMcastBoundaryMaskValue);
    do
    {
      owa.l7rc = usmDbMcastMrouteBoundaryEntryNextGet (L7_UNIT_CURRENT,
                                      &nextObjipMcastBoundaryIfIndexValue,
                                      &nextObjipMcastBoundaryAddressValue,
                                      &nextObjipMcastBoundaryMaskValue);
    }while (owa.l7rc == L7_SUCCESS &&
           (nextObjipMcastBoundaryIfIndexValue == objipMcastBoundaryIfIndexValue) &&
           (L7_INET_ADDR_COMPARE(&nextObjipMcastBoundaryAddressValue, &objipMcastBoundaryAddressValue) == 0));
  }
  if (owa.l7rc != L7_SUCCESS ||
      (nextObjipMcastBoundaryIfIndexValue != objipMcastBoundaryIfIndexValue)) 
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastBoundaryAddressValue, owa.len);

  /* return the object value: ipMcastBoundaryAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjipMcastBoundaryAddressValue,
                           sizeof (objipMcastBoundaryAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength
*
* @purpose Get 'ipMcastBoundaryAddressPrefixLength'
 *@description  [ipMcastBoundaryAddressPrefixLength] The length in bits of the
* mask which when, combined with the corresponding value of
* ipMcastBoundaryAddress, identifies the group range for which the scoped
* boundary exists. The InetAddressType is given by
* ipMcastBoundaryAddressType. For values 'ipv4' and 'ipv4z', this object must be in
* the range 4..32. For values 'ipv6' and 'ipv6z', this object must
* be set to 16.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength (void *wap,
                                                                                  void *bufp)
{

  xLibU32_t objipMcastBoundaryIfIndexValue;
  xLibU32_t nextObjipMcastBoundaryIfIndexValue;
  xLibU32_t objipMcastBoundaryAddressTypeValue;
  xLibU32_t nextObjipMcastBoundaryAddressTypeValue;
  L7_inet_addr_t objipMcastBoundaryAddressValue;
  L7_inet_addr_t nextObjipMcastBoundaryAddressValue;
  L7_inet_addr_t nextObjipMcastBoundaryMaskValue;
  xLibV4V6Mask_t objipMcastBoundaryAddressPrefixLengthValue;
  xLibV4V6Mask_t nextObjipMcastBoundaryAddressPrefixLengthValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t addressType;
  xLibU8_t  tmpVal;
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: ipMcastBoundaryIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                          (xLibU8_t *) & objipMcastBoundaryIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryIfIndexValue, owa.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                          (xLibU8_t *) & objipMcastBoundaryAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryAddressTypeValue, owa.len);

  /* retrieve key: ipMcastBoundaryAddress */
  owa.len = sizeof (L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress,
                          (xLibU8_t *) &objipMcastBoundaryAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryAddressValue, owa.len);

  owa.len = sizeof (xLibV4V6Mask_t);
  /* retrieve key: ipMcastBoundaryAddressPrefixLength */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength,
                          (xLibU8_t *) & objipMcastBoundaryAddressPrefixLengthValue, &owa.len);

  nextObjipMcastBoundaryIfIndexValue = objipMcastBoundaryIfIndexValue;
  nextObjipMcastBoundaryAddressTypeValue = objipMcastBoundaryAddressTypeValue;
  inetCopy(&nextObjipMcastBoundaryAddressValue, &objipMcastBoundaryAddressValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    inetAddressZeroSet(L7_AF_INET, &(nextObjipMcastBoundaryMaskValue));
    owa.l7rc =
      usmDbMcastMrouteBoundaryEntryNextGet (L7_UNIT_CURRENT, &nextObjipMcastBoundaryIfIndexValue,
                            &nextObjipMcastBoundaryAddressValue,
                            &nextObjipMcastBoundaryMaskValue);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastBoundaryAddressPrefixLengthValue, owa.len);

    if(objipMcastBoundaryAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
    {
      addressType = L7_AF_INET;
    }
    else
    {
      addressType = L7_AF_INET6;
    }
    inetMaskLenToMask(addressType, objipMcastBoundaryAddressPrefixLengthValue.addr, &nextObjipMcastBoundaryMaskValue);
    owa.l7rc = usmDbMcastMrouteBoundaryEntryNextGet (L7_UNIT_CURRENT,
                                    &nextObjipMcastBoundaryIfIndexValue,
                                    &nextObjipMcastBoundaryAddressValue,
                                    &nextObjipMcastBoundaryMaskValue);
  }

  if ((objipMcastBoundaryIfIndexValue != nextObjipMcastBoundaryIfIndexValue)
      || (objipMcastBoundaryAddressTypeValue != nextObjipMcastBoundaryAddressTypeValue)
      || (memcmp(&objipMcastBoundaryAddressValue, &nextObjipMcastBoundaryAddressValue, sizeof(nextObjipMcastBoundaryAddressValue)) != 0)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  inetMaskToMaskLen(&nextObjipMcastBoundaryMaskValue, &tmpVal);
  nextObjipMcastBoundaryAddressPrefixLengthValue.family = objipMcastBoundaryAddressTypeValue;
  nextObjipMcastBoundaryAddressPrefixLengthValue.addr = tmpVal;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastBoundaryAddressPrefixLengthValue, owa.len);

  /* return the object value: ipMcastBoundaryAddressPrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastBoundaryAddressPrefixLengthValue,
                           sizeof (objipMcastBoundaryAddressPrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryTimeStamp
*
* @purpose Get 'ipMcastBoundaryTimeStamp'
 *@description  [ipMcastBoundaryTimeStamp] The value of sysUpTime at which the
* multicast boundary information represented by this entry was
* learned by the router. If this information was present at the most
* recent re-initialization of the local management subsystem, then
* this object contains a zero value.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryTimeStamp (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastBoundaryIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryIfIndexValue;
  fpObjWa_t kwaipMcastBoundaryAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressTypeValue;
  fpObjWa_t kwaipMcastBoundaryAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastBoundaryAddressValue;
  fpObjWa_t kwaipMcastBoundaryAddressPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastBoundaryTimeStampValue;
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: ipMcastBoundaryIfIndex */
  kwaipMcastBoundaryIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                   (xLibU8_t *) & keyipMcastBoundaryIfIndexValue, &kwaipMcastBoundaryIfIndex.len);
  if (kwaipMcastBoundaryIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryIfIndex);
    return kwaipMcastBoundaryIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryIfIndexValue, kwaipMcastBoundaryIfIndex.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  kwaipMcastBoundaryAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                   (xLibU8_t *) & keyipMcastBoundaryAddressTypeValue,
                   &kwaipMcastBoundaryAddressType.len);
  if (kwaipMcastBoundaryAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressType);
    return kwaipMcastBoundaryAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressTypeValue,
                           kwaipMcastBoundaryAddressType.len);

  /* retrieve key: ipMcastBoundaryAddress */
  kwaipMcastBoundaryAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress,
                   (xLibU8_t *) &keyipMcastBoundaryAddressValue, &kwaipMcastBoundaryAddress.len);
  if (kwaipMcastBoundaryAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddress);
    return kwaipMcastBoundaryAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastBoundaryAddressValue, kwaipMcastBoundaryAddress.len);

  /* retrieve key: ipMcastBoundaryAddressPrefixLength */
  kwaipMcastBoundaryAddressPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength,
                   (xLibU8_t *) & keyipMcastBoundaryAddressPrefixLengthValue,
                   &kwaipMcastBoundaryAddressPrefixLength.len);
  if (kwaipMcastBoundaryAddressPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressPrefixLength);
    return kwaipMcastBoundaryAddressPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressPrefixLengthValue,
                           kwaipMcastBoundaryAddressPrefixLength.len);
  if(kwaipMcastBoundaryAddressType == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastBoundaryAddressPrefixLengthValue, &keyipMcastBoundaryMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastMrouteBoundaryEntryGet (L7_UNIT_CURRENT, keyipMcastBoundaryIfIndexValue,
                              &keyipMcastBoundaryAddressValue,
                              &keyipMcastBoundaryMaskValue,
                              &objipMcastBoundaryTimeStampValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastBoundaryTimeStamp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastBoundaryTimeStampValue,
                           sizeof (objipMcastBoundaryTimeStampValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryDroppedMcastOctets
*
* @purpose Get 'ipMcastBoundaryDroppedMcastOctets'
 *@description  [ipMcastBoundaryDroppedMcastOctets] The number of octets of
* multicast packets that have been dropped as a result of this zone
* boundary configuration. Discontinuities in this monotonically
* increasing value occur at re-initialization of the management system.
* Discontinuities can also occur as a result of boundary
* configuration being removed and replaced, which can be detected by observing
* the value of ipMcastBoundaryTimeStamp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryDroppedMcastOctets (void *wap,
                                                                                 void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastBoundaryIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryIfIndexValue;
  fpObjWa_t kwaipMcastBoundaryAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressTypeValue;
  fpObjWa_t kwaipMcastBoundaryAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastBoundaryAddressValue;
  fpObjWa_t kwaipMcastBoundaryAddressPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastBoundaryDroppedMcastOctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastBoundaryIfIndex */
  kwaipMcastBoundaryIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                   (xLibU8_t *) & keyipMcastBoundaryIfIndexValue, &kwaipMcastBoundaryIfIndex.len);
  if (kwaipMcastBoundaryIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryIfIndex);
    return kwaipMcastBoundaryIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryIfIndexValue, kwaipMcastBoundaryIfIndex.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  kwaipMcastBoundaryAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                   (xLibU8_t *) & keyipMcastBoundaryAddressTypeValue,
                   &kwaipMcastBoundaryAddressType.len);
  if (kwaipMcastBoundaryAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressType);
    return kwaipMcastBoundaryAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressTypeValue,
                           kwaipMcastBoundaryAddressType.len);

  /* retrieve key: ipMcastBoundaryAddress */
  kwaipMcastBoundaryAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress,
                   (xLibU8_t *) &keyipMcastBoundaryAddressValue, &kwaipMcastBoundaryAddress.len);
  if (kwaipMcastBoundaryAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddress);
    return kwaipMcastBoundaryAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastBoundaryAddressValue, kwaipMcastBoundaryAddress.len);

  /* retrieve key: ipMcastBoundaryAddressPrefixLength */
  kwaipMcastBoundaryAddressPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength,
                   (xLibU8_t *) & keyipMcastBoundaryAddressPrefixLengthValue,
                   &kwaipMcastBoundaryAddressPrefixLength.len);
  if (kwaipMcastBoundaryAddressPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressPrefixLength);
    return kwaipMcastBoundaryAddressPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressPrefixLengthValue,
                           kwaipMcastBoundaryAddressPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastBoundaryIfIndexValue,
                              keyipMcastBoundaryAddressTypeValue,
                              keyipMcastBoundaryAddressValue,
                              keyipMcastBoundaryAddressPrefixLengthValue,
                              &objipMcastBoundaryDroppedMcastOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastBoundaryDroppedMcastOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastBoundaryDroppedMcastOctetsValue,
                           sizeof (objipMcastBoundaryDroppedMcastOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryDroppedMcastPkts
*
* @purpose Get 'ipMcastBoundaryDroppedMcastPkts'
 *@description  [ipMcastBoundaryDroppedMcastPkts] The number of multicast
* packets that have been dropped as a result of this zone boundary
* configuration. Discontinuities in this monotonically increasing value
* occur at re-initialization of the management system.
* Discontinuities can also occur as a result of boundary configuration being
* removed and replaced, which can be detected by observing the value
* of ipMcastBoundaryTimeStamp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryDroppedMcastPkts (void *wap,
                                                                               void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastBoundaryIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryIfIndexValue;
  fpObjWa_t kwaipMcastBoundaryAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressTypeValue;
  fpObjWa_t kwaipMcastBoundaryAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastBoundaryAddressValue;
  fpObjWa_t kwaipMcastBoundaryAddressPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastBoundaryDroppedMcastPktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastBoundaryIfIndex */
  kwaipMcastBoundaryIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                   (xLibU8_t *) & keyipMcastBoundaryIfIndexValue, &kwaipMcastBoundaryIfIndex.len);
  if (kwaipMcastBoundaryIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryIfIndex);
    return kwaipMcastBoundaryIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryIfIndexValue, kwaipMcastBoundaryIfIndex.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  kwaipMcastBoundaryAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                   (xLibU8_t *) & keyipMcastBoundaryAddressTypeValue,
                   &kwaipMcastBoundaryAddressType.len);
  if (kwaipMcastBoundaryAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressType);
    return kwaipMcastBoundaryAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressTypeValue,
                           kwaipMcastBoundaryAddressType.len);

  /* retrieve key: ipMcastBoundaryAddress */
  kwaipMcastBoundaryAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress,
                   (xLibU8_t *) &keyipMcastBoundaryAddressValue, &kwaipMcastBoundaryAddress.len);
  if (kwaipMcastBoundaryAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddress);
    return kwaipMcastBoundaryAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastBoundaryAddressValue, kwaipMcastBoundaryAddress.len);

  /* retrieve key: ipMcastBoundaryAddressPrefixLength */
  kwaipMcastBoundaryAddressPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength,
                   (xLibU8_t *) & keyipMcastBoundaryAddressPrefixLengthValue,
                   &kwaipMcastBoundaryAddressPrefixLength.len);
  if (kwaipMcastBoundaryAddressPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressPrefixLength);
    return kwaipMcastBoundaryAddressPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressPrefixLengthValue,
                           kwaipMcastBoundaryAddressPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastBoundaryIfIndexValue,
                              keyipMcastBoundaryAddressTypeValue,
                              keyipMcastBoundaryAddressValue,
                              keyipMcastBoundaryAddressPrefixLengthValue,
                              &objipMcastBoundaryDroppedMcastPktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastBoundaryDroppedMcastPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastBoundaryDroppedMcastPktsValue,
                           sizeof (objipMcastBoundaryDroppedMcastPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryStatus
*
* @purpose Get 'ipMcastBoundaryStatus'
 *@description  [ipMcastBoundaryStatus] The status of this row, by which rows in
* this table can be created and destroyed. This status object can
* be set to active(1) without setting any other columnar objects in
* this entry. All writeable objects in this entry can be modified
* when the status of this entry is active(1).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastBoundaryIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryIfIndexValue;
  fpObjWa_t kwaipMcastBoundaryAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressTypeValue;
  fpObjWa_t kwaipMcastBoundaryAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastBoundaryAddressValue;
  L7_inet_addr_t keyipMcastBoundaryMaskValue;
  fpObjWa_t kwaipMcastBoundaryAddressPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keyipMcastBoundaryAddressPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastBoundaryStatusValue;
  xLibU32_t addressType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastBoundaryIfIndex */
  kwaipMcastBoundaryIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                   (xLibU8_t *) & keyipMcastBoundaryIfIndexValue, &kwaipMcastBoundaryIfIndex.len);
  if (kwaipMcastBoundaryIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryIfIndex);
    return kwaipMcastBoundaryIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryIfIndexValue, kwaipMcastBoundaryIfIndex.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  kwaipMcastBoundaryAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                   (xLibU8_t *) & keyipMcastBoundaryAddressTypeValue,
                   &kwaipMcastBoundaryAddressType.len);
  if (kwaipMcastBoundaryAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressType);
    return kwaipMcastBoundaryAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressTypeValue,
                           kwaipMcastBoundaryAddressType.len);

  /* retrieve key: ipMcastBoundaryAddress */
  kwaipMcastBoundaryAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress,
                   (xLibU8_t *) &keyipMcastBoundaryAddressValue, &kwaipMcastBoundaryAddress.len);
  if (kwaipMcastBoundaryAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddress);
    return kwaipMcastBoundaryAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressValue, kwaipMcastBoundaryAddress.len);

  /* retrieve key: ipMcastBoundaryAddressPrefixLength */
  kwaipMcastBoundaryAddressPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength,
                   (xLibU8_t *) & keyipMcastBoundaryAddressPrefixLengthValue,
                   &kwaipMcastBoundaryAddressPrefixLength.len);
  if (kwaipMcastBoundaryAddressPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressPrefixLength);
    return kwaipMcastBoundaryAddressPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressPrefixLengthValue,
                           kwaipMcastBoundaryAddressPrefixLength.len);

  if(keyipMcastBoundaryAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastBoundaryAddressPrefixLengthValue.addr, &keyipMcastBoundaryMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastMrouteBoundaryEntryGet (L7_UNIT_CURRENT, keyipMcastBoundaryIfIndexValue,
                              &keyipMcastBoundaryAddressValue,
                              &keyipMcastBoundaryMaskValue);
                              
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_MROUTE_BOUNDARY_ENTRY_GET_FAILED;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objipMcastBoundaryStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: ipMcastBoundaryStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastBoundaryStatusValue,
                           sizeof (objipMcastBoundaryStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastBoundaryTable_ipMcastBoundaryStatus
*
* @purpose Set 'ipMcastBoundaryStatus'
 *@description  [ipMcastBoundaryStatus] The status of this row, by which rows in
* this table can be created and destroyed. This status object can
* be set to active(1) without setting any other columnar objects in
* this entry. All writeable objects in this entry can be modified
* when the status of this entry is active(1).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastBoundaryTable_ipMcastBoundaryStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastBoundaryStatusValue;

  fpObjWa_t kwaipMcastBoundaryIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryIfIndexValue;
  fpObjWa_t kwaipMcastBoundaryAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressTypeValue;
  fpObjWa_t kwaipMcastBoundaryAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastBoundaryAddressValue;
  L7_inet_addr_t keyipMcastBoundaryMaskValue;
  fpObjWa_t kwaipMcastBoundaryAddressPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keyipMcastBoundaryAddressPrefixLengthValue;
  xLibU32_t addressType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastBoundaryStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastBoundaryStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastBoundaryStatusValue, owa.len);
  /* retrieve key: ipMcastBoundaryIfIndex */
  kwaipMcastBoundaryIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                   (xLibU8_t *) & keyipMcastBoundaryIfIndexValue, &kwaipMcastBoundaryIfIndex.len);
  if (kwaipMcastBoundaryIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryIfIndex);
    return kwaipMcastBoundaryIfIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryIfIndexValue, kwaipMcastBoundaryIfIndex.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  kwaipMcastBoundaryAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                   (xLibU8_t *) & keyipMcastBoundaryAddressTypeValue,
                   &kwaipMcastBoundaryAddressType.len);
  if (kwaipMcastBoundaryAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressType);
    return kwaipMcastBoundaryAddressType.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressTypeValue,
                           kwaipMcastBoundaryAddressType.len);

  /* retrieve key: ipMcastBoundaryAddress */
  kwaipMcastBoundaryAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress,
                   (xLibU8_t *) &keyipMcastBoundaryAddressValue, &kwaipMcastBoundaryAddress.len);
  if (kwaipMcastBoundaryAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddress);
    return kwaipMcastBoundaryAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressValue, kwaipMcastBoundaryAddress.len);



  if ((keyipMcastBoundaryAddressValue.addr.ipv4.s_addr < L7_MCAST_ADMINSCOPE_ADDR_MIN) ||
      (keyipMcastBoundaryAddressValue.addr.ipv4.s_addr > L7_MCAST_ADMINSCOPE_ADDR_MAX))
  {
    owa.rc = XLIBRC_MCAST_ADDRESS_NOT_IN_ADMIN_BOUNDARY;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: ipMcastBoundaryAddressPrefixLength */
  kwaipMcastBoundaryAddressPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength,
                   (xLibU8_t *) & keyipMcastBoundaryAddressPrefixLengthValue,
                   &kwaipMcastBoundaryAddressPrefixLength.len);
  if (kwaipMcastBoundaryAddressPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressPrefixLength);
    return kwaipMcastBoundaryAddressPrefixLength.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressPrefixLengthValue,
                           kwaipMcastBoundaryAddressPrefixLength.len);

  if(keyipMcastBoundaryAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastBoundaryAddressPrefixLengthValue.addr, &keyipMcastBoundaryMaskValue);

  owa.l7rc = L7_SUCCESS;
  if (objipMcastBoundaryStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbMcastMrouteBoundaryAdd (L7_UNIT_CURRENT, keyipMcastBoundaryIfIndexValue,
                                &keyipMcastBoundaryAddressValue,
                                &keyipMcastBoundaryMaskValue);
  }
  else if (objipMcastBoundaryStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbMcastMrouteBoundaryDelete (L7_UNIT_CURRENT, keyipMcastBoundaryIfIndexValue,
                                   &keyipMcastBoundaryAddressValue,
                                   &keyipMcastBoundaryMaskValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryStorageType
*
* @purpose Get 'ipMcastBoundaryStorageType'
 *@description  [ipMcastBoundaryStorageType] The storage type for this row. Rows
* having the value 'permanent' need not allow write-access to any
* columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastBoundaryTable_ipMcastBoundaryStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastBoundaryIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryIfIndexValue;
  fpObjWa_t kwaipMcastBoundaryAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressTypeValue;
  fpObjWa_t kwaipMcastBoundaryAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastBoundaryAddressValue;
  fpObjWa_t kwaipMcastBoundaryAddressPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastBoundaryStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastBoundaryIfIndex */
  kwaipMcastBoundaryIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                   (xLibU8_t *) & keyipMcastBoundaryIfIndexValue, &kwaipMcastBoundaryIfIndex.len);
  if (kwaipMcastBoundaryIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryIfIndex);
    return kwaipMcastBoundaryIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryIfIndexValue, kwaipMcastBoundaryIfIndex.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  kwaipMcastBoundaryAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                   (xLibU8_t *) & keyipMcastBoundaryAddressTypeValue,
                   &kwaipMcastBoundaryAddressType.len);
  if (kwaipMcastBoundaryAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressType);
    return kwaipMcastBoundaryAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressTypeValue,
                           kwaipMcastBoundaryAddressType.len);

  /* retrieve key: ipMcastBoundaryAddress */
  kwaipMcastBoundaryAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress,
                   (xLibU8_t *) &keyipMcastBoundaryAddressValue, &kwaipMcastBoundaryAddress.len);
  if (kwaipMcastBoundaryAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddress);
    return kwaipMcastBoundaryAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastBoundaryAddressValue, kwaipMcastBoundaryAddress.len);

  /* retrieve key: ipMcastBoundaryAddressPrefixLength */
  kwaipMcastBoundaryAddressPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength,
                   (xLibU8_t *) & keyipMcastBoundaryAddressPrefixLengthValue,
                   &kwaipMcastBoundaryAddressPrefixLength.len);
  if (kwaipMcastBoundaryAddressPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressPrefixLength);
    return kwaipMcastBoundaryAddressPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressPrefixLengthValue,
                           kwaipMcastBoundaryAddressPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastBoundaryIfIndexValue,
                              keyipMcastBoundaryAddressTypeValue,
                              keyipMcastBoundaryAddressValue,
                              keyipMcastBoundaryAddressPrefixLengthValue,
                              &objipMcastBoundaryStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastBoundaryStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastBoundaryStorageTypeValue,
                           sizeof (objipMcastBoundaryStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastBoundaryTable_ipMcastBoundaryStorageType
*
* @purpose Set 'ipMcastBoundaryStorageType'
 *@description  [ipMcastBoundaryStorageType] The storage type for this row. Rows
* having the value 'permanent' need not allow write-access to any
* columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastBoundaryTable_ipMcastBoundaryStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastBoundaryStorageTypeValue;

  fpObjWa_t kwaipMcastBoundaryIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryIfIndexValue;
  fpObjWa_t kwaipMcastBoundaryAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressTypeValue;
  fpObjWa_t kwaipMcastBoundaryAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastBoundaryAddressValue;
  fpObjWa_t kwaipMcastBoundaryAddressPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastBoundaryAddressPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastBoundaryStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastBoundaryStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastBoundaryStorageTypeValue, owa.len);

  /* retrieve key: ipMcastBoundaryIfIndex */
  kwaipMcastBoundaryIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryIfIndex,
                   (xLibU8_t *) & keyipMcastBoundaryIfIndexValue, &kwaipMcastBoundaryIfIndex.len);
  if (kwaipMcastBoundaryIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryIfIndex);
    return kwaipMcastBoundaryIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryIfIndexValue, kwaipMcastBoundaryIfIndex.len);

  /* retrieve key: ipMcastBoundaryAddressType */
  kwaipMcastBoundaryAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressType,
                   (xLibU8_t *) & keyipMcastBoundaryAddressTypeValue,
                   &kwaipMcastBoundaryAddressType.len);
  if (kwaipMcastBoundaryAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressType);
    return kwaipMcastBoundaryAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressTypeValue,
                           kwaipMcastBoundaryAddressType.len);

  /* retrieve key: ipMcastBoundaryAddress */
  kwaipMcastBoundaryAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddress,
                   (xLibU8_t *) &keyipMcastBoundaryAddressValue, &kwaipMcastBoundaryAddress.len);
  if (kwaipMcastBoundaryAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddress);
    return kwaipMcastBoundaryAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastBoundaryAddressValue, kwaipMcastBoundaryAddress.len);

  /* retrieve key: ipMcastBoundaryAddressPrefixLength */
  kwaipMcastBoundaryAddressPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastBoundaryTable_ipMcastBoundaryAddressPrefixLength,
                   (xLibU8_t *) & keyipMcastBoundaryAddressPrefixLengthValue,
                   &kwaipMcastBoundaryAddressPrefixLength.len);
  if (kwaipMcastBoundaryAddressPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastBoundaryAddressPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastBoundaryAddressPrefixLength);
    return kwaipMcastBoundaryAddressPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastBoundaryAddressPrefixLengthValue,
                           kwaipMcastBoundaryAddressPrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyipMcastBoundaryIfIndexValue,
                              keyipMcastBoundaryAddressTypeValue,
                              keyipMcastBoundaryAddressValue,
                              keyipMcastBoundaryAddressPrefixLengthValue,
                              objipMcastBoundaryStorageTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}
