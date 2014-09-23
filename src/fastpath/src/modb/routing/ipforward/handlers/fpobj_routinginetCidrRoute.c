/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routinginetCidrRoute.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ipforward-object.xml
*
* @create  4 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routinginetCidrRoute_obj.h"
#include "usmdb_mib_ipfwd_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_destinationType
*
* @purpose Get 'destinationType'
 *@description  [destinationType] The type of the inetCidrRouteDest address, as
* defined in the InetAddress MIB. Only those address types that may
* appear in an actual routing table are allowed as values of this
* object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_destinationType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdestinationTypeValue;
  xLibU32_t nextObjdestinationTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: destinationType */
  owa.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                          (xLibU8_t *) & objdestinationTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjdestinationTypeValue = 1;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdestinationTypeValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdestinationTypeValue, owa.len);

  /* return the object value: destinationType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdestinationTypeValue,
                           sizeof (objdestinationTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_destinationIpAddress
*
* @purpose Get 'destinationIpAddress'
 *@description  [destinationIpAddress] The destination IP address of this
* route.The type of this address is determined by the value of the
* inetCidrRouteDestType object. The values for the index objects
* inetCidrRouteDest and inetCidrRoutePfxLen must be consistent. When the
* value of inetCidrRouteDest (excluding the zone index, if one is
* present) is x, then the bitwise logical-AND of x with the value of
* the mask formed from the corresponding index object
* inetCidrRoutePfxLen MUST be equal to x. If not, then the index pair is not
* consistent and an inconsistentName error must be returned on SET or
* CREATE requests.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_destinationIpAddress (void *wap, void *bufp)
{

  xLibU32_t objdestinationIpAddressValue;
  xLibU32_t nextObjdestinationIpAddressValue;
  xLibU32_t objprefixLenValue;
  xLibStr256_t objnextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTosNext = 0;
  L7_uint32 IPaddr;

  /* retrieve key: destinationIpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                          (xLibU8_t *) & objdestinationIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objdestinationIpAddressValue, 0, sizeof (objdestinationIpAddressValue));
    memset (&objprefixLenValue, 0, sizeof (objprefixLenValue));
    memset (objnextHopValue, 0, sizeof (objnextHopValue));
    netMask = netMask << (32- objprefixLenValue);
  
    if (usmDbInetAton(objnextHopValue, &IPaddr) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
    owa.l7rc = usmDbIpCidrRouteEntryGetNext(L7_UNIT_CURRENT, &objdestinationIpAddressValue, &netMask, &routeTosNext,
                                 &IPaddr);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdestinationIpAddressValue, owa.len);
    nextObjdestinationIpAddressValue = objdestinationIpAddressValue;
    memset (&objprefixLenValue, 0, sizeof (objprefixLenValue));
    memset (objnextHopValue, 0, sizeof (objnextHopValue));
    netMask = netMask << (32- objprefixLenValue);
    if (usmDbInetAton(objnextHopValue, &IPaddr) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }

    do
    {
      owa.l7rc = usmDbIpCidrRouteEntryGetNext(L7_UNIT_CURRENT, &objdestinationIpAddressValue, &netMask, &routeTosNext,
                                   &IPaddr);
    }
    while ((objdestinationIpAddressValue == nextObjdestinationIpAddressValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjdestinationIpAddressValue = objdestinationIpAddressValue;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdestinationIpAddressValue, owa.len);

  /* return the object value: destinationIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdestinationIpAddressValue,
                           sizeof (objdestinationIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_prefixLen
*
* @purpose Get 'prefixLen'
 *@description  [prefixLen] Indicates the number of leading one bits which form
* the mask to be logical-ANDed with the destination address before
* being compared to the value in the inetCidrRouteDest field. The
* values for the index objects inetCidrRouteDest and
* inetCidrRoutePfxLen must be consistent.When the value of inetCidrRouteDest
* (excluding the zone index, if one is present) is x, then the bitwise
* logical-AND of x with the value of the mask formed from the
* corresponding index object inetCidrRoutePfxLen MUST be equal to x. If
* not, then the index pair is not consistent and an inconsistentName
* error must be returned on SET or CREATE requests.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_prefixLen (void *wap, void *bufp)
{

  xLibU32_t objdestinationIpAddressValue;
  xLibU32_t nextObjdestinationIpAddressValue;
  xLibU32_t objprefixLenValue;
  xLibU32_t nextObjprefixLenValue;
  xLibStr256_t objnextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_uint32 bitValue = 0X80000000;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTosNext = 0;
  L7_uint32 IPaddr;
  L7_uint32 length=0;

  /* retrieve key: destinationIpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                          (xLibU8_t *) & objdestinationIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdestinationIpAddressValue, owa.len);

  /* retrieve key: prefixLen */
  owa.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                          (xLibU8_t *) & objprefixLenValue, &owa.len);

  nextObjdestinationIpAddressValue = objdestinationIpAddressValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objprefixLenValue, 0, sizeof (objprefixLenValue));
    memset (objnextHopValue, 0, sizeof (objnextHopValue));
    netMask = netMask << (32- objprefixLenValue);
  
    if (usmDbInetAton(objnextHopValue, &IPaddr) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
    
    owa.l7rc = usmDbIpCidrRouteEntryGetNext(L7_UNIT_CURRENT, &objdestinationIpAddressValue, &netMask, &routeTosNext,
                                 &IPaddr);
    while(netMask & bitValue)
    {
      length ++;
      bitValue = bitValue >> 1;
    }

    objprefixLenValue= length;
    
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objprefixLenValue, owa.len);
    nextObjprefixLenValue = objprefixLenValue;
    memset (objnextHopValue, 0, sizeof (objnextHopValue));
    do
    {
      netMask = netMask << (32- objprefixLenValue);
  
      if (usmDbInetAton(objnextHopValue, &IPaddr) != L7_SUCCESS)
      {
        return XLIBRC_FAILURE;
      }
      
      owa.l7rc = usmDbIpCidrRouteEntryGetNext(L7_UNIT_CURRENT, &objdestinationIpAddressValue, &netMask, &routeTosNext,
                                   &IPaddr);
      while(netMask & bitValue)
      {
        length ++;
        bitValue = bitValue >> 1;
      }

      objprefixLenValue= length;
    }
    while ((objdestinationIpAddressValue == nextObjdestinationIpAddressValue)
           && (objprefixLenValue == nextObjprefixLenValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objdestinationIpAddressValue != nextObjdestinationIpAddressValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjprefixLenValue = objprefixLenValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjprefixLenValue, sizeof(nextObjprefixLenValue));

  /* return the object value: prefixLen */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjprefixLenValue, sizeof (objprefixLenValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_policy
*
* @purpose Get 'policy'
 *@description  [policy] This object is an opaque object without any defined
* semantics. Its purpose is to serve as an additional index which may
* delineate between multiple entries to the same destination. The
* value { 0 0 } shall be used as the default value for this object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_policy (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpolicyValue;
  xLibStr256_t nextObjpolicyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: policy */
  owa.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                          (xLibU8_t *) objpolicyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    strcpy(nextObjpolicyValue,"0.0"); 
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpolicyValue, owa.len);
    owa.l7rc = L7_FAILURE;
    
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpolicyValue, strlen (nextObjpolicyValue));

  /* return the object value: policy */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpolicyValue, strlen (nextObjpolicyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_nextHopType
*
* @purpose Get 'nextHopType'
 *@description  [nextHopType] The type of the inetCidrRouteNextHop address, as
* defined in the InetAddress MIB. Value should be set to unknown(0)
* for non-remote routes. Only those address types that may appear
* in an actual routing table are allowed as values of this object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_nextHopType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnextHopTypeValue;
  xLibU32_t nextObjnextHopTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: nextHopType */
  owa.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                          (xLibU8_t *) & objnextHopTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjnextHopTypeValue = 1;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objnextHopTypeValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjnextHopTypeValue, owa.len);

  /* return the object value: nextHopType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjnextHopTypeValue,
                           sizeof (objnextHopTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_nextHop
*
* @purpose Get 'nextHop'
 *@description  [nextHop] On remote routes, the address of the next system en
* route. For non-remote routes, a zero length string. The type of
* this address is determined by the value of the
* inetCidrRouteNextHopType object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_nextHop (void *wap, void *bufp)
{

  xLibU32_t objdestinationIpAddressValue;
  xLibU32_t nextObjdestinationIpAddressValue;
  xLibU32_t objprefixLenValue;
  xLibU32_t nextObjprefixLenValue;
  xLibStr256_t objnextHopValue;
  xLibStr256_t nextObjnextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_uint32 bitValue = 0X80000000;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTosNext = 0;
  L7_uint32 IPaddr;
  L7_uint32 length=0;

  /* retrieve key: destinationIpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                          (xLibU8_t *) & objdestinationIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdestinationIpAddressValue, owa.len);

  /* retrieve key: prefixLen */
  owa.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                          (xLibU8_t *) & objprefixLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objprefixLenValue, owa.len);

  /* retrieve key: nextHop */
  owa.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                          (xLibU8_t *) objnextHopValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objnextHopValue, 0, sizeof (objnextHopValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objnextHopValue, owa.len);

  }

  nextObjdestinationIpAddressValue = objdestinationIpAddressValue;
  nextObjprefixLenValue = objprefixLenValue;
  netMask = netMask << (32- objprefixLenValue);
  if (usmDbInetAton(objnextHopValue, &IPaddr) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  
  owa.l7rc = usmDbIpCidrRouteEntryGetNext(L7_UNIT_CURRENT, &objdestinationIpAddressValue, &netMask, &routeTosNext,
                                &IPaddr);
  while(netMask & bitValue)
  {
     length ++;
     bitValue = bitValue >> 1;
  }

  objprefixLenValue= length;
  
  
  if((objdestinationIpAddressValue != nextObjdestinationIpAddressValue)
      || (objprefixLenValue != nextObjprefixLenValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  
  usmDbInetNtoa(IPaddr, nextObjnextHopValue);
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjnextHopValue, strlen(nextObjnextHopValue));

  /* return the object value: nextHop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjnextHopValue, strlen (nextObjnextHopValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_destinationType_destinationIpAddress_prefixLen_policy_nextHopType_nextHop
*
* @purpose Get 'destinationType + destinationIpAddress + prefixLen + policy + nextHopType + nextHop +'
*
* @description [destinationType]: The type of the inetCidrRouteDest address,
*              as defined in the InetAddress MIB. Only those address types
*              that may appear in an actual routing table are allowed
*              as values of this object. 
*              [destinationIpAddress]: The destination IP address of this
*              route.The type of this address is determined by the value of
*              the inetCidrRouteDestType object. The values for the index
*              objects inetCidrRouteDest and inetCidrRoutePfxLen must be
*              consistent. When the value of inetCidrRouteDest (excluding
*              the zone index, if one is present) is x, then the bitwise
*              logical-AND of x with the value of the mask formed from the
*              corresponding index object inetCidrRoutePfxLen MUST be equal
*              to x. If not, then the index pair is not consistent and
*              an inconsistentName error must be returned on SET or CREATE
*              requests. 
*              [prefixLen]: Indicates the number of leading one bits which
*              form the mask to be logical-ANDed with the destination address
*              before being compared to the value in the inetCidrRouteDest
*              field. The values for the index objects inetCidrRouteDest
*              and inetCidrRoutePfxLen must be consistent.When the value
*              of inetCidrRouteDest (excluding the zone index, if one
*              is present) is x, then the bitwise logical-AND of x with the
*              value of the mask formed from the corresponding index object
*              inetCidrRoutePfxLen MUST be equal to x. If not, then the
*              index pair is not consistent and an inconsistentName error
*              must be returned on SET or CREATE requests. 
*              [policy]: This object is an opaque object without any defined
*              semantics. Its purpose is to serve as an additional index
*              which may delineate between multiple entries to the same
*              destination. The value { 0 0 } shall be used as the default
*              value for this object. 
*              [nextHopType]: The type of the inetCidrRouteNextHop address,
*              as defined in the InetAddress MIB. Value should be set to
*              unknown(0) for non-remote routes. Only those address types
*              that may appear in an actual routing table are allowed as
*              values of this object. 
*              [nextHop]: On remote routes, the address of the next system
*              en route. For non-remote routes, a zero length string. The
*              type of this address is determined by the value of the inetCidrRouteNextHopType
*              object. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_routinginetCidrRoute_destinationType_destinationIpAddress_prefixLen_policy_nextHopType_nextHop
(void *wap, void *bufp[], xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owadestinationType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdestinationTypeValue, nextObjdestinationTypeValue;
  fpObjWa_t owadestinationIpAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdestinationIpAddressValue, nextObjdestinationIpAddressValue;
  fpObjWa_t owaprefixLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objprefixLenValue, nextObjprefixLenValue;
  fpObjWa_t owapolicy = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpolicyValue, nextObjpolicyValue;
  fpObjWa_t owanextHopType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnextHopTypeValue, nextObjnextHopTypeValue;
  fpObjWa_t owanextHop = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objnextHopValue, nextObjnextHopValue;
  void *outdestinationType = (void *) bufp[--keyCount];
  void *outdestinationIpAddress = (void *) bufp[--keyCount];
  void *outprefixLen = (void *) bufp[--keyCount];
  void *outpolicy = (void *) bufp[--keyCount];
  void *outnextHopType = (void *) bufp[--keyCount];
  void *outnextHop = (void *) bufp[--keyCount];

  L7_uint32 bitValue = 0X80000000;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTosNext = 0;
  L7_uint32 IPaddr;
  L7_uint32 length;

  FPOBJ_TRACE_ENTER (outdestinationType);
  FPOBJ_TRACE_ENTER (outdestinationIpAddress);
  FPOBJ_TRACE_ENTER (outprefixLen);
  FPOBJ_TRACE_ENTER (outpolicy);
  FPOBJ_TRACE_ENTER (outnextHopType);
  FPOBJ_TRACE_ENTER (outnextHop);

  /* retrieve key: destinationType */
  owadestinationType.rc =
    xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                   (xLibU8_t *) & objdestinationTypeValue,
                   &owadestinationType.len);
  if (owadestinationType.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: destinationIpAddress */
    owadestinationIpAddress.rc =
      xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                     (xLibU8_t *) & objdestinationIpAddressValue,
                     &owadestinationIpAddress.len);
    if (owadestinationIpAddress.rc == XLIBRC_SUCCESS)
    {
      /* retrieve key: prefixLen */
      owaprefixLen.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                                       (xLibU8_t *) & objprefixLenValue,
                                       &owaprefixLen.len);
      if (owaprefixLen.rc == XLIBRC_SUCCESS)
      {
        /* retrieve key: policy */
        owapolicy.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                                      (xLibU8_t *) objpolicyValue,
                                      &owapolicy.len);
        if (owapolicy.rc == XLIBRC_SUCCESS)
        {
          /* retrieve key: nextHopType */
          owanextHopType.rc =
            xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & objnextHopTypeValue,
                           &owanextHopType.len);
          if (owanextHopType.rc == XLIBRC_SUCCESS)
          {
            /* retrieve key: nextHop */
            owanextHop.rc =
              xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                             (xLibU8_t *) objnextHopValue, &owanextHop.len);
          }
        }
      }
    }
  }
  else
  {
    objdestinationTypeValue = 0;
    nextObjdestinationTypeValue = 0; 
    objdestinationIpAddressValue = 0;
    nextObjdestinationIpAddressValue = 0;
    objprefixLenValue = 0;
    nextObjprefixLenValue = 0;
    memset(objpolicyValue, 0x0, sizeof(objpolicyValue));
    memset(nextObjpolicyValue, 0x0, sizeof(nextObjpolicyValue));
    objnextHopTypeValue = 0;
    nextObjnextHopTypeValue = 0;
    memset(objnextHopValue, 0x0, sizeof(objnextHopValue));
    memset(nextObjnextHopValue, 0x0, sizeof(nextObjnextHopValue));
  }

  FPOBJ_TRACE_CURRENT_KEY (outdestinationType, &objdestinationTypeValue,
                           owadestinationType.len);
  FPOBJ_TRACE_CURRENT_KEY (outdestinationIpAddress,
                           &objdestinationIpAddressValue,
                           owadestinationIpAddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outprefixLen, &objprefixLenValue, owaprefixLen.len);
  FPOBJ_TRACE_CURRENT_KEY (outpolicy, &objpolicyValue, owapolicy.len);
  FPOBJ_TRACE_CURRENT_KEY (outnextHopType, &objnextHopTypeValue,
                           owanextHopType.len);
  FPOBJ_TRACE_CURRENT_KEY (outnextHop, &objnextHopValue, owanextHop.len);

  netMask = netMask << (32- objprefixLenValue);

  usmDbInetAton(objnextHopValue, &IPaddr);

  owa.rc = usmDbIpCidrRouteEntryGetNext(L7_UNIT_CURRENT, &objdestinationIpAddressValue, &netMask, &routeTosNext,
                                 &IPaddr);

  nextObjdestinationTypeValue = objdestinationTypeValue;
  nextObjdestinationIpAddressValue = objdestinationIpAddressValue;
  nextObjprefixLenValue = objprefixLenValue;
  memcpy(nextObjpolicyValue, objpolicyValue, sizeof(nextObjpolicyValue));
  nextObjnextHopTypeValue = objnextHopTypeValue;
  memcpy(nextObjnextHopValue, objnextHopValue, sizeof(nextObjnextHopValue));

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outdestinationType, owadestinationType);
    FPOBJ_TRACE_EXIT (outdestinationIpAddress, owadestinationIpAddress);
    FPOBJ_TRACE_EXIT (outprefixLen, owaprefixLen);
    FPOBJ_TRACE_EXIT (outpolicy, owapolicy);
    FPOBJ_TRACE_EXIT (outnextHopType, owanextHopType);
    FPOBJ_TRACE_EXIT (outnextHop, owanextHop);
    return owa.rc;
  }
  
  while(netMask & bitValue)
  {
    length ++;
    bitValue = bitValue >> 1;
  }

  objprefixLenValue= length;

  nextObjdestinationTypeValue = 0; 
  objdestinationTypeValue = 0;
  nextObjdestinationIpAddressValue = 0;
  objdestinationIpAddressValue = 0;
  nextObjprefixLenValue = 0;
  objprefixLenValue = 0;
  memcpy(nextObjpolicyValue, objpolicyValue, sizeof(nextObjpolicyValue));
  nextObjnextHopTypeValue = 0;
  objnextHopTypeValue = 0;
  memcpy(nextObjnextHopValue, objnextHopValue, sizeof(nextObjnextHopValue));

  FPOBJ_TRACE_CURRENT_KEY (outdestinationType, &nextObjdestinationTypeValue,
                           owadestinationType.len);
  FPOBJ_TRACE_CURRENT_KEY (outdestinationIpAddress,
                           &nextObjdestinationIpAddressValue,
                           owadestinationIpAddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outprefixLen, &nextObjprefixLenValue,
                           owaprefixLen.len);
  FPOBJ_TRACE_CURRENT_KEY (outpolicy, &nextObjpolicyValue, owapolicy.len);
  FPOBJ_TRACE_CURRENT_KEY (outnextHopType, &nextObjnextHopTypeValue,
                           owanextHopType.len);
  FPOBJ_TRACE_CURRENT_KEY (outnextHop, &nextObjnextHopValue, owanextHop.len);

  /* return the object value: destinationType */
  xLibBufDataSet (outdestinationType,
                  (xLibU8_t *) & nextObjdestinationTypeValue,
                  sizeof (nextObjdestinationTypeValue));

  /* return the object value: destinationIpAddress */
  xLibBufDataSet (outdestinationIpAddress,
                  (xLibU8_t *) & nextObjdestinationIpAddressValue,
                  sizeof (nextObjdestinationIpAddressValue));

  /* return the object value: prefixLen */
  xLibBufDataSet (outprefixLen,
                  (xLibU8_t *) & nextObjprefixLenValue,
                  sizeof (nextObjprefixLenValue));

  /* return the object value: policy */
  xLibBufDataSet (outpolicy,
                  (xLibU8_t *) nextObjpolicyValue, strlen (nextObjpolicyValue));

  /* return the object value: nextHopType */
  xLibBufDataSet (outnextHopType,
                  (xLibU8_t *) & nextObjnextHopTypeValue,
                  sizeof (nextObjnextHopTypeValue));

  /* return the object value: nextHop */
  xLibBufDataSet (outnextHop,
                  (xLibU8_t *) nextObjnextHopValue,
                  strlen (nextObjnextHopValue));
  FPOBJ_TRACE_EXIT (outdestinationType, owadestinationType);
  FPOBJ_TRACE_EXIT (outdestinationIpAddress, owadestinationIpAddress);
  FPOBJ_TRACE_EXIT (outprefixLen, owaprefixLen);
  FPOBJ_TRACE_EXIT (outpolicy, owapolicy);
  FPOBJ_TRACE_EXIT (outnextHopType, owanextHopType);
  FPOBJ_TRACE_EXIT (outnextHop, owanextHop);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_ifIndex
*
* @purpose Get 'ifIndex'
*
* @description [ifIndex]: The ifIndex value which identifies the local interface
*              through which the next hop of this route should be reached.
*              A value of 0 is valid and represents the scenario where
*              no interface is specified. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_ifIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprefixLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keynextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keynextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue;
  L7_uint32 routeTos = 0;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 IPaddr;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: destinationType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                           (xLibU8_t *) & keydestinationTypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationTypeValue, kwa1.len);

  /* retrieve key: destinationIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                           (xLibU8_t *) & keydestinationIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationIpAddressValue, kwa2.len);

  /* retrieve key: prefixLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                           (xLibU8_t *) & keyprefixLenValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprefixLenValue, kwa3.len);

  /* retrieve key: policy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                           (xLibU8_t *) keypolicyValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypolicyValue, kwa4.len);

  /* retrieve key: nextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & keynextHopTypeValue, &kwa5.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynextHopTypeValue, kwa5.len);

  /* retrieve key: nextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                           (xLibU8_t *) keynextHopValue, &kwa6.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keynextHopValue, kwa6.len);

  /* get the value from application */
  netMask = netMask << (32- keyprefixLenValue);

  if (usmDbInetAton(keynextHopValue, &IPaddr) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  owa.l7rc =
    usmDbIpCidrRouteIfIndexGet (L7_UNIT_CURRENT, keydestinationIpAddressValue, netMask, routeTos, IPaddr, &objifIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifIndexValue,
                           sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_type
*
* @purpose Get 'type'
*
* @description [type]: The type of route. Note that local(3) refers to a route
*              for which the next hop is the final destination; remote(4)refers
*              to a route for which the next hop is not the final
*              destination. Routes which do not result in traffic forwarding
*              or rejection should not be displayed even if the implementation
*              keeps them stored internally. reject(2) refers to
*              a route which, if matched, discards the message as unreachable
*              and returns a notification(e.g. ICMP error) to the message
*              sender. This is used in some protocols as a means of
*              correctly aggregating routes. blackhole(5) refers to a route
*              which, if matched, discards the message silently. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_type (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprefixLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keynextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keynextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objtypeValue;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTos = 0;
  FPOBJ_TRACE_ENTER (bufp);
  L7_uint32 IPaddr;

  /* retrieve key: destinationType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                           (xLibU8_t *) & keydestinationTypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationTypeValue, kwa1.len);

  /* retrieve key: destinationIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                           (xLibU8_t *) & keydestinationIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationIpAddressValue, kwa2.len);

  /* retrieve key: prefixLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                           (xLibU8_t *) & keyprefixLenValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprefixLenValue, kwa3.len);

  /* retrieve key: policy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                           (xLibU8_t *) keypolicyValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypolicyValue, kwa4.len);

  /* retrieve key: nextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & keynextHopTypeValue, &kwa5.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynextHopTypeValue, kwa5.len);

  /* retrieve key: nextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                           (xLibU8_t *) keynextHopValue, &kwa6.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keynextHopValue, kwa6.len);

  netMask = netMask << (32- keyprefixLenValue);

  if (usmDbInetAton(keynextHopValue, &IPaddr) != L7_SUCCESS )
  {
    return XLIBRC_FAILURE;
  }

  /* get the value from application */
  owa.l7rc = usmDbIpCidrRouteTypeGet (L7_UNIT_CURRENT, keydestinationIpAddressValue, netMask, routeTos,
                                      IPaddr, &objtypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objtypeValue,
                           sizeof (objtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_protocol
*
* @purpose Get 'protocol'
*
* @description [protocol]: The routing mechanism via which this route was
*              learned. Inclusion of values for gateway routing protocols
*              is not intended to imply that hosts should support those protocols.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_protocol (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprefixLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keynextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keynextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objprotocolValue;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTos = 0, IPaddr;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: destinationType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                           (xLibU8_t *) & keydestinationTypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationTypeValue, kwa1.len);

  /* retrieve key: destinationIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                           (xLibU8_t *) & keydestinationIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationIpAddressValue, kwa2.len);

  /* retrieve key: prefixLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                           (xLibU8_t *) & keyprefixLenValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprefixLenValue, kwa3.len);

  /* retrieve key: policy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                           (xLibU8_t *) keypolicyValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypolicyValue, kwa4.len);

  /* retrieve key: nextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & keynextHopTypeValue, &kwa5.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynextHopTypeValue, kwa5.len);

  /* retrieve key: nextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                           (xLibU8_t *) keynextHopValue, &kwa6.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keynextHopValue, kwa6.len);

  /* get the value from application */
  netMask = netMask << (32- keyprefixLenValue);

  if (usmDbInetAton(keynextHopValue, &IPaddr) != L7_SUCCESS )
  {
    return XLIBRC_FAILURE;
  }
  
  owa.l7rc = usmDbIpCidrRouteProtoGet (L7_UNIT_CURRENT, keydestinationIpAddressValue, netMask, routeTos, IPaddr, &objprotocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: protocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objprotocolValue,
                           sizeof (objprotocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_metric1
*
* @purpose Get 'metric1'
*
* @description [metric1]: The primary routing metric for this route. The semantics
*              of this metric are determined by the routing- protocol
*              specified in the route's inetCidrRouteProto value. If
*              this metric is not used, its value should be set to -1. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_metric1 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprefixLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keynextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keynextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmetric1Value;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTos = 0, IPaddr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: destinationType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                           (xLibU8_t *) & keydestinationTypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationTypeValue, kwa1.len);

  /* retrieve key: destinationIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                           (xLibU8_t *) & keydestinationIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationIpAddressValue, kwa2.len);

  /* retrieve key: prefixLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                           (xLibU8_t *) & keyprefixLenValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprefixLenValue, kwa3.len);

  /* retrieve key: policy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                           (xLibU8_t *) keypolicyValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypolicyValue, kwa4.len);

  /* retrieve key: nextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & keynextHopTypeValue, &kwa5.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynextHopTypeValue, kwa5.len);

  /* retrieve key: nextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                           (xLibU8_t *) keynextHopValue, &kwa6.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keynextHopValue, kwa6.len);

  netMask = netMask << (32- keyprefixLenValue);

  if (usmDbInetAton(keynextHopValue, &IPaddr) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  
  /* get the value from application */
  owa.l7rc =
    usmDbIpCidrRouteMetric1Get (L7_UNIT_CURRENT, keydestinationIpAddressValue, netMask, routeTos, IPaddr, &objmetric1Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: metric1 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmetric1Value,
                           sizeof (objmetric1Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_metric2
*
* @purpose Get 'metric2'
*
* @description [metric2]: An alternate routing metric for this route. The
*              semantics of this metric are determined by the routing protocol
*              specified in the route's inetCidrRouteProto value. If
*              this metric is not used, its value should be set to -1. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_metric2 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprefixLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keynextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keynextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmetric2Value;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTos = 0, IPaddr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: destinationType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                           (xLibU8_t *) & keydestinationTypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationTypeValue, kwa1.len);

  /* retrieve key: destinationIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                           (xLibU8_t *) & keydestinationIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationIpAddressValue, kwa2.len);

  /* retrieve key: prefixLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                           (xLibU8_t *) & keyprefixLenValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprefixLenValue, kwa3.len);

  /* retrieve key: policy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                           (xLibU8_t *) keypolicyValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypolicyValue, kwa4.len);

  /* retrieve key: nextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & keynextHopTypeValue, &kwa5.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynextHopTypeValue, kwa5.len);

  /* retrieve key: nextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                           (xLibU8_t *) keynextHopValue, &kwa6.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keynextHopValue, kwa6.len);
  
  netMask = netMask << (32- keyprefixLenValue);

  if (usmDbInetAton(keynextHopValue, &IPaddr) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbIpCidrRouteMetric2Get (L7_UNIT_CURRENT, keydestinationIpAddressValue, netMask, routeTos, IPaddr, &objmetric2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: metric2 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmetric2Value,
                           sizeof (objmetric2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_metric3
*
* @purpose Get 'metric3'
*
* @description [metric3]: An alternate routing metric for this route. The
*              semantics of this metric are determined by the routing- protocol
*              specified in the route's inetCidrRouteProto value. If
*              this metric is not used, its value should be set to -1. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_metric3 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprefixLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keynextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keynextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmetric3Value;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTos = 0, IPaddr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: destinationType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                           (xLibU8_t *) & keydestinationTypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationTypeValue, kwa1.len);

  /* retrieve key: destinationIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                           (xLibU8_t *) & keydestinationIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationIpAddressValue, kwa2.len);

  /* retrieve key: prefixLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                           (xLibU8_t *) & keyprefixLenValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprefixLenValue, kwa3.len);

  /* retrieve key: policy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                           (xLibU8_t *) keypolicyValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypolicyValue, kwa4.len);

  /* retrieve key: nextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & keynextHopTypeValue, &kwa5.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynextHopTypeValue, kwa5.len);

  /* retrieve key: nextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                           (xLibU8_t *) keynextHopValue, &kwa6.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keynextHopValue, kwa6.len);

  /* get the value from application */
  netMask = netMask << (32- keyprefixLenValue);

  if (usmDbInetAton(keynextHopValue, &IPaddr) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  owa.l7rc =
    usmDbIpCidrRouteMetric3Get (L7_UNIT_CURRENT, keydestinationIpAddressValue, netMask, routeTos, IPaddr, &objmetric3Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: metric3 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmetric3Value,
                           sizeof (objmetric3Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_metric4
*
* @purpose Get 'metric4'
*
* @description [metric4]: An alternate routing metric for this route. The
*              semantics of this metric are determined by the routing- protocol
*              specified in the route's inetCidrRouteProto value. If
*              this metric is not used, its value should be set to -1. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_metric4 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprefixLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keynextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keynextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmetric4Value;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTos = 0, IPaddr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: destinationType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                           (xLibU8_t *) & keydestinationTypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationTypeValue, kwa1.len);

  /* retrieve key: destinationIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                           (xLibU8_t *) & keydestinationIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationIpAddressValue, kwa2.len);

  /* retrieve key: prefixLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                           (xLibU8_t *) & keyprefixLenValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprefixLenValue, kwa3.len);

  /* retrieve key: policy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                           (xLibU8_t *) keypolicyValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypolicyValue, kwa4.len);

  /* retrieve key: nextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & keynextHopTypeValue, &kwa5.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynextHopTypeValue, kwa5.len);

  /* retrieve key: nextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                           (xLibU8_t *) keynextHopValue, &kwa6.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keynextHopValue, kwa6.len);

  netMask = netMask << (32- keyprefixLenValue);

  if (usmDbInetAton(keynextHopValue, &IPaddr) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbIpCidrRouteMetric4Get (L7_UNIT_CURRENT, keydestinationIpAddressValue, netMask, routeTos, IPaddr, &objmetric4Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: metric4 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmetric4Value,
                           sizeof (objmetric4Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_metric5
*
* @purpose Get 'metric5'
*
* @description [metric5]: An alternate routing metric for this route. The
*              semantics of this metric are determined by the routing- protocol
*              specified in the route's inetCidrRouteProto value. If
*              this metric is not used, its value should be set to -1. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_metric5 (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprefixLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keynextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keynextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmetric5Value;
  L7_uint32 netMask = 0xffffffff;
  L7_uint32 routeTos = 0, IPaddr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: destinationType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                           (xLibU8_t *) & keydestinationTypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationTypeValue, kwa1.len);

  /* retrieve key: destinationIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                           (xLibU8_t *) & keydestinationIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationIpAddressValue, kwa2.len);

  /* retrieve key: prefixLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                           (xLibU8_t *) & keyprefixLenValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprefixLenValue, kwa3.len);

  /* retrieve key: policy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                           (xLibU8_t *) keypolicyValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypolicyValue, kwa4.len);

  /* retrieve key: nextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & keynextHopTypeValue, &kwa5.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynextHopTypeValue, kwa5.len);

  /* retrieve key: nextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                           (xLibU8_t *) keynextHopValue, &kwa6.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keynextHopValue, kwa6.len);

  netMask = netMask << (32- keyprefixLenValue);

  if (usmDbInetAton(keynextHopValue, &IPaddr) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }


  /* get the value from application */
  owa.l7rc =
    usmDbIpCidrRouteMetric5Get (L7_UNIT_CURRENT, keydestinationIpAddressValue, netMask,
                                routeTos, IPaddr, &objmetric5Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: metric5 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmetric5Value,
                           sizeof (objmetric5Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_status
*
* @purpose Get 'status'
*
* @description [status]: The row status variable, used according to row installation
*              and removal conventions. A row entry cannot be modified
*              when the status is marked as active(1). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationTypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydestinationIpAddressValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyprefixLenValue;
  fpObjWa_t kwa4 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypolicyValue;
  fpObjWa_t kwa5 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keynextHopTypeValue;
  fpObjWa_t kwa6 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keynextHopValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objstatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: destinationType */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationType,
                           (xLibU8_t *) & keydestinationTypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationTypeValue, kwa1.len);

  /* retrieve key: destinationIpAddress */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_destinationIpAddress,
                           (xLibU8_t *) & keydestinationIpAddressValue,
                           &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydestinationIpAddressValue, kwa2.len);

  /* retrieve key: prefixLen */
  kwa3.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_prefixLen,
                           (xLibU8_t *) & keyprefixLenValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyprefixLenValue, kwa3.len);

  /* retrieve key: policy */
  kwa4.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_policy,
                           (xLibU8_t *) keypolicyValue, &kwa4.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypolicyValue, kwa4.len);

  /* retrieve key: nextHopType */
  kwa5.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHopType,
                           (xLibU8_t *) & keynextHopTypeValue, &kwa5.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keynextHopTypeValue, kwa5.len);

  /* retrieve key: nextHop */
  kwa6.rc = xLibFilterGet (wap, XOBJ_routinginetCidrRoute_nextHop,
                           (xLibU8_t *) keynextHopValue, &kwa6.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keynextHopValue, kwa6.len);

  /* get the value from application */

  objstatusValue = L7_ROW_STATUS_ACTIVE;

  owa.l7rc = L7_SUCCESS;
        
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objstatusValue,
                           sizeof (objstatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routinginetCidrRoute_numberOfEntries
*
* @purpose Get 'numberOfEntries'
*
* @description [numberOfEntries]: The number of current inetCidrRouteTable
*              entries that are not invalid. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routinginetCidrRoute_numberOfEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objnumberOfEntriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIpCidrRouteNumberGet (L7_UNIT_CURRENT, &objnumberOfEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objnumberOfEntriesValue,
                     sizeof (objnumberOfEntriesValue));

  /* return the object value: numberOfEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objnumberOfEntriesValue,
                           sizeof (objnumberOfEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
