
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastipMcastSsmRangeTable.c
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
#include "_xe_ipmcastipMcastSsmRangeTable_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_pimsm_api.h"
#include "l3_mcast_defaultconfig.h"

static L7_BOOL xuiPimsmSsmDefaultRangeConfigured(L7_uint32 addressType,
                                                 L7_inet_addr_t *ssmRangeAddr,
                                                 L7_uchar8 prefixLen)
{
  L7_uchar8  strDefSSMGrpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8  defSSMGrpMaskLen = 8;
  L7_inet_addr_t inetDefSSMGrpAddr;


  if (addressType == L7_AF_INET)
  {
    osapiStrncpy(strDefSSMGrpAddr, FD_PIMSM_SSM_RANGE_GROUP_ADDRESS, 
                 sizeof(strDefSSMGrpAddr));
    usmDbParseInetAddrFromStr(strDefSSMGrpAddr, &inetDefSSMGrpAddr);  
    defSSMGrpMaskLen = 8;

    if (usmDbPimsmSsmRangeEntryGet(L7_AF_INET, &inetDefSSMGrpAddr, defSSMGrpMaskLen)
        == L7_SUCCESS)
    {
      if (inetAddrCompareAddrWithMask(&inetDefSSMGrpAddr, defSSMGrpMaskLen,
                                      ssmRangeAddr, defSSMGrpMaskLen) == L7_NULL)
      {
        return L7_TRUE;
      }
    }
  }
  else if (addressType == L7_AF_INET6)
  {
    osapiStrncpy(strDefSSMGrpAddr, FD_PIMSM6_SSM_RANGE_GROUP_ADDRESS, 
                 sizeof(strDefSSMGrpAddr));
    usmDbParseInetAddrFromStr(strDefSSMGrpAddr, &inetDefSSMGrpAddr);  
    defSSMGrpMaskLen = FD_PIMSM6_SSM_RANGE_PREFIX_LEN;
    if (usmDbPimsmSsmRangeEntryGet(L7_AF_INET6, &inetDefSSMGrpAddr, defSSMGrpMaskLen)
        == L7_SUCCESS)
    {
      defSSMGrpMaskLen = 12;
      if (inetAddrCompareAddrWithMask(&inetDefSSMGrpAddr, defSSMGrpMaskLen,
                                      ssmRangeAddr, defSSMGrpMaskLen) == L7_NULL)
      {
        return L7_TRUE;
      }
    }

  }
  return L7_FALSE;
}


/*******************************************************************************
* @function fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType
*
* @purpose Get 'ipMcastSsmRangeAddressType'
 *@description  [ipMcastSsmRangeAddressType] The address type of the multicast
* group prefix.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType (void *wap, void *bufp)
{

  xLibU32_t objipMcastSsmRangeAddressTypeValue;
  xLibU32_t nextObjipMcastSsmRangeAddressTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastSsmRangeAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType,
                          (xLibU8_t *) & objipMcastSsmRangeAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjipMcastSsmRangeAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastSsmRangeAddressTypeValue, owa.len);
    if(objipMcastSsmRangeAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjipMcastSsmRangeAddressTypeValue = objipMcastSsmRangeAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastSsmRangeAddressTypeValue, owa.len);

  /* return the object value: ipMcastSsmRangeAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastSsmRangeAddressTypeValue,
                           sizeof (objipMcastSsmRangeAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddress
*
* @purpose Get 'ipMcastSsmRangeAddress'
 *@description  [ipMcastSsmRangeAddress] The multicast group address which, when
* combined with ipMcastSsmRangePrefixLength, gives the group
* prefix for this SSM range. The InetAddressType is given by
* ipMcastSsmRangeAddressType. This address object is only significant up to
* ipMcastSsmRangePrefixLength bits. The remaining address bits are
* set to zero. This is especially important for this index field,
* which is part of the index of this entry. Any non-zero bits would
* signify an entirely different entry. For IPv6 SSM address ranges,
* only ranges prefixed by FF3x::/16 are permitted, where 'x' is a
* valid IPv6 RFC 4291 multicast address scope. The syntax of the
* address range is given by RFC 3306, Sections 4 and 7. For addresses
* of type ipv4z or ipv6z, the appended zone index is significant
* even though it lies beyond the prefix length. The use of these
* address types indicate that this SSM range entry applies only within
* the given zone. Zone index zero is not valid in this table. If
* non-global scope SSM range entries are present, then consistent
* ipMcastBoundaryTable entries are required on routers at the zone
* boundary.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddress (void *wap, void *bufp)
{

  xLibU32_t objipMcastSsmRangeAddressTypeValue;
  xLibU32_t nextObjipMcastSsmRangeAddressTypeValue;
  L7_inet_addr_t objipMcastSsmRangeAddressValue;
  L7_inet_addr_t nextObjipMcastSsmRangeAddressValue;
  static xLibU8_t nextObjipMcastSsmRangePrefixLengthValue;
  xLibU32_t addressType;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastSsmRangeAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType,
                          (xLibU8_t *) & objipMcastSsmRangeAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastSsmRangeAddressTypeValue, owa.len);

  /* retrieve key: ipMcastSsmRangeAddress */
  owa.len = sizeof (L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddress,
                          (xLibU8_t *) &objipMcastSsmRangeAddressValue, &owa.len);
  nextObjipMcastSsmRangeAddressTypeValue = objipMcastSsmRangeAddressTypeValue;
  if(nextObjipMcastSsmRangeAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
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
    inetAddressZeroSet(addressType, &nextObjipMcastSsmRangeAddressValue);
    nextObjipMcastSsmRangePrefixLengthValue = 0;
    owa.l7rc = usmDbPimsmSsmRangeEntryNextGet (addressType,
                                     &nextObjipMcastSsmRangeAddressValue,
                                     (xLibU8_t *)&nextObjipMcastSsmRangePrefixLengthValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastSsmRangeAddressValue, owa.len);
    /*inetAddressZeroSet(addressType, &nextObjipMcastSsmRangeAddressValue);
    nextObjipMcastSsmRangePrefixLengthValue = 0;*/
    memcpy(&nextObjipMcastSsmRangeAddressValue,&objipMcastSsmRangeAddressValue,sizeof(L7_inet_addr_t));
    do
    {
      owa.l7rc = usmDbPimsmSsmRangeEntryNextGet (addressType,
                                      &nextObjipMcastSsmRangeAddressValue,
                                      (xLibU8_t *)&nextObjipMcastSsmRangePrefixLengthValue);
    }
    while (/*(objipMcastSsmRangeAddressTypeValue == nextObjipMcastSsmRangeAddressTypeValue)
           &&*/ (!memcmp(&objipMcastSsmRangeAddressValue, &nextObjipMcastSsmRangeAddressValue, sizeof(objipMcastSsmRangeAddressValue)))
           && (owa.l7rc == L7_SUCCESS));

  }

  if (/*(objipMcastSsmRangeAddressTypeValue != nextObjipMcastSsmRangeAddressTypeValue)
      || */(owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastSsmRangeAddressValue, owa.len);

  /* return the object value: ipMcastSsmRangeAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjipMcastSsmRangeAddressValue,
                           sizeof (objipMcastSsmRangeAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangePrefixLength
*
* @purpose Get 'ipMcastSsmRangePrefixLength'
 *@description  [ipMcastSsmRangePrefixLength] The length in bits of the mask
* which, when combined with ipMcastSsmRangeAddress, gives the group
* prefix for this SSM range. The InetAddressType is given by
* ipMcastSsmRangeAddressType. For values 'ipv4' and 'ipv4z', this object
* must be in the range 4..32. For values 'ipv6' and 'ipv6z', this
* object must be in the range 8..128.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangePrefixLength (void *wap, void *bufp)
{

  xLibU32_t objipMcastSsmRangeAddressTypeValue;
  xLibU32_t nextObjipMcastSsmRangeAddressTypeValue;
  L7_inet_addr_t objipMcastSsmRangeAddressValue;
  L7_inet_addr_t nextObjipMcastSsmRangeAddressValue;
  xLibV4V6Mask_t objipMcastSsmRangePrefixLengthValue;
  xLibV4V6Mask_t nextObjipMcastSsmRangePrefixLengthValue;
  xLibU32_t addressType;
  L7_uchar8 maskLen=0;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastSsmRangeAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType,
                          (xLibU8_t *) & objipMcastSsmRangeAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memset(&nextObjipMcastSsmRangePrefixLengthValue,0,sizeof(xLibV4V6Mask_t));
  memset(&objipMcastSsmRangePrefixLengthValue,0,sizeof(xLibV4V6Mask_t));

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastSsmRangeAddressTypeValue, owa.len);

  /* retrieve key: ipMcastSsmRangeAddress */
  owa.len = sizeof (L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddress,
                          (xLibU8_t *) &objipMcastSsmRangeAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastSsmRangeAddressValue, owa.len);

  /* retrieve key: ipMcastSsmRangePrefixLength */
  
  owa.len = sizeof(xLibV4V6Mask_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangePrefixLength,
                          (xLibU8_t *) & objipMcastSsmRangePrefixLengthValue, &owa.len);
  nextObjipMcastSsmRangeAddressTypeValue = objipMcastSsmRangeAddressTypeValue;
  if(nextObjipMcastSsmRangeAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
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
    inetAddressZeroSet(addressType, &nextObjipMcastSsmRangeAddressValue);
    memset (&nextObjipMcastSsmRangePrefixLengthValue, 0, sizeof (objipMcastSsmRangePrefixLengthValue));
    maskLen=0;
    do 
    {
      owa.l7rc = usmDbPimsmSsmRangeEntryNextGet (addressType,
                                       &nextObjipMcastSsmRangeAddressValue,
                                       (xLibU8_t *)&maskLen);
      nextObjipMcastSsmRangePrefixLengthValue.addr =maskLen;
    }while ((objipMcastSsmRangeAddressTypeValue == nextObjipMcastSsmRangeAddressTypeValue)
           && (memcmp(&objipMcastSsmRangeAddressValue, &nextObjipMcastSsmRangeAddressValue, sizeof(objipMcastSsmRangeAddressValue))) && (owa.l7rc == L7_SUCCESS));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objipMcastSsmRangePrefixLengthValue, owa.len);
    memcpy(&nextObjipMcastSsmRangeAddressValue, &objipMcastSsmRangeAddressValue, sizeof(objipMcastSsmRangeAddressValue));
/*    nextObjipMcastSsmRangePrefixLengthValue.addr = objipMcastSsmRangePrefixLengthValue.addr;*/
    maskLen = objipMcastSsmRangePrefixLengthValue.addr;
    owa.l7rc = usmDbPimsmSsmRangeEntryNextGet (addressType,
                                    &nextObjipMcastSsmRangeAddressValue,
                                    (xLibU8_t *)&maskLen);
     nextObjipMcastSsmRangePrefixLengthValue.addr =maskLen;

  }

  if ((objipMcastSsmRangeAddressTypeValue != nextObjipMcastSsmRangeAddressTypeValue)
      || (memcmp(&objipMcastSsmRangeAddressValue, &nextObjipMcastSsmRangeAddressValue, sizeof(objipMcastSsmRangeAddressValue)) != 0)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjipMcastSsmRangePrefixLengthValue, owa.len);

  nextObjipMcastSsmRangePrefixLengthValue.family = addressType; 
  /* return the object value: ipMcastSsmRangePrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjipMcastSsmRangePrefixLengthValue,
                           sizeof (objipMcastSsmRangePrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeRowStatus
*
* @purpose Get 'ipMcastSsmRangeRowStatus'
 *@description  [ipMcastSsmRangeRowStatus] The status of this row, by which rows
* in this table can be created and destroyed. This status object
* can be set to active(1) without setting any other columnar objects
* in this entry. All writeable objects in this entry can be
* modified when the status of this entry is active(1).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeRowStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastSsmRangeAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastSsmRangeAddressTypeValue;
  fpObjWa_t kwaipMcastSsmRangeAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastSsmRangeAddressValue;
  fpObjWa_t kwaipMcastSsmRangePrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keyipMcastSsmRangePrefixLengthValue;
  xLibU32_t addressType;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastSsmRangeRowStatusValue;

  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: ipMcastSsmRangeAddressType */
  kwaipMcastSsmRangeAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType,
                   (xLibU8_t *) & keyipMcastSsmRangeAddressTypeValue,
                   &kwaipMcastSsmRangeAddressType.len);
  if (kwaipMcastSsmRangeAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangeAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangeAddressType);
    return kwaipMcastSsmRangeAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangeAddressTypeValue,
                           kwaipMcastSsmRangeAddressType.len);

  /* retrieve key: ipMcastSsmRangeAddress */
  kwaipMcastSsmRangeAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddress,
                   (xLibU8_t *) &keyipMcastSsmRangeAddressValue, &kwaipMcastSsmRangeAddress.len);
  if (kwaipMcastSsmRangeAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangeAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangeAddress);
    return kwaipMcastSsmRangeAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangeAddressValue, kwaipMcastSsmRangeAddress.len);

  /* retrieve key: ipMcastSsmRangePrefixLength */
  kwaipMcastSsmRangePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangePrefixLength,
                   (xLibU8_t *) & keyipMcastSsmRangePrefixLengthValue,
                   &kwaipMcastSsmRangePrefixLength.len);
  if (kwaipMcastSsmRangePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangePrefixLength);
    return kwaipMcastSsmRangePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangePrefixLengthValue,
                           kwaipMcastSsmRangePrefixLength.len);

  if(keyipMcastSsmRangeAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }
  /* get the value from application */
  owa.l7rc = usmDbPimsmSsmRangeEntryGet (addressType,
                              &keyipMcastSsmRangeAddressValue,
                              keyipMcastSsmRangePrefixLengthValue.addr);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  objipMcastSsmRangeRowStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: ipMcastSsmRangeRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastSsmRangeRowStatusValue,
                           sizeof (objipMcastSsmRangeRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeRowStatus
*
* @purpose Set 'ipMcastSsmRangeRowStatus'
 *@description  [ipMcastSsmRangeRowStatus] The status of this row, by which rows
* in this table can be created and destroyed. This status object
* can be set to active(1) without setting any other columnar objects
* in this entry. All writeable objects in this entry can be
* modified when the status of this entry is active(1).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastSsmRangeRowStatusValue;

  fpObjWa_t kwaipMcastSsmRangeAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastSsmRangeAddressTypeValue;
  fpObjWa_t kwaipMcastSsmRangeAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastSsmRangeAddressValue;
  fpObjWa_t kwaipMcastSsmRangePrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keyipMcastSsmRangePrefixLengthValue;
  xLibU32_t addressType;
  L7_inet_addr_t grpMask;

  xLibU32_t      v4mask = 0x0;

  L7_uint32 unit=0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastSsmRangeRowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastSsmRangeRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastSsmRangeRowStatusValue, owa.len);

  /* retrieve key: ipMcastSsmRangeAddressType */
  kwaipMcastSsmRangeAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType,
                   (xLibU8_t *) & keyipMcastSsmRangeAddressTypeValue,
                   &kwaipMcastSsmRangeAddressType.len);
  if (kwaipMcastSsmRangeAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangeAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangeAddressType);
    return kwaipMcastSsmRangeAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangeAddressTypeValue,
                           kwaipMcastSsmRangeAddressType.len);

  /* retrieve key: ipMcastSsmRangeAddress */
  kwaipMcastSsmRangeAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddress,
                   (xLibU8_t *) &keyipMcastSsmRangeAddressValue, &kwaipMcastSsmRangeAddress.len);
  if (kwaipMcastSsmRangeAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangeAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangeAddress);
    return kwaipMcastSsmRangeAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangeAddressValue, kwaipMcastSsmRangeAddress.len);

  /* retrieve key: ipMcastSsmRangePrefixLength */
  kwaipMcastSsmRangePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangePrefixLength,
                   (xLibU8_t *) & keyipMcastSsmRangePrefixLengthValue,
                   &kwaipMcastSsmRangePrefixLength.len);
  if (kwaipMcastSsmRangePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangePrefixLength);
    return kwaipMcastSsmRangePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangePrefixLengthValue,
                           kwaipMcastSsmRangePrefixLength.len);

  if(keyipMcastSsmRangeAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
  }
  else
  {
    addressType = L7_AF_INET6;
  }

  inetMaskLenToMask(addressType, keyipMcastSsmRangePrefixLengthValue.addr, &grpMask);

  if (owa.l7rc != L7_SUCCESS)
  {
    if (addressType == L7_AF_INET)
    {
      owa.rc = XLIBRC_INVALID_SUBNET_MASK;    /* TODO: Change if required */
    }
    else if ((addressType == L7_AF_INET6) &&
             (keyipMcastSsmRangePrefixLengthValue.addr < 1  || keyipMcastSsmRangePrefixLengthValue.addr > 128))
    {
      owa.rc = XLIBRC_INVALID_PREFIX_LENGTH;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (addressType == L7_AF_INET)
  {
    inetAddressGet(addressType, &grpMask, &v4mask);
    if (usmDbNetmaskValidate(v4mask) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_INVALID_SUBNET_MASK;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }


  owa.l7rc = L7_SUCCESS;
  if (objipMcastSsmRangeRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    if (xuiPimsmSsmDefaultRangeConfigured(addressType,
                                &keyipMcastSsmRangeAddressValue,
                                keyipMcastSsmRangePrefixLengthValue.addr) == L7_TRUE)
    {
      owa.rc = XLIBRC_MCAST_PIM_DEFAULT_SSM_RANGE_ALREADY_CONFIGURED;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    unit=0;
    inetAddressZeroSet(addressType,&grpMask);
    inetMaskLenToMask(addressType,keyipMcastSsmRangePrefixLengthValue.addr,&grpMask);

    owa.l7rc = usmDbPimsmSsmRangeSet (unit, addressType, L7_ENABLE,
                                &keyipMcastSsmRangeAddressValue, &grpMask);
  }
  else if (objipMcastSsmRangeRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    unit=0;
    inetAddressZeroSet(addressType,&grpMask);
    inetMaskLenToMask(addressType,keyipMcastSsmRangePrefixLengthValue.addr,&grpMask);
    owa.l7rc = usmDbPimsmSsmRangeSet (unit,addressType,L7_DISABLE,
                                   &keyipMcastSsmRangeAddressValue,
                                   &grpMask);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    if (owa.l7rc == L7_ALREADY_CONFIGURED)
    {
      owa.rc = XLIBRC_MCAST_PIM_SSM_RANGE_ALREADY_CONFIGURED;
    }
    else if (addressType == L7_AF_INET6 && owa.l7rc == L7_NOT_SUPPORTED)
    {
      owa.rc = XLIBRC_MCAST_PIM_SSM6_SCOPEID_NOT_SUPPORTED;
    }
    else
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeStorageType
*
* @purpose Get 'ipMcastSsmRangeStorageType'
 *@description  [ipMcastSsmRangeStorageType] The storage type for this row. Rows
* having the value 'permanent' need not allow write-access to any
* columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwaipMcastSsmRangeAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastSsmRangeAddressTypeValue;
  fpObjWa_t kwaipMcastSsmRangeAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastSsmRangeAddressValue;
  fpObjWa_t kwaipMcastSsmRangePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastSsmRangePrefixLengthValue;
  xLibU32_t addressType;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastSsmRangeStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastSsmRangeAddressType */
  kwaipMcastSsmRangeAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType,
                   (xLibU8_t *) & keyipMcastSsmRangeAddressTypeValue,
                   &kwaipMcastSsmRangeAddressType.len);
  if (kwaipMcastSsmRangeAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangeAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangeAddressType);
    return kwaipMcastSsmRangeAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangeAddressTypeValue,
                           kwaipMcastSsmRangeAddressType.len);

  /* retrieve key: ipMcastSsmRangeAddress */
  kwaipMcastSsmRangeAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddress,
                   (xLibU8_t *) keyipMcastSsmRangeAddressValue, &kwaipMcastSsmRangeAddress.len);
  if (kwaipMcastSsmRangeAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangeAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangeAddress);
    return kwaipMcastSsmRangeAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastSsmRangeAddressValue, kwaipMcastSsmRangeAddress.len);

  /* retrieve key: ipMcastSsmRangePrefixLength */
  kwaipMcastSsmRangePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangePrefixLength,
                   (xLibU8_t *) & keyipMcastSsmRangePrefixLengthValue,
                   &kwaipMcastSsmRangePrefixLength.len);
  if (kwaipMcastSsmRangePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangePrefixLength);
    return kwaipMcastSsmRangePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangePrefixLengthValue,
                           kwaipMcastSsmRangePrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyipMcastSsmRangeAddressTypeValue,
                              keyipMcastSsmRangeAddressValue,
                              keyipMcastSsmRangePrefixLengthValue,
                              &objipMcastSsmRangeStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ipMcastSsmRangeStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objipMcastSsmRangeStorageTypeValue,
                           sizeof (objipMcastSsmRangeStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif
  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeStorageType
*
* @purpose Set 'ipMcastSsmRangeStorageType'
 *@description  [ipMcastSsmRangeStorageType] The storage type for this row. Rows
* having the value 'permanent' need not allow write-access to any
* columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objipMcastSsmRangeStorageTypeValue;

  fpObjWa_t kwaipMcastSsmRangeAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastSsmRangeAddressTypeValue;
  fpObjWa_t kwaipMcastSsmRangeAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keyipMcastSsmRangeAddressValue;
  fpObjWa_t kwaipMcastSsmRangePrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastSsmRangePrefixLengthValue;
  xLibU32_t addressType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ipMcastSsmRangeStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objipMcastSsmRangeStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objipMcastSsmRangeStorageTypeValue, owa.len);

  /* retrieve key: ipMcastSsmRangeAddressType */
  kwaipMcastSsmRangeAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType,
                   (xLibU8_t *) & keyipMcastSsmRangeAddressTypeValue,
                   &kwaipMcastSsmRangeAddressType.len);
  if (kwaipMcastSsmRangeAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangeAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangeAddressType);
    return kwaipMcastSsmRangeAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangeAddressTypeValue,
                           kwaipMcastSsmRangeAddressType.len);

  /* retrieve key: ipMcastSsmRangeAddress */
  kwaipMcastSsmRangeAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddress,
                   (xLibU8_t *) keyipMcastSsmRangeAddressValue, &kwaipMcastSsmRangeAddress.len);
  if (kwaipMcastSsmRangeAddress.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangeAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangeAddress);
    return kwaipMcastSsmRangeAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyipMcastSsmRangeAddressValue, kwaipMcastSsmRangeAddress.len);

  /* retrieve key: ipMcastSsmRangePrefixLength */
  kwaipMcastSsmRangePrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangePrefixLength,
                   (xLibU8_t *) & keyipMcastSsmRangePrefixLengthValue,
                   &kwaipMcastSsmRangePrefixLength.len);
  if (kwaipMcastSsmRangePrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangePrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangePrefixLength);
    return kwaipMcastSsmRangePrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangePrefixLengthValue,
                           kwaipMcastSsmRangePrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyipMcastSsmRangeAddressTypeValue,
                              keyipMcastSsmRangeAddressValue,
                              keyipMcastSsmRangePrefixLengthValue,
                              objipMcastSsmRangeStorageTypeValue);

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
* @function fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeDefaultIsExists
*
* @purpose Get 'ipMcastSsmRangeDefaultIsExists'
* @description  [ipMcastSsmRangeDefaultIsExists]
*
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeDefaultIsExists (void *wap, void *bufp)
{

  fpObjWa_t kwaipMcastSsmRangeAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyipMcastSsmRangeAddressTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (TrueFalse_t));
  L7_inet_addr_t keyipMcastSsmRangeAddressValue;
  L7_uchar8 keyipMcastSsmRangePrefixLengthValue;
  xLibU32_t addressType;
  L7_uchar8 strDefSSMGrpAddr[256];
  TrueFalse_t isExists = L7_FALSE;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ipMcastSsmRangeAddressType */
  kwaipMcastSsmRangeAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastipMcastSsmRangeTable_ipMcastSsmRangeAddressType,
                   (xLibU8_t *) & keyipMcastSsmRangeAddressTypeValue,
                   &kwaipMcastSsmRangeAddressType.len);
  if (kwaipMcastSsmRangeAddressType.rc != XLIBRC_SUCCESS)
  {
    kwaipMcastSsmRangeAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaipMcastSsmRangeAddressType);
    return kwaipMcastSsmRangeAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyipMcastSsmRangeAddressTypeValue,
                           kwaipMcastSsmRangeAddressType.len);


  if(keyipMcastSsmRangeAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    addressType = L7_AF_INET;
    osapiStrncpy(strDefSSMGrpAddr, FD_PIMSM_SSM_RANGE_GROUP_ADDRESS, 
                   sizeof(strDefSSMGrpAddr));
    usmDbParseInetAddrFromStr(strDefSSMGrpAddr, &keyipMcastSsmRangeAddressValue);  
    keyipMcastSsmRangePrefixLengthValue = 8; /* masklen format of FD_PIMSM_SSM_RANGE_GROUP_MASK.*/
  }
  else
  {
    addressType = L7_AF_INET6;
    osapiStrncpy(strDefSSMGrpAddr, FD_PIMSM6_SSM_RANGE_GROUP_ADDRESS, 
                   sizeof(strDefSSMGrpAddr));
    usmDbParseInetAddrFromStr(strDefSSMGrpAddr, &keyipMcastSsmRangeAddressValue);  
    keyipMcastSsmRangePrefixLengthValue = FD_PIMSM6_SSM_RANGE_PREFIX_LEN;
  }

  /* get the value from application */
  owa.l7rc = usmDbPimsmSsmRangeEntryGet (addressType,
                              &keyipMcastSsmRangeAddressValue,
                              keyipMcastSsmRangePrefixLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    isExists = L7_XUI_FALSE;
  }
  else
  {
    isExists = L7_XUI_TRUE; 
  }

  owa.rc = L7_SUCCESS;
  /* return the object value: ipMcastSsmRangeRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & isExists,
                           sizeof (isExists));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
