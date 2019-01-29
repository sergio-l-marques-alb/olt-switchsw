
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastipMcastRouteTable.c
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
#include "_xe_ipmcastipMcastRouteTable_obj.h"
#include "usmdb_mib_mcast_api.h"
#include "usmdb_pimsm_api.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_pim_rfc5060_api.h"
#include "usmdb_mib_pimsm_api.h"


xLibRC_t fpObjSet_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType (void *wap, void *bufp)
{
    return XLIBRC_SUCCESS;
}
xLibRC_t fpObjSet_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType (void *wap, void *bufp)
{
    return XLIBRC_SUCCESS;
}
xLibRC_t fpObjSet_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength (void *wap, void *bufp)
{
    return XLIBRC_SUCCESS;
}
xLibRC_t fpObjSet_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength (void *wap, void *bufp)
{
    return XLIBRC_SUCCESS;
}
/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteGroup
*
* @purpose Get 'ipMcastRouteGroup'
 *@description  [ipMcastRouteGroup] The IP multicast group address which, when
* combined with the corresponding value specified in
* ipMcastRouteGroupPrefixLength, identifies the groups for which this entry
* contains multicast routing information. This address object is only
* significant up to ipMcastRouteGroupPrefixLength bits. The remaining
* address bits are set to zero. This is especially important for
* this index field, which is part of the index of this entry. Any
* non-zero bits would signify an entirely different entry. For
* addresses of type ipv4z or ipv6z, the appended zone index is significant
* even though it lies beyond the prefix length. The use of these
* address types indicate that this forwarding state applies only
* within the given zone. Zone index zero is not valid in this table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteGroup (void *wap, void *bufp)
{

  xLibU32_t objipMcastRouteGroupAddressTypeValue;
  L7_inet_addr_t objipMcastRouteGroupValue;
  L7_inet_addr_t nextObjipMcastRouteGroupValue;
  xLibU32_t      objipMcastRouteSourcePrefixLengthValue;
  L7_inet_addr_t objipMcastRouteSourceMaskValue;
  L7_inet_addr_t nextObjipMcastRouteSourceMaskValue;
  L7_inet_addr_t nextObjipMcastRouteSourceValue;
  xLibU32_t addressType;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                          (xLibU8_t *) & objipMcastRouteGroupAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteGroupAddressTypeValue, owa.len);

  if(objipMcastRouteGroupAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                          (xLibU8_t *) & objipMcastRouteSourcePrefixLengthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteSourcePrefixLengthValue, owa.len);

  /* retrieve key: ipMcastRouteGroup */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                          (xLibU8_t *) &objipMcastRouteGroupValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    inetAddressZeroSet(addressType, &nextObjipMcastRouteGroupValue);
    inetAddressZeroSet(addressType, &nextObjipMcastRouteSourceValue);
    inetMaskLenToMask(addressType, objipMcastRouteSourcePrefixLengthValue, &objipMcastRouteSourceMaskValue);
    memcpy(&nextObjipMcastRouteSourceMaskValue, &objipMcastRouteSourceMaskValue, sizeof(L7_inet_addr_t));

    if(usmDbPimsmIsOperational(L7_UNIT_CURRENT, addressType) == L7_TRUE)
    {
      owa.l7rc = usmDbPimSGEntryNextGet (addressType, &nextObjipMcastRouteGroupValue,
          &nextObjipMcastRouteSourceValue);
    }
    else
    {
      owa.l7rc = usmDbMcastIpMRouteEntryNextGet (addressType, L7_UNIT_CURRENT, &nextObjipMcastRouteGroupValue,
          &nextObjipMcastRouteSourceValue, &nextObjipMcastRouteSourceMaskValue);
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteGroupValue, owa.len);

    memcpy(&nextObjipMcastRouteGroupValue, &objipMcastRouteGroupValue, sizeof(L7_inet_addr_t));
    inetAddressZeroSet(addressType, &nextObjipMcastRouteSourceValue);
    inetMaskLenToMask(addressType, objipMcastRouteSourcePrefixLengthValue, &objipMcastRouteSourceMaskValue);
    memcpy(&nextObjipMcastRouteSourceMaskValue, &objipMcastRouteSourceMaskValue, sizeof(L7_inet_addr_t));

    if(usmDbPimsmIsOperational(L7_UNIT_CURRENT, addressType) == L7_TRUE)
    {
      do
      {

        owa.l7rc = usmDbPimSGEntryNextGet (addressType, &nextObjipMcastRouteGroupValue,
            &nextObjipMcastRouteSourceValue);
      }while ((memcmp(&nextObjipMcastRouteGroupValue, &objipMcastRouteGroupValue, sizeof(L7_inet_addr_t))==0)
          && (owa.l7rc == L7_SUCCESS));

    }
    else
    {
      do
      {
        owa.l7rc = usmDbMcastIpMRouteEntryNextGet (addressType, L7_UNIT_CURRENT,
            &nextObjipMcastRouteGroupValue,
            &nextObjipMcastRouteSourceValue,
            &nextObjipMcastRouteSourceMaskValue);
      }while ((memcmp(&nextObjipMcastRouteGroupValue, &objipMcastRouteGroupValue, sizeof(L7_inet_addr_t))==0)
          && (owa.l7rc == L7_SUCCESS));
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastRouteGroupValue, owa.len);

  /* return the object value: ipMcastRouteGroup */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjipMcastRouteGroupValue,
                           sizeof (objipMcastRouteGroupValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteSource
*
* @purpose Get 'ipMcastRouteSource'
 *@description  [ipMcastRouteSource] The network address which, when combined
* with the corresponding value of ipMcastRouteSourcePrefixLength,
* identifies the sources for which this entry contains multicast
* routing information. This address object is only significant up to
* ipMcastRouteSourcePrefixLength bits. The remaining address bits are
* set to zero. This is especially important for this index field,
* which is part of the index of this entry. Any non-zero bits would
* signify an entirely different entry. For addresses of type ipv4z
* or ipv6z, the appended zone index is significant even though it
* lies beyond the prefix length. The use of these address types
* indicate that this source address applies only within the given zone.
* Zone index zero is not valid in this table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteSource (void *wap, void *bufp)
{
  L7_inet_addr_t objipMcastRouteGroupValue;
  L7_inet_addr_t nextObjipMcastRouteGroupValue;
  xLibU32_t      objipMcastRouteSourceAddressTypeValue;
  xLibU32_t      objipMcastRouteSourcePrefixLengthValue;
  L7_inet_addr_t objipMcastRouteSourceValue;
  L7_inet_addr_t nextObjipMcastRouteSourceValue;
  L7_inet_addr_t objipMcastRouteSourceMaskValue;
  L7_inet_addr_t nextObjipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteSourceAddressType */
  owa.len = sizeof(xLibU32_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                          (xLibU8_t *) & objipMcastRouteSourceAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteSourceAddressTypeValue, owa.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                          (xLibU8_t *) & objipMcastRouteSourcePrefixLengthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteSourcePrefixLengthValue, owa.len);

  /* retrieve key: ipMcastRouteGroup */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                          (xLibU8_t *) &objipMcastRouteGroupValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteGroupValue, owa.len);

  /* retrieve key: ipMcastRouteSource */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                          (xLibU8_t *) &objipMcastRouteSourceValue, &owa.len);
  if(objipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memcpy(&nextObjipMcastRouteGroupValue, &objipMcastRouteGroupValue, sizeof(L7_inet_addr_t));
    inetAddressZeroSet(addressType, &nextObjipMcastRouteSourceValue);
    inetMaskLenToMask(addressType, objipMcastRouteSourcePrefixLengthValue, &objipMcastRouteSourceMaskValue);
    memcpy(&nextObjipMcastRouteSourceMaskValue, &objipMcastRouteSourceMaskValue, sizeof(L7_inet_addr_t));

    if(usmDbPimsmIsOperational(L7_UNIT_CURRENT, addressType) == L7_TRUE)
    {
      owa.l7rc = usmDbPimSGEntryNextGet (addressType, &nextObjipMcastRouteGroupValue,
          &nextObjipMcastRouteSourceValue);
    }
    else
    {
      owa.l7rc = usmDbMcastIpMRouteEntryNextGet (addressType, L7_UNIT_CURRENT, &nextObjipMcastRouteGroupValue,
          &nextObjipMcastRouteSourceValue, &nextObjipMcastRouteSourceMaskValue);
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteSourceValue, owa.len);

    memcpy(&nextObjipMcastRouteGroupValue, &objipMcastRouteGroupValue, sizeof(L7_inet_addr_t));
    memcpy(&nextObjipMcastRouteSourceValue, &objipMcastRouteSourceValue, sizeof(L7_inet_addr_t));
    inetMaskLenToMask(addressType, objipMcastRouteSourcePrefixLengthValue, &objipMcastRouteSourceMaskValue);
    memcpy(&nextObjipMcastRouteSourceMaskValue, &objipMcastRouteSourceMaskValue, sizeof(L7_inet_addr_t));

    if(usmDbPimsmIsOperational(L7_UNIT_CURRENT, addressType) == L7_TRUE)
    {
      do
      {
        owa.l7rc = usmDbPimSGEntryNextGet (addressType, &nextObjipMcastRouteGroupValue,
            &nextObjipMcastRouteSourceValue);
      }while ((memcmp(&nextObjipMcastRouteGroupValue, &objipMcastRouteGroupValue, sizeof(L7_inet_addr_t))==0)
          && (memcmp(&nextObjipMcastRouteSourceValue, &objipMcastRouteSourceValue, sizeof(L7_inet_addr_t)) == 0)
          && (owa.l7rc == L7_SUCCESS));
    }
    else
    {
      do
      {
        owa.l7rc = usmDbMcastIpMRouteEntryNextGet (addressType, L7_UNIT_CURRENT,
            &nextObjipMcastRouteGroupValue,
            &nextObjipMcastRouteSourceValue,
            &nextObjipMcastRouteSourceMaskValue);
      }while ((memcmp(&nextObjipMcastRouteGroupValue, &objipMcastRouteGroupValue, sizeof(L7_inet_addr_t))==0)
          && (memcmp(&nextObjipMcastRouteSourceValue, &objipMcastRouteSourceValue, sizeof(L7_inet_addr_t)) == 0)
          && (owa.l7rc == L7_SUCCESS));
    }
  }

  if((memcmp(&nextObjipMcastRouteGroupValue, &objipMcastRouteGroupValue, sizeof(L7_inet_addr_t))) || owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastRouteSourceValue, owa.len);

  /* return the object value: ipMcastRouteSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjipMcastRouteSourceValue,
                           sizeof (objipMcastRouteSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteUpstreamNeighborType
*
* @purpose Get 'ipMcastRouteUpstreamNeighborType'
 *@description  [ipMcastRouteUpstreamNeighborType] A value indicating the
* address family of the address contained in
* ipMcastRouteUpstreamNeighbor. An address type of unknown(0) indicates that the upstream
* neighbor is unknown, for example in BIDIR-PIM.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteUpstreamNeighborType (void *wap, void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  L7_inet_addr_t objipMcastRouteUpstreamNeighbor;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteUpstreamNeighborTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteUpstreamNbrGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteUpstreamNeighbor);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objipMcastRouteUpstreamNeighbor.family == L7_AF_INET)
  {
    objipMcastRouteUpstreamNeighborTypeValue = L7_INET_ADDR_TYPE_IPV4;
  }
  else
  {
    objipMcastRouteUpstreamNeighborTypeValue = L7_INET_ADDR_TYPE_IPV6;
  }
  /* return the object value: ipMcastRouteUpstreamNeighborType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteUpstreamNeighborTypeValue,
                           sizeof (objipMcastRouteUpstreamNeighborTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteUpstreamNeighbor
*
* @purpose Get 'ipMcastRouteUpstreamNeighbor'
 *@description  [ipMcastRouteUpstreamNeighbor] The address of the upstream
* neighbor (for example, RPF neighbor) from which IP datagrams from
* these sources to this multicast address are received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteUpstreamNeighbor (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objipMcastRouteUpstreamNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteUpstreamNbrGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteUpstreamNeighborValue);
  objipMcastRouteUpstreamNeighborValue.family = addressType;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteUpstreamNeighbor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objipMcastRouteUpstreamNeighborValue,
                           sizeof (objipMcastRouteUpstreamNeighborValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteInIfIndex
*
* @purpose Get 'ipMcastRouteInIfIndex'
 *@description  [ipMcastRouteInIfIndex] The value of ifIndex for the interface
* on which IP datagrams sent by these sources to this multicast
* address are received. A value of 0 indicates that datagrams are not
* subject to an incoming interface check, but may be accepted on
* multiple interfaces (for example, in BIDIR-PIM).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteInIfIndex (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteInIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteIfIndexGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteInIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteInIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteInIfIndexValue,
                           sizeof (objipMcastRouteInIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteTimeStamp
*
* @purpose Get 'ipMcastRouteTimeStamp'
 *@description  [ipMcastRouteTimeStamp] The value of sysUpTime at which the
* multicast routing information represented by this entry was learned
* by the router. If this information was present at the most recent
* re-initialization of the local management subsystem, then this
* object contains a zero value.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteTimeStamp (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;
  L7_timespec timeSpec;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t mcastRouteTimeStampValue;
  xLibStr256_t objipMcastRouteTimeStampValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteUpTimeGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &mcastRouteTimeStampValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiConvertRawUpTime(mcastRouteTimeStampValue, (L7_timespec *)&timeSpec);
  osapiSnprintf(objipMcastRouteTimeStampValue, sizeof(objipMcastRouteTimeStampValue), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);

  /* return the object value: ipMcastRouteTimeStamp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteTimeStampValue,
                           sizeof (objipMcastRouteTimeStampValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteExpiryTime
*
* @purpose Get 'ipMcastRouteExpiryTime'
 *@description  [ipMcastRouteExpiryTime] The minimum amount of time remaining
* before this entry will be aged out. The value 0 indicates that the
* entry is not subject to aging. If ipMcastRouteNextHopState is
* pruned(1), this object represents the remaining time until the prune
* expires. If this timer expires, state reverts to forwarding(2).
* Otherwise, this object represents the time until this entry is
* removed from the table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteExpiryTime (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objipMcastRouteExpiryTimeValue;
  xLibU32_t mcastRouteExpiryTimeValue;
  L7_timespec timeSpec;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteExpiryGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &mcastRouteExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiConvertRawUpTime(mcastRouteExpiryTimeValue, (L7_timespec *)&timeSpec);
  osapiSnprintf(objipMcastRouteExpiryTimeValue, sizeof(objipMcastRouteExpiryTimeValue), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);
  /* return the object value: ipMcastRouteExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteExpiryTimeValue,
                           sizeof (objipMcastRouteExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRpfAddress
*
* @purpose Get 'ipMcastRouteRpfAddress'
 *@description  [ipMcastRouteRpfAddress] Route RPF Address   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRpfAddress (void *wap, void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_inet_addr_t objipMcastRouteRpfAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);


  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMrouteRpfAddressGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteRpfAddressValue);
  objipMcastRouteRpfAddressValue.family = addressType;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteRpfAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteRpfAddressValue,
                           sizeof (objipMcastRouteRpfAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteFlags
*
* @purpose Get 'ipMcastRouteFlags'
 *@description  [ipMcastRouteFlags] Route Flags   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteFlags (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteFlagsValue;
  xLibU32_t objipMcastRouteProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  if (L7_SUCCESS == usmDbMcastIpMRouteProtocolGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteProtocolValue))
  {
    if (objipMcastRouteProtocolValue == L7_MCAST_IANA_MROUTE_PIM_SM )
    {
      /* get the value from application */
      owa.l7rc = usmDbMcastIpMRouteFlagsGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteFlagsValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

      /* return the object value: ipMcastRouteFlags */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteFlagsValue,
                           sizeof (objipMcastRouteFlagsValue));
    }
    else
    {
    objipMcastRouteFlagsValue = L7_XUI_MROUTE_FLAG_DASH;
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteFlagsValue,
                           sizeof (objipMcastRouteFlagsValue));
    }
  }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteProtocol
*
* @purpose Get 'ipMcastRouteProtocol'
 *@description  [ipMcastRouteProtocol] The multicast routing protocol via which
* this multicast forwarding entry was learned.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteProtocol (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteProtocolGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (addressType == L7_AF_INET6 &&
      objipMcastRouteProtocolValue == L7_XUI_MCAST_IANA_MROUTE_IGMP_PROXY)
  {
    objipMcastRouteProtocolValue =  L7_XUI_MCAST_IANA_MROUTE_MLD_PROXY;
  }

   /* return the object value: ipMcastRouteProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteProtocolValue,
                           sizeof (objipMcastRouteProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup
*
* @purpose Get 'ipMcastRouteStarGGroup'
 *@description  [ipMcastRouteStarGGroup] The IP multicast group address which, when
* combined with the corresponding value specified in
* ipMcastRouteGroupPrefixLength, identifies the groups for which this entry
* contains multicast routing information. This address object is only
* significant up to ipMcastRouteGroupPrefixLength bits. The remaining
* address bits are set to zero. This is especially important for
* this index field, which is part of the index of this entry. Any
* non-zero bits would signify an entirely different entry. For
* addresses of type ipv4z or ipv6z, the appended zone index is significant
* even though it lies beyond the prefix length. The use of these
* address types indicate that this forwarding state applies only
* within the given zone. Zone index zero is not valid in this table.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup (void *wap, void *bufp)
{

  xLibU32_t objipMcastRouteGroupAddressTypeValue;
  L7_inet_addr_t objipMcastRouteStarGGroupValue;
  L7_inet_addr_t nextObjipMcastRouteStarGGroupValue;
  xLibU32_t addressType;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                          (xLibU8_t *) & objipMcastRouteGroupAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteGroupAddressTypeValue, owa.len);

  if(objipMcastRouteGroupAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  /* retrieve key: ipMcastRouteStarGGroup */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup,
                          (xLibU8_t *) &objipMcastRouteStarGGroupValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    inetAddressZeroSet(addressType, &nextObjipMcastRouteStarGGroupValue);
    owa.l7rc = usmDbPimsmStarGEntryNextGet (addressType, &nextObjipMcastRouteStarGGroupValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastRouteStarGGroupValue, owa.len);

    memcpy(&nextObjipMcastRouteStarGGroupValue, &objipMcastRouteStarGGroupValue, sizeof(L7_inet_addr_t));
    do
    {
      owa.l7rc = usmDbPimsmStarGEntryNextGet (addressType, &nextObjipMcastRouteStarGGroupValue);
    }while ((memcmp(&nextObjipMcastRouteStarGGroupValue, &objipMcastRouteStarGGroupValue, sizeof(L7_inet_addr_t))==0)
          && (owa.l7rc == L7_SUCCESS));
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastRouteStarGGroupValue, owa.len);

  /* return the object value: ipMcastRouteStarGGroup */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjipMcastRouteStarGGroupValue,
                           sizeof (objipMcastRouteStarGGroupValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGInIfIndex
*
* @purpose Get 'ipMcastRouteStarGInIfIndex'
 *@description  [ipMcastRouteStarGInIfIndex] The value of ifIndex for the interface
* on which IP datagrams sent by these sources to this multicast
* address are received. A value of 0 indicates that datagrams are not
* subject to an incoming interface check, but may be accepted on
* multiple interfaces (for example, in BIDIR-PIM).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGInIfIndex (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteStarGGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteStarGGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteStarGInIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteStarGGroup */
  kwaipMcastRouteStarGGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup,
                                           (xLibU8_t *) &keyipMcastRouteStarGGroupValue,
                                           &kwaipMcastRouteStarGGroup.len);
  if (kwaipMcastRouteStarGGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteStarGGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteStarGGroup);
    return kwaipMcastRouteStarGGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteStarGGroupValue, kwaipMcastRouteStarGGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  inetAddressZeroSet(addressType, &keyipMcastRouteSourceValue);
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteIfIndexGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteStarGGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteStarGInIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteStarGInIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteStarGInIfIndexValue,
                           sizeof (objipMcastRouteStarGInIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGOutIntfEntry
*
* @purpose Get 'ipMcastRouteStarGOutIntfEntry'
 *@description  [ipMcastRouteStarGOutIntfEntry] The outgoing interface for the given
* source address, group address and source mask   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGOutIntfEntry (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteStarGGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteStarGGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteStarGOutIntfEntryValue;
  xLibStr256_t objipMcastRouteOutIntfList;
  L7_INTF_MASK_t      outIntIfMask;
  xLibU32_t oifListStr[L7_CLI_MAX_STRING_LENGTH];
  xLibU32_t numOifs = 0;
  xLibU32_t outIntIfNum, flag = 1;
  xLibS8_t tempBuf[10];
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteStarGGroup */
  kwaipMcastRouteStarGGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup,
                                           (xLibU8_t *) &keyipMcastRouteStarGGroupValue,
                                           &kwaipMcastRouteStarGGroup.len);
  if (kwaipMcastRouteStarGGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteStarGGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteStarGGroup);
    return kwaipMcastRouteStarGGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteStarGGroupValue, kwaipMcastRouteStarGGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  inetAddressZeroSet(addressType, &keyipMcastRouteSourceValue);
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  objipMcastRouteStarGOutIntfEntryValue = 0;
  if (usmDbPimsmIsOperational(L7_UNIT_CURRENT, addressType) != L7_TRUE)
  {
    memset(objipMcastRouteOutIntfList, 0, sizeof(xLibStr256_t));
    do
    {
      if(L7_SUCCESS == usmDbMcastIpMRouteOutIntfEntryNextGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteStarGGroupValue,
          &keyipMcastRouteSourceValue,
          &keyipMcastRouteSourceMaskValue,
          &objipMcastRouteStarGOutIntfEntryValue))
      {
        memset (tempBuf, 0x0, sizeof (tempBuf));
        if (flag == 1)
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf), "%d", objipMcastRouteStarGOutIntfEntryValue);
          flag = 0;
        }
        else
        {
          tempBuf[0] = ',';
          osapiSnprintf(&tempBuf[1], (sizeof(tempBuf)-1), "%d", objipMcastRouteStarGOutIntfEntryValue);
        }
        osapiStrncat(objipMcastRouteOutIntfList, tempBuf, sizeof(tempBuf));
      }
      else
        break;
    }while(1);
  }
  else
  {
    memset(objipMcastRouteOutIntfList, 0, sizeof(xLibStr256_t));
    memset(&outIntIfMask, 0, sizeof(L7_INTF_MASK_t));
    usmDbPimsmMapOIFGet(addressType, &keyipMcastRouteStarGGroupValue, 
                        &keyipMcastRouteSourceValue, &keyipMcastRouteSourceMaskValue,
                        &outIntIfMask);
    memset(oifListStr, 0, sizeof(oifListStr));
    usmDbConvertMaskToList(&outIntIfMask, oifListStr, &numOifs);
    for (outIntIfNum = 1; outIntIfNum <= numOifs; outIntIfNum++)
    {
      memset (tempBuf, 0x0, sizeof (tempBuf));
      if (flag == 1)
      {
       osapiSnprintf(tempBuf, sizeof(tempBuf), "%d", oifListStr[outIntIfNum]);
       flag = 0;
      }
      else
      {
        tempBuf[0] = ','; 
        osapiSnprintf(&tempBuf[1], (sizeof(tempBuf)-1), "%d", oifListStr[outIntIfNum]);
      }
      osapiStrncat(objipMcastRouteOutIntfList, tempBuf, sizeof(tempBuf));
    }
  }

  /* return the object value: ipMcastRouteStarGOutIntfEntry */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objipMcastRouteOutIntfList,
                           sizeof (objipMcastRouteOutIntfList));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGTimeStamp
*
* @purpose Get 'ipMcastRouteStarGTimeStamp'
 *@description  [ipMcastRouteStarGTimeStamp] The value of sysUpTime at which the
* multicast routing information represented by this entry was learned
* by the router. If this information was present at the most recent
* re-initialization of the local management subsystem, then this
* object contains a zero value.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGTimeStamp (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteStarGGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteStarGGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;
  L7_timespec timeSpec;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t mcastRouteStarGTimeStampValue;
  xLibStr256_t objipMcastRouteStarGTimeStampValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteStarGGroup */
  kwaipMcastRouteStarGGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup,
                                           (xLibU8_t *) &keyipMcastRouteStarGGroupValue,
                                           &kwaipMcastRouteStarGGroup.len);
  if (kwaipMcastRouteStarGGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteStarGGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteStarGGroup);
    return kwaipMcastRouteStarGGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteStarGGroupValue, kwaipMcastRouteStarGGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  inetAddressZeroSet(addressType, &keyipMcastRouteSourceValue);
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteUpTimeGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteStarGGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &mcastRouteStarGTimeStampValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiConvertRawUpTime(mcastRouteStarGTimeStampValue, (L7_timespec *)&timeSpec);
  osapiSnprintf(objipMcastRouteStarGTimeStampValue, sizeof(objipMcastRouteStarGTimeStampValue), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);

  /* return the object value: ipMcastRouteStarGTimeStamp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteStarGTimeStampValue,
                           sizeof (objipMcastRouteStarGTimeStampValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGExpiryTime
*
* @purpose Get 'ipMcastRouteStarGExpiryTime'
 *@description  [ipMcastRouteStarGExpiryTime] The minimum amount of time remaining
* before this entry will be aged out. The value 0 indicates that the
* entry is not subject to aging. If ipMcastRouteNextHopState is
* pruned(1), this object represents the remaining time until the prune
* expires. If this timer expires, state reverts to forwarding(2).
* Otherwise, this object represents the time until this entry is
* removed from the table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGExpiryTime (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteStarGGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteStarGGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objipMcastRouteStarGExpiryTimeValue;
  xLibU32_t mcastRouteStarGExpiryTimeValue;
  L7_timespec timeSpec;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteStarGGroup */
  kwaipMcastRouteStarGGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup,
                                           (xLibU8_t *) &keyipMcastRouteStarGGroupValue,
                                           &kwaipMcastRouteStarGGroup.len);
  if (kwaipMcastRouteStarGGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteStarGGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteStarGGroup);
    return kwaipMcastRouteStarGGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteStarGGroupValue, kwaipMcastRouteStarGGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  inetAddressZeroSet(addressType, &keyipMcastRouteSourceValue);
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteExpiryGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteStarGGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &mcastRouteStarGExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiConvertRawUpTime(mcastRouteStarGExpiryTimeValue, (L7_timespec *)&timeSpec);
  osapiSnprintf(objipMcastRouteStarGExpiryTimeValue, sizeof(objipMcastRouteStarGExpiryTimeValue), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);
  /* return the object value: ipMcastRouteStarGExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteStarGExpiryTimeValue,
                           sizeof (objipMcastRouteStarGExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGUpstreamNeighbor
*
* @purpose Get 'ipMcastRouteStarGUpstreamNeighbor'
 *@description  [ipMcastRouteStarGUpstreamNeighbor] The address of the upstream
* neighbor (for example, RPF neighbor) from which IP datagrams from
* these sources to this multicast address are received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGUpstreamNeighbor (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteStarGGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteStarGGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objipMcastRouteStarGUpstreamNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteStarGGroup */
  kwaipMcastRouteStarGGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup,
                                           (xLibU8_t *) &keyipMcastRouteStarGGroupValue,
                                           &kwaipMcastRouteStarGGroup.len);
  if (kwaipMcastRouteStarGGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteStarGGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteStarGGroup);
    return kwaipMcastRouteStarGGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteStarGGroupValue, kwaipMcastRouteStarGGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  inetAddressZeroSet(addressType, &keyipMcastRouteSourceValue);
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteUpstreamNbrGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteStarGGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteStarGUpstreamNeighborValue);
  objipMcastRouteStarGUpstreamNeighborValue.family = addressType;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteStarGUpstreamNeighbor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objipMcastRouteStarGUpstreamNeighborValue,
                           sizeof (objipMcastRouteStarGUpstreamNeighborValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGProtocol
*
* @purpose Get 'ipMcastRouteStarGProtocol'
 *@description  [ipMcastRouteStarGProtocol] The multicast routing protocol via which
* this multicast forwarding entry was learned.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGProtocol (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteStarGGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteStarGGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteStarGProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteStarGGroup */
  kwaipMcastRouteStarGGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup,
                                           (xLibU8_t *) &keyipMcastRouteStarGGroupValue,
                                           &kwaipMcastRouteStarGGroup.len);
  if (kwaipMcastRouteStarGGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteStarGGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteStarGGroup);
    return kwaipMcastRouteStarGGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteStarGGroupValue, kwaipMcastRouteStarGGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  inetAddressZeroSet(addressType, &keyipMcastRouteSourceValue);
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteProtocolGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteStarGGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteStarGProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteStarGProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteStarGProtocolValue,
                           sizeof (objipMcastRouteStarGProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGFlags
*
* @purpose Get 'ipMcastRouteStarGFlags'
 *@description  [ipMcastRouteStarGFlags] Route Flags   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteStarGFlags (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteStarGGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteStarGGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteStarGFlagsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteStarGGroup */
  kwaipMcastRouteStarGGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteStarGGroup,
                                           (xLibU8_t *) &keyipMcastRouteStarGGroupValue,
                                           &kwaipMcastRouteStarGGroup.len);
  if (kwaipMcastRouteStarGGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteStarGGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteStarGGroup);
    return kwaipMcastRouteStarGGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteStarGGroupValue, kwaipMcastRouteStarGGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  inetAddressZeroSet(addressType, &keyipMcastRouteSourceValue);
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);
  
  /* get the value from application */
    owa.l7rc = usmDbMcastIpMRouteFlagsGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteStarGGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteStarGFlagsValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  /* return the object value: ipMcastRouteStarGFlags */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteStarGFlagsValue,
                           sizeof (objipMcastRouteStarGFlagsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtProtocol
*
* @purpose Get 'ipMcastRouteRtProtocol'
 *@description  [ipMcastRouteRtProtocol] The routing mechanism via which the
* route used to find the upstream or parent interface for this
* multicast forwarding entry was learned.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtProtocol (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteRtProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteRtProtoGet (L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteRtProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteRtProtocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteRtProtocolValue,
                           sizeof (objipMcastRouteRtProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteOutIntfEntry
*
* @purpose Get 'ipMcastRouteOutIntfEntry'
 *@description  [ipMcastRouteOutIntfEntry] The outgoing interface for the given
* source address, group address and source mask   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteOutIntfEntry (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteOutIntfEntryValue;
  xLibStr256_t objipMcastRouteOutIntfList;
  L7_INTF_MASK_t      outIntIfMask;
  xLibU32_t oifListStr[L7_CLI_MAX_STRING_LENGTH];
  xLibU32_t numOifs = 0;
  xLibU32_t outIntIfNum, flag = 1;
  xLibS8_t tempBuf[10];
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  objipMcastRouteOutIntfEntryValue = 0;
  if (usmDbPimsmIsOperational(L7_UNIT_CURRENT, addressType) != L7_TRUE)
  {
    memset(objipMcastRouteOutIntfList, 0, sizeof(xLibStr256_t));
    do
    {
      if(L7_SUCCESS == usmDbMcastIpMRouteOutIntfEntryNextGet (addressType, L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
          &keyipMcastRouteSourceValue,
          &keyipMcastRouteSourceMaskValue,
          &objipMcastRouteOutIntfEntryValue))
      {
        memset (tempBuf, 0x0, sizeof (tempBuf));
        if (flag == 1)
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf), "%d", objipMcastRouteOutIntfEntryValue);
          flag = 0;
        }
        else
        {
          tempBuf[0] = ',';
          osapiSnprintf(&tempBuf[1], (sizeof(tempBuf)-1), "%d", objipMcastRouteOutIntfEntryValue);
        }
        osapiStrncat(objipMcastRouteOutIntfList, tempBuf, sizeof(tempBuf));
      }
      else
        break;
    }while(1);
  }
  else
  {
    memset(objipMcastRouteOutIntfList, 0, sizeof(xLibStr256_t));
    memset(&outIntIfMask, 0, sizeof(L7_INTF_MASK_t));
    usmDbPimsmMapOIFGet(addressType, &keyipMcastRouteGroupValue, 
                        &keyipMcastRouteSourceValue, &keyipMcastRouteSourceMaskValue,
                        &outIntIfMask);
    memset(oifListStr, 0, sizeof(oifListStr));
    usmDbConvertMaskToList(&outIntIfMask, oifListStr, &numOifs);
    for (outIntIfNum = 1; outIntIfNum <= numOifs; outIntIfNum++)
    {
      memset (tempBuf, 0x0, sizeof (tempBuf));
      if (flag == 1)
      {
       osapiSnprintf(tempBuf, sizeof(tempBuf), "%d", oifListStr[outIntIfNum]);
       flag = 0;
      }
      else
      {
        tempBuf[0] = ','; 
        osapiSnprintf(&tempBuf[1], (sizeof(tempBuf)-1), "%d", oifListStr[outIntIfNum]);
      }
      osapiStrncat(objipMcastRouteOutIntfList, tempBuf, sizeof(tempBuf));
    }
  }

  /* return the object value: ipMcastRouteOutIntfEntry */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objipMcastRouteOutIntfList,
                           sizeof (objipMcastRouteOutIntfList));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtAddressType
*
* @purpose Get 'ipMcastRouteRtAddressType'
 *@description  [ipMcastRouteRtAddressType] A value indicating the address
* family of the address contained in ipMcastRouteRtAddress.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtAddressType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  L7_inet_addr_t objipMcastRouteRtAddress;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteRtAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteRtAddressGet (L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteRtAddress);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objipMcastRouteRtAddress.family == L7_AF_INET)
  {
    objipMcastRouteRtAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
  }
  else
  {
    objipMcastRouteRtAddressTypeValue = L7_INET_ADDR_TYPE_IPV6;
  }
  /* return the object value: ipMcastRouteRtAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteRtAddressTypeValue,
                           sizeof (objipMcastRouteRtAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtAddress
*
* @purpose Get 'ipMcastRouteRtAddress'
 *@description  [ipMcastRouteRtAddress] The address portion of the route used to
* find the upstream or parent interface for this multicast
* forwarding entry. This address object is only significant up to
* ipMcastRouteRtPrefixLength bits. The remaining address bits are set to
* zero. For addresses of type ipv4z or ipv6z, the appended zone index
* is significant even though it lies beyond the prefix length. The
* use of these address types indicate that this forwarding state
* applies only within the given zone. Zone index zero is not valid
* in this table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtAddress (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objipMcastRouteRtAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteRtAddressGet (L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteRtAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteRtAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objipMcastRouteRtAddressValue,
                           sizeof (objipMcastRouteRtAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtPrefixLength
*
* @purpose Get 'ipMcastRouteRtPrefixLength'
 *@description  [ipMcastRouteRtPrefixLength] The length in bits of the mask
* associated with the route used to find the upstream or parent
* interface for this multicast forwarding entry. The InetAddressType is
* given by ipMcastRouteRtAddressType. For values 'ipv4' and 'ipv4z',
* this object must be in the range 4..32. For values 'ipv6' and
* 'ipv6z', this object must be in the range 8..128.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtPrefixLength (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  L7_inet_addr_t objipMcastRouteRtPrefix;
  xLibU32_t addressType;
  xLibU8_t  tmpVal;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteRtPrefixLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteRtMaskGet (L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteRtPrefix);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  inetMaskToMaskLen(&objipMcastRouteRtPrefix, &tmpVal);
  objipMcastRouteRtPrefixLengthValue = tmpVal;

  /* return the object value: ipMcastRouteRtPrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteRtPrefixLengthValue,
                           sizeof (objipMcastRouteRtPrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtType
*
* @purpose Get 'ipMcastRouteRtType'
 *@description  [ipMcastRouteRtType] The reason the given route was placed in
* the (logical) multicast Routing Information Base (RIB). A value of
* unicast means that the route would normally be placed only in the
* unicast RIB, but was placed in the multicast RIB due (instead or
* in addition) to local configuration, such as when running PIM
* over RIP. A value of multicast means that the route was explicitly
* added to the multicast RIB by the routing protocol, such as the
* Distance Vector Multicast Routing Protocol (DVMRP) or
* Multiprotocol BGP.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteRtType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  L7_inet_addr_t keyipMcastRouteSourceMaskValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteRtTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) &keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) &keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  if(keyipMcastRouteSourceAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  inetMaskLenToMask(addressType, keyipMcastRouteSourcePrefixLengthValue, &keyipMcastRouteSourceMaskValue);

  /* get the value from application */
  owa.l7rc = usmDbMcastIpMRouteRtTypeGet (L7_UNIT_CURRENT, &keyipMcastRouteGroupValue,
                              &keyipMcastRouteSourceValue,
                              &keyipMcastRouteSourceMaskValue,
                              &objipMcastRouteRtTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteRtType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteRtTypeValue,
                           sizeof (objipMcastRouteRtTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteOctets
*
* @purpose Get 'ipMcastRouteOctets'
 *@description  [ipMcastRouteOctets] The number of octets contained in IP
* datagrams that were received from these sources and addressed to this
* multicast group address, and which were forwarded by this router.
* Discontinuities in this monotonically increasing value occur at
* re-initialization of the management system. Discontinuities can
* also occur as a result of routes being removed and replaced, which
* can be detected by observing the value of ipMcastRouteTimeStamp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteOctets (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteOctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastRouteGroupAddressTypeValue,
                              keyipMcastRouteGroupValue,
                              keyipMcastRouteGroupPrefixLengthValue,
                              keyipMcastRouteSourceAddressTypeValue,
                              keyipMcastRouteSourceValue,
                              keyipMcastRouteSourcePrefixLengthValue, &objipMcastRouteOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteOctetsValue,
                           sizeof (objipMcastRouteOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRoutePkts
*
* @purpose Get 'ipMcastRoutePkts'
 *@description  [ipMcastRoutePkts] The number of packets routed using this
* multicast route entry. Discontinuities in this monotonically
* increasing value occur at re-initialization of the management system.
* Discontinuities can also occur as a result of routes being removed
* and replaced, which can be detected by observing the value of
* ipMcastRouteTimeStamp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRoutePkts (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRoutePktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastRouteGroupAddressTypeValue,
                              keyipMcastRouteGroupValue,
                              keyipMcastRouteGroupPrefixLengthValue,
                              keyipMcastRouteSourceAddressTypeValue,
                              keyipMcastRouteSourceValue,
                              keyipMcastRouteSourcePrefixLengthValue, &objipMcastRoutePktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRoutePkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRoutePktsValue,
                           sizeof (objipMcastRoutePktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteTtlDropOctets
*
* @purpose Get 'ipMcastRouteTtlDropOctets'
 *@description  [ipMcastRouteTtlDropOctets] The number of octets contained in IP
* datagrams that this router has received from these sources and
* addressed to this multicast group address, which were dropped
* because the TTL (IPv4) or Hop Limit (IPv6) was decremented to zero,
* or to a value less than ipMcastInterfaceTtl for all next hops.
* Discontinuities in this monotonically increasing value occur at
* re-initialization of the management system. Discontinuities can also
* occur as a result of routes being removed and replaced, which can
* be detected by observing the value of ipMcastRouteTimeStamp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteTtlDropOctets (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteTtlDropOctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastRouteGroupAddressTypeValue,
                              keyipMcastRouteGroupValue,
                              keyipMcastRouteGroupPrefixLengthValue,
                              keyipMcastRouteSourceAddressTypeValue,
                              keyipMcastRouteSourceValue,
                              keyipMcastRouteSourcePrefixLengthValue,
                              &objipMcastRouteTtlDropOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteTtlDropOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteTtlDropOctetsValue,
                           sizeof (objipMcastRouteTtlDropOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteTtlDropPackets
*
* @purpose Get 'ipMcastRouteTtlDropPackets'
 *@description  [ipMcastRouteTtlDropPackets] The number of packets that this
* router has received from these sources and addressed to this
* multicast group address, which were dropped because the TTL (IPv4) or
* Hop Limit (IPv6) was decremented to zero, or to a value less than
* ipMcastInterfaceTtl for all next hops. Discontinuities in this
* monotonically increasing value occur at re-initialization of the
* management system. Discontinuities can also occur as a result of
* routes being removed and replaced, which can be detected by
* observing the value of ipMcastRouteTimeStamp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteTtlDropPackets (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteTtlDropPacketsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastRouteGroupAddressTypeValue,
                              keyipMcastRouteGroupValue,
                              keyipMcastRouteGroupPrefixLengthValue,
                              keyipMcastRouteSourceAddressTypeValue,
                              keyipMcastRouteSourceValue,
                              keyipMcastRouteSourcePrefixLengthValue,
                              &objipMcastRouteTtlDropPacketsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteTtlDropPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteTtlDropPacketsValue,
                           sizeof (objipMcastRouteTtlDropPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteDifferentInIfOctets
*
* @purpose Get 'ipMcastRouteDifferentInIfOctets'
 *@description  [ipMcastRouteDifferentInIfOctets] The number of octets contained
* in IP datagrams that this router has received from these sources
* and addressed to this multicast group address, which were
* dropped because they were received on an unexpected interface. For RPF
* checking protocols (such as PIM-SM), these packets arrived on
* interfaces other than ipMcastRouteInIfIndex, and were dropped
* because of this failed RPF check. (RPF paths are 'Reverse Path
* Forwarding' paths; the unicast routes to the expected origin of multicast
* data flows). Other protocols may drop packets on an incoming
* interface check for different reasons (for example, BIDIR-PIM
* performs a DF check on receipt of packets). All packets dropped as a
* result of an incoming interface check are counted here. If this
* counter increases rapidly, this indicates a problem. A significant
* quantity of multicast data is arriving at this router on
* unexpected interfaces, and is not being forwarded. For guidance, if the
* rate of increase of this counter exceeds 1% of the rate of increase
* of ipMcastRouteOctets, then there are multicast routing problems
* that require investigation. Discontinuities in this
* monotonically increasing value occur at re-initialization of the management
* system. Discontinuities can also occur as a result of routes being
* removed and replaced, which can be detected by observing the
* value of ipMcastRouteTimeStamp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteDifferentInIfOctets (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteDifferentInIfOctetsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastRouteGroupAddressTypeValue,
                              keyipMcastRouteGroupValue,
                              keyipMcastRouteGroupPrefixLengthValue,
                              keyipMcastRouteSourceAddressTypeValue,
                              keyipMcastRouteSourceValue,
                              keyipMcastRouteSourcePrefixLengthValue,
                              &objipMcastRouteDifferentInIfOctetsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteDifferentInIfOctets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteDifferentInIfOctetsValue,
                           sizeof (objipMcastRouteDifferentInIfOctetsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteDifferentInIfPackets
*
* @purpose Get 'ipMcastRouteDifferentInIfPackets'
 *@description  [ipMcastRouteDifferentInIfPackets] The number of packets which
* this router has received from these sources and addressed to this
* multicast group address, which were dropped because they were
* received on an unexpected interface. For RPF checking protocols
* (such as PIM-SM), these packets arrived on interfaces other than
* ipMcastRouteInIfIndex, and were dropped because of this failed RPF
* check. (RPF paths are 'Reverse Path Forwarding' path; the unicast
* routes to the expected origin of multicast data flows). Other
* protocols may drop packets on an incoming interface check for
* different reasons (for example, BIDIR-PIM performs a DF check on receipt
* of packets). All packets dropped as a result of an incoming
* interface check are counted here. If this counter increases rapidly,
* this indicates a problem. A significant quantity of multicast
* data is arriving at this router on unexpected interfaces, and is not
* being forwarded. For guidance, if the rate of increase of this
* counter exceeds 1% of the rate of increase of ipMcastRoutePkts,
* then there are multicast routing problems that require
* investigation. Discontinuities in this monotonically increasing value occur
* at re-initialization of the management system. Discontinuities can
* also occur as a result of routes being removed and replaced,
* which can be detected by observing the value of
* ipMcastRouteTimeStamp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteDifferentInIfPackets (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteDifferentInIfPacketsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastRouteGroupAddressTypeValue,
                              keyipMcastRouteGroupValue,
                              keyipMcastRouteGroupPrefixLengthValue,
                              keyipMcastRouteSourceAddressTypeValue,
                              keyipMcastRouteSourceValue,
                              keyipMcastRouteSourcePrefixLengthValue,
                              &objipMcastRouteDifferentInIfPacketsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteDifferentInIfPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteDifferentInIfPacketsValue,
                           sizeof (objipMcastRouteDifferentInIfPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteBps
*
* @purpose Get 'ipMcastRouteBps'
 *@description  [ipMcastRouteBps] Bits per second forwarded by this router using
* this multicast routing entry. This value is a sample; it is the
* number of bits forwarded during the last whole 1 second sampling
* period. The value during the current 1 second sampling period is
* not made available until the period is completed. The quantity
* being sampled is the same as that measured by ipMcastRouteOctets.
* The units and the sampling method are different.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastRouteTable_ipMcastRouteBps (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastRouteGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupAddressTypeValue;
  fpObjWa_t kwaipMcastRouteGroup = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteGroupValue;
  fpObjWa_t kwaipMcastRouteGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteGroupPrefixLengthValue;
  fpObjWa_t kwaipMcastRouteSourceAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourceAddressTypeValue;
  fpObjWa_t kwaipMcastRouteSource = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastRouteSourceValue;
  xLibU32_t addressType;
  fpObjWa_t kwaipMcastRouteSourcePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastRouteSourcePrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastRouteBpsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastRouteGroupAddressType */
  kwaipMcastRouteGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupAddressType,
                   (xLibU8_t *) & keyipMcastRouteGroupAddressTypeValue,
                   &kwaipMcastRouteGroupAddressType.len);
  if (kwaipMcastRouteGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupAddressType);
    return kwaipMcastRouteGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupAddressTypeValue,
                           kwaipMcastRouteGroupAddressType.len);

  /* retrieve key: ipMcastRouteGroup */
  kwaipMcastRouteGroup.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroup,
                                           (xLibU8_t *) keyipMcastRouteGroupValue,
                                           &kwaipMcastRouteGroup.len);
  if (kwaipMcastRouteGroup.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroup.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroup);
    return kwaipMcastRouteGroup.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteGroupValue, kwaipMcastRouteGroup.len);

  /* retrieve key: ipMcastRouteGroupPrefixLength */
  kwaipMcastRouteGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteGroupPrefixLength,
                   (xLibU8_t *) & keyipMcastRouteGroupPrefixLengthValue,
                   &kwaipMcastRouteGroupPrefixLength.len);
  if (kwaipMcastRouteGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteGroupPrefixLength);
    return kwaipMcastRouteGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteGroupPrefixLengthValue,
                           kwaipMcastRouteGroupPrefixLength.len);

  /* retrieve key: ipMcastRouteSourceAddressType */
  kwaipMcastRouteSourceAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourceAddressType,
                   (xLibU8_t *) & keyipMcastRouteSourceAddressTypeValue,
                   &kwaipMcastRouteSourceAddressType.len);
  if (kwaipMcastRouteSourceAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourceAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourceAddressType);
    return kwaipMcastRouteSourceAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourceAddressTypeValue,
                           kwaipMcastRouteSourceAddressType.len);

  /* retrieve key: ipMcastRouteSource */
  kwaipMcastRouteSource.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSource,
                                            (xLibU8_t *) keyipMcastRouteSourceValue,
                                            &kwaipMcastRouteSource.len);
  if (kwaipMcastRouteSource.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSource.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSource);
    return kwaipMcastRouteSource.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastRouteSourceValue, kwaipMcastRouteSource.len);

  /* retrieve key: ipMcastRouteSourcePrefixLength */
  kwaipMcastRouteSourcePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastRouteTable_ipMcastRouteSourcePrefixLength,
                   (xLibU8_t *) & keyipMcastRouteSourcePrefixLengthValue,
                   &kwaipMcastRouteSourcePrefixLength.len);
  if (kwaipMcastRouteSourcePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastRouteSourcePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastRouteSourcePrefixLength);
    return kwaipMcastRouteSourcePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastRouteSourcePrefixLengthValue,
                           kwaipMcastRouteSourcePrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastRouteGroupAddressTypeValue,
                              keyipMcastRouteGroupValue,
                              keyipMcastRouteGroupPrefixLengthValue,
                              keyipMcastRouteSourceAddressTypeValue,
                              keyipMcastRouteSourceValue,
                              keyipMcastRouteSourcePrefixLengthValue, &objipMcastRouteBpsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastRouteBps */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastRouteBpsValue,
                           sizeof (objipMcastRouteBpsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}
