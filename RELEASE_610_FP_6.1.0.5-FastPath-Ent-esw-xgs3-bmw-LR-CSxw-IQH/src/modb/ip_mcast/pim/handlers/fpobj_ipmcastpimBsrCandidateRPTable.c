
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimBsrCandidateRPTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  13 May 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimBsrCandidateRPTable_obj.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"
#include "usmdb_iputil_api.h"

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
#include "usmdb_ip6_api.h"
#endif

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_pimsm_api.h"
#endif

#include "l7utils_inet_addr_api.h"
static L7_RC_t
usmDbIpInterfaceNextGet(L7_uint32 UnitIndex, L7_uint32 *intIfNum)
{
  L7_int32 temp_val;
  /* loop through available external interface numbers */
  while (usmDbValidIntIfNumNext(*intIfNum, intIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a valid routing interface */
    if ( (usmDbValidateRtrIntf(UnitIndex, *intIfNum) == L7_SUCCESS) &&
         (usmDbVisibleInterfaceCheck(UnitIndex, *intIfNum, &temp_val) == L7_SUCCESS) )
    return L7_SUCCESS;

  }

  return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateCandidateRPInterface
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex]: The IfIndex associated with this instance.
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateCandidateRPInterface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCandRPInterfaceValue;
  xLibU32_t nextObjCandRPInterfaceValue;

  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;

  FPOBJ_TRACE_ENTER (bufp);


  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);


  /* retrieve key: IfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateCandidateRPInterface,
                          (xLibU8_t *) & objCandRPInterfaceValue, &owa.len);

  nextObjCandRPInterfaceValue = 0;
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    if( (owa.l7rc = usmDbPimsmMapCandRPConfiguredIntfGet( keypimBsrCandidateRPAddressTypeValue, &nextObjCandRPInterfaceValue )) != L7_SUCCESS )
    {
      nextObjCandRPInterfaceValue = 0;
      owa.l7rc = usmDbIpInterfaceNextGet(L7_UNIT_CURRENT, &nextObjCandRPInterfaceValue);
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objCandRPInterfaceValue, owa.len);

    if( usmDbPimsmMapCandRPConfiguredIntfGet( keypimBsrCandidateRPAddressTypeValue, &nextObjCandRPInterfaceValue ) != L7_SUCCESS )
    {
      nextObjCandRPInterfaceValue = objCandRPInterfaceValue;
      owa.l7rc = usmDbIpInterfaceNextGet(L7_UNIT_CURRENT, &nextObjCandRPInterfaceValue);
    }
    else
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
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjCandRPInterfaceValue, owa.len);

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjCandRPInterfaceValue,
                           sizeof (objCandRPInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType
*
* @purpose Get 'pimBsrCandidateRPAddressType'
 *@description  [pimBsrCandidateRPAddressType] The Inet address type of the
* Candidate-RP.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPAddressTypeValue;
  xLibU32_t nextObjpimBsrCandidateRPAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                          (xLibU8_t *) & objpimBsrCandidateRPAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimBsrCandidateRPAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateRPAddressTypeValue, owa.len);

    if (objpimBsrCandidateRPAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimBsrCandidateRPAddressTypeValue = objpimBsrCandidateRPAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBsrCandidateRPAddressTypeValue, owa.len);

  /* return the object value: pimBsrCandidateRPAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimBsrCandidateRPAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress
*
* @purpose Get 'pimBsrCandidateRPAddress'
 *@description  [pimBsrCandidateRPAddress] The (unicast) address that will be
* advertised as a Candidate-RP. The InetAddressType is given by the
* pimBsrCandidateRPAddressType object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;

  L7_inet_addr_t objpimBsrCandidateRPAddressValue;
  L7_inet_addr_t nextObjpimBsrCandidateRPAddressValue;
  
  L7_inet_addr_t nextObjpimBsrCandidateRPGroupAddressValue;

  xLibU8_t nextObjpimBsrCandidateRPGroupPrefixLengthValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                          (xLibU8_t *) &objpimBsrCandidateRPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memset (&nextObjpimBsrCandidateRPAddressValue, 0, sizeof (L7_inet_addr_t));

    inetAddressZeroSet( keypimBsrCandidateRPAddressTypeValue, &nextObjpimBsrCandidateRPGroupAddressValue); 
    nextObjpimBsrCandidateRPGroupPrefixLengthValue = 0;

    owa.l7rc =
      usmDbPimsmbsrCandRPEntryNextGet (keypimBsrCandidateRPAddressTypeValue,
                                                              &nextObjpimBsrCandidateRPAddressValue,
                                                              &nextObjpimBsrCandidateRPGroupAddressValue,
                                                              &nextObjpimBsrCandidateRPGroupPrefixLengthValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateRPAddressValue, owa.len);

    memset (&nextObjpimBsrCandidateRPGroupAddressValue, 0, sizeof (L7_inet_addr_t));
    nextObjpimBsrCandidateRPGroupPrefixLengthValue = 0;
    memcpy(&nextObjpimBsrCandidateRPAddressValue, &objpimBsrCandidateRPAddressValue,sizeof (L7_inet_addr_t));
    
    do
    {
      owa.l7rc =
        usmDbPimsmbsrCandRPEntryNextGet (keypimBsrCandidateRPAddressTypeValue,
                                                                &nextObjpimBsrCandidateRPAddressValue,
                                                                &nextObjpimBsrCandidateRPGroupAddressValue,
                                                                &nextObjpimBsrCandidateRPGroupPrefixLengthValue);
    }
    while (!memcmp(&objpimBsrCandidateRPAddressValue, &nextObjpimBsrCandidateRPAddressValue,sizeof (L7_inet_addr_t))
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBsrCandidateRPAddressValue, owa.len);

  /* return the object value: pimBsrCandidateRPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimBsrCandidateRPAddressValue,
                           sizeof (objpimBsrCandidateRPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress
*
* @purpose Get 'pimBsrCandidateRPGroupAddress'
 *@description  [pimBsrCandidateRPGroupAddress] The IP multicast group address
* that, when combined with the corresponding value of
* pimBsrCandidateRPGroupPrefixLength, identifies a group prefix for which the
* local router will advertise itself as a Candidate-RP. The
* InetAddressType is given by the pimBsrCandidateRPAddressType object. This
* address object is only significant up to
* pimBsrCandidateRPGroupPrefixLength bits. The remainder of the address bits are zero. This
* is especially important for this field, which is part of the
* index of this entry. Any non-zero bits would signify an entirely
* different entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress (void *wap,
                                                                               void *bufp)
{
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;

  L7_inet_addr_t objpimBsrCandidateRPAddressValue;
  L7_inet_addr_t nextObjpimBsrCandidateRPAddressValue;
  
  L7_inet_addr_t objpimBsrCandidateRPGroupAddressValue;
  L7_inet_addr_t nextObjpimBsrCandidateRPGroupAddressValue;

  xLibU8_t nextObjpimBsrCandidateRPGroupPrefixLengthValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                          (xLibU8_t *) &objpimBsrCandidateRPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateRPAddressValue, owa.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                          (xLibU8_t *) &objpimBsrCandidateRPGroupAddressValue, &owa.len);

  memcpy (&nextObjpimBsrCandidateRPAddressValue, &objpimBsrCandidateRPAddressValue, sizeof (L7_inet_addr_t));

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);


    inetAddressZeroSet( keypimBsrCandidateRPAddressTypeValue, &nextObjpimBsrCandidateRPGroupAddressValue); 
    nextObjpimBsrCandidateRPGroupPrefixLengthValue = 0;
    
    owa.l7rc =
      usmDbPimsmbsrCandRPEntryNextGet (keypimBsrCandidateRPAddressTypeValue,
                                                              &nextObjpimBsrCandidateRPAddressValue,
                                                              &nextObjpimBsrCandidateRPGroupAddressValue,
                                                              &nextObjpimBsrCandidateRPGroupPrefixLengthValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateRPGroupAddressValue, owa.len);

    memcpy (&nextObjpimBsrCandidateRPGroupAddressValue, &objpimBsrCandidateRPGroupAddressValue, sizeof (L7_inet_addr_t));

    nextObjpimBsrCandidateRPGroupPrefixLengthValue = 0;
   do {
      owa.l7rc =
        usmDbPimsmbsrCandRPEntryNextGet (keypimBsrCandidateRPAddressTypeValue,
                                                                &nextObjpimBsrCandidateRPAddressValue,
                                                                &nextObjpimBsrCandidateRPGroupAddressValue,
                                                                &nextObjpimBsrCandidateRPGroupPrefixLengthValue);
    } while (owa.l7rc == L7_SUCCESS && 
             !memcmp(&objpimBsrCandidateRPGroupAddressValue, &nextObjpimBsrCandidateRPGroupAddressValue, sizeof (L7_inet_addr_t))
             );


  }

  if (memcmp(&objpimBsrCandidateRPAddressValue, &nextObjpimBsrCandidateRPAddressValue, sizeof (L7_inet_addr_t))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBsrCandidateRPGroupAddressValue, owa.len);

  /* return the object value: pimBsrCandidateRPGroupAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimBsrCandidateRPGroupAddressValue,
                           sizeof (objpimBsrCandidateRPGroupAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength
*
* @purpose Get 'pimBsrCandidateRPGroupPrefixLength'
 *@description  [pimBsrCandidateRPGroupPrefixLength] The multicast group address
* mask that, when combined with the corresponding value of
* pimBsrCandidateRPGroupAddress, identifies a group prefix for which the
* local router will advertise itself as a Candidate-RP. The
* InetAddressType is given by the pimBsrCandidateRPAddressType object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength (void *wap,
                                                                                    void *bufp)
{
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;

  L7_inet_addr_t objpimBsrCandidateRPAddressValue;
  L7_inet_addr_t nextObjpimBsrCandidateRPAddressValue;
  
  L7_inet_addr_t objpimBsrCandidateRPGroupAddressValue;
  L7_inet_addr_t nextObjpimBsrCandidateRPGroupAddressValue;

  xLibV4V6Mask_t objpimBsrCandidateRPGroupPrefixLengthValue;
  xLibU8_t nextObjpimBsrCandidateRPGroupPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);
/*  L7_uchar8 buff1[50], buff2[50];*/
  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                          (xLibU8_t *) &objpimBsrCandidateRPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateRPAddressValue, owa.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                          (xLibU8_t *) &objpimBsrCandidateRPGroupAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateRPGroupAddressValue, owa.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  owa.len = sizeof(xLibV4V6Mask_t);
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & objpimBsrCandidateRPGroupPrefixLengthValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    
    nextObjpimBsrCandidateRPGroupPrefixLengthValue = 0;
    memcpy (&nextObjpimBsrCandidateRPAddressValue, &objpimBsrCandidateRPAddressValue, sizeof (L7_inet_addr_t));
    memcpy (&nextObjpimBsrCandidateRPGroupAddressValue, &objpimBsrCandidateRPGroupAddressValue, sizeof (L7_inet_addr_t));
            
    owa.l7rc =
      usmDbPimsmbsrCandRPEntryNextGet (keypimBsrCandidateRPAddressTypeValue,
                                                              &nextObjpimBsrCandidateRPAddressValue,
                                                              &nextObjpimBsrCandidateRPGroupAddressValue,
                                                              &nextObjpimBsrCandidateRPGroupPrefixLengthValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateRPGroupPrefixLengthValue, owa.len);

    memcpy (&nextObjpimBsrCandidateRPAddressValue, &objpimBsrCandidateRPAddressValue, sizeof (L7_inet_addr_t));
    memcpy (&nextObjpimBsrCandidateRPGroupAddressValue, &objpimBsrCandidateRPGroupAddressValue, sizeof (L7_inet_addr_t));

    nextObjpimBsrCandidateRPGroupPrefixLengthValue = objpimBsrCandidateRPGroupPrefixLengthValue.addr;
    owa.l7rc =
      usmDbPimsmbsrCandRPEntryNextGet (keypimBsrCandidateRPAddressTypeValue,
                                                              &nextObjpimBsrCandidateRPAddressValue,
                                                              &nextObjpimBsrCandidateRPGroupAddressValue,
                                                              &nextObjpimBsrCandidateRPGroupPrefixLengthValue);

  }

  if (memcmp(&objpimBsrCandidateRPAddressValue, &nextObjpimBsrCandidateRPAddressValue, sizeof(L7_inet_addr_t))
      || (memcmp(&objpimBsrCandidateRPGroupAddressValue, &nextObjpimBsrCandidateRPGroupAddressValue, sizeof(L7_inet_addr_t)))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBsrCandidateRPGroupPrefixLengthValue, owa.len);

  /* return the object value: pimBsrCandidateRPGroupPrefixLength */
  objpimBsrCandidateRPGroupPrefixLengthValue.family = keypimBsrCandidateRPAddressTypeValue;
  objpimBsrCandidateRPGroupPrefixLengthValue.addr = nextObjpimBsrCandidateRPGroupPrefixLengthValue;
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateRPGroupPrefixLengthValue,
                           sizeof (objpimBsrCandidateRPGroupPrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPBidir
*
* @purpose Get 'pimBsrCandidateRPBidir'
 *@description  [pimBsrCandidateRPBidir] If this object is set to TRUE, this
* group range is advertised with this RP as a BIDIR-PIM group range.
* If it is set to FALSE, it is advertised as a PIM-SM group range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPBidir (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;

  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;

  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;

  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPBidirValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              &objpimBsrCandidateRPBidirValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateRPBidir */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateRPBidirValue,
                           sizeof (objpimBsrCandidateRPBidirValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPBidir
*
* @purpose Set 'pimBsrCandidateRPBidir'
 *@description  [pimBsrCandidateRPBidir] If this object is set to TRUE, this
* group range is advertised with this RP as a BIDIR-PIM group range.
* If it is set to FALSE, it is advertised as a PIM-SM group range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPBidir (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPBidirValue;

  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateRPBidir */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateRPBidirValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateRPBidirValue, owa.len);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              objpimBsrCandidateRPBidirValue);

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
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAdvTimer
*
* @purpose Get 'pimBsrCandidateRPAdvTimer'
 *@description  [pimBsrCandidateRPAdvTimer] The time remaining before the local
* router next sends a Candidate-RP-Advertisement to the elected BSR
* for this zone.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAdvTimer (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPAdvTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              &objpimBsrCandidateRPAdvTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateRPAdvTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateRPAdvTimerValue,
                           sizeof (objpimBsrCandidateRPAdvTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPPriority
*
* @purpose Get 'pimBsrCandidateRPPriority'
 *@description  [pimBsrCandidateRPPriority] The priority for this Candidate RP
* advertised in Candidate-RP-Advertisements.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPPriority (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              &objpimBsrCandidateRPPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateRPPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateRPPriorityValue,
                           sizeof (objpimBsrCandidateRPPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPPriority
*
* @purpose Set 'pimBsrCandidateRPPriority'
 *@description  [pimBsrCandidateRPPriority] The priority for this Candidate RP
* advertised in Candidate-RP-Advertisements.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPPriority (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPPriorityValue;

  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateRPPriority */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateRPPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateRPPriorityValue, owa.len);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              objpimBsrCandidateRPPriorityValue);

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
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAdvInterval
*
* @purpose Get 'pimBsrCandidateRPAdvInterval'
 *@description  [pimBsrCandidateRPAdvInterval] A Candidate RP generates
* Candidate-RP-Advertisements periodically. This object represents the time
* interval in seconds between two consecutive advertisements.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAdvInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPAdvIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              &objpimBsrCandidateRPAdvIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateRPAdvInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateRPAdvIntervalValue,
                           sizeof (objpimBsrCandidateRPAdvIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAdvInterval
*
* @purpose Set 'pimBsrCandidateRPAdvInterval'
 *@description  [pimBsrCandidateRPAdvInterval] A Candidate RP generates
* Candidate-RP-Advertisements periodically. This object represents the time
* interval in seconds between two consecutive advertisements.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAdvInterval (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPAdvIntervalValue;

  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateRPAdvInterval */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateRPAdvIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateRPAdvIntervalValue, owa.len);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              objpimBsrCandidateRPAdvIntervalValue);

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
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPHoldtime
*
* @purpose Get 'pimBsrCandidateRPHoldtime'
 *@description  [pimBsrCandidateRPHoldtime] Holdtime for this Candidate RP. The
* amount of time (in seconds) this Candidate-RP entry is valid.
* This object's value can be zero only when this C-RP is shutting down   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPHoldtime (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPHoldtimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              &objpimBsrCandidateRPHoldtimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateRPHoldtime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateRPHoldtimeValue,
                           sizeof (objpimBsrCandidateRPHoldtimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPHoldtime
*
* @purpose Set 'pimBsrCandidateRPHoldtime'
 *@description  [pimBsrCandidateRPHoldtime] Holdtime for this Candidate RP. The
* amount of time (in seconds) this Candidate-RP entry is valid.
* This object's value can be zero only when this C-RP is shutting down   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPHoldtime (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPHoldtimeValue;

  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateRPHoldtime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateRPHoldtimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateRPHoldtimeValue, owa.len);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              objpimBsrCandidateRPHoldtimeValue);

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
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPStatus
*
* @purpose Get 'pimBsrCandidateRPStatus'
 *@description  [pimBsrCandidateRPStatus] The status of this row, by which new
* entries may be created, or old entries deleted from this table.
* This status object can be set to active(1) without setting any
* other columnar objects in this entry. All writable objects in this
* entry can be modified when the status of this entry is active(1).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPStatus (void *wap, void *bufp)
{
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;

  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimBsrCandidateRPAddressValue;

  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimBsrCandidateRPGroupAddressValue;

  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimBsrCandidateRPGroupPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) &keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) &keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmbsrCandRPModeGet (keypimBsrCandidateRPAddressTypeValue,
                                                                    &keypimBsrCandidateRPAddressValue,
                                                                    &keypimBsrCandidateRPGroupAddressValue,
                                                                    keypimBsrCandidateRPGroupPrefixLengthValue.addr,
                                                                    &objpimBsrCandidateRPStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateRPStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateRPStatusValue,
                           sizeof (objpimBsrCandidateRPStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPStatus
*
* @purpose Set 'pimBsrCandidateRPStatus'
 *@description  [pimBsrCandidateRPStatus] The status of this row, by which new
* entries may be created, or old entries deleted from this table.
* This status object can be set to active(1) without setting any
* other columnar objects in this entry. All writable objects in this
* entry can be modified when the status of this entry is active(1).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPStatusValue;

  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimBsrCandidateRPGroupAddressValue;

  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimBsrCandidateRPGroupPrefixLengthValue;

  fpObjWa_t objpimBsrCandidateInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateInterfaceValue;

  L7_inet_addr_t bsrCandtRPGrpMask;
  xLibU32_t      v4mask = 0x0;


  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve object: pimBsrCandidateRPStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateRPStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateRPStatusValue, owa.len);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);



  /* Retrieve interface key */
  objpimBsrCandidateInterface.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateCandidateRPInterface,
                   (xLibU8_t *) & objpimBsrCandidateInterfaceValue,
                   &objpimBsrCandidateInterface.len);

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrCandidateInterfaceValue,
                           objpimBsrCandidateInterface.len);

  if (objpimBsrCandidateInterface.rc != XLIBRC_SUCCESS)
  {
    objpimBsrCandidateInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, objpimBsrCandidateInterface);
    return objpimBsrCandidateInterface.rc;
  }

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) &keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  inetAddressZeroSet(keypimBsrCandidateRPAddressTypeValue, &bsrCandtRPGrpMask);
  owa.l7rc = inetMaskLenToMask(keypimBsrCandidateRPAddressTypeValue, 
                                                keypimBsrCandidateRPGroupPrefixLengthValue.addr, 
                                                &bsrCandtRPGrpMask);


  if (owa.l7rc != L7_SUCCESS)
  {
    if (keypimBsrCandidateRPAddressTypeValue == L7_AF_INET)
    {
      owa.rc = XLIBRC_INVALID_SUBNET_MASK;    /* TODO: Change if required */
    }
    else if ((keypimBsrCandidateRPAddressTypeValue == L7_AF_INET6) &&
             (keypimBsrCandidateRPGroupPrefixLengthValue.addr < 1  || keypimBsrCandidateRPGroupPrefixLengthValue.addr > 128))
    {
      owa.rc = XLIBRC_INVALID_PREFIX_LENGTH;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (keypimBsrCandidateRPAddressTypeValue == L7_AF_INET)
  {
    inetAddressGet(keypimBsrCandidateRPAddressTypeValue, &bsrCandtRPGrpMask, &v4mask);
    if (usmDbNetmaskValidate(v4mask) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_INVALID_SUBNET_MASK;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  /* PIMSM supports only global scope range. Hence 4th nibble of the group address, which signifies the scope zone the group belongs to, should always be E.
  Check for the same. The family issue is taken care inside the API itself.*/
  if(inetIsAddrMulticastScope(&keypimBsrCandidateRPGroupAddressValue) == L7_TRUE)
  {
    owa.rc = XLIBRC_PIM_RP_CANDIDATE_GROUP_ADDRESS_NOT_IN_GLOBAL_SCOPE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  if (objpimBsrCandidateRPStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* set the value in application */
    owa.l7rc = usmDbPimsmCandRPSet (keypimBsrCandidateRPAddressTypeValue,
                                                          objpimBsrCandidateInterfaceValue,
                                                          &keypimBsrCandidateRPGroupAddressValue,
                                                          &bsrCandtRPGrpMask,
                                                          L7_ENABLE);
  }
  else if (objpimBsrCandidateRPStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* set the value in application */
    owa.l7rc = usmDbPimsmCandRPSet (keypimBsrCandidateRPAddressTypeValue,
                                                          objpimBsrCandidateInterfaceValue,
                                                          &keypimBsrCandidateRPGroupAddressValue,
                                                          &bsrCandtRPGrpMask,
                                                          L7_DISABLE);
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPStorageType
*
* @purpose Get 'pimBsrCandidateRPStorageType'
 *@description  [pimBsrCandidateRPStorageType] The storage type for this row.
* Rows having the value 'permanent' need not allow write-access to
* any columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              &objpimBsrCandidateRPStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrCandidateRPStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrCandidateRPStorageTypeValue,
                           sizeof (objpimBsrCandidateRPStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPStorageType
*
* @purpose Set 'pimBsrCandidateRPStorageType'
 *@description  [pimBsrCandidateRPStorageType] The storage type for this row.
* Rows having the value 'permanent' need not allow write-access to
* any columnar objects in the row.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrCandidateRPStorageTypeValue;

  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;
  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrCandidateRPGroupAddressValue;
  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPGroupPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimBsrCandidateRPStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimBsrCandidateRPStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimBsrCandidateRPStorageTypeValue, owa.len);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimBsrCandidateRPAddressTypeValue,
                              keypimBsrCandidateRPAddressValue,
                              keypimBsrCandidateRPGroupAddressValue,
                              keypimBsrCandidateRPGroupPrefixLengthValue,
                              objpimBsrCandidateRPStorageTypeValue);

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
* @function fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateCandidateRPInterfaceConfig
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex]: The IfIndex associated with this instance.
*
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrCandidateRPTable_pimBsrCandidateCandidateRPInterfaceConfig (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCandRPInterfaceConfigValue;
  fpObjWa_t kwapimBsrCandidateRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrCandidateRPAddressTypeValue;

  fpObjWa_t kwapimBsrCandidateRPAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimBsrCandidateRPAddressValue;

  fpObjWa_t kwapimBsrCandidateRPGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimBsrCandidateRPGroupAddressValue;

  fpObjWa_t kwapimBsrCandidateRPGroupPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimBsrCandidateRPGroupPrefixLengthValue;
  
  


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrCandidateRPAddressType */
  kwapimBsrCandidateRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddressType,
                   (xLibU8_t *) & keypimBsrCandidateRPAddressTypeValue,
                   &kwapimBsrCandidateRPAddressType.len);
  if (kwapimBsrCandidateRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddressType);
    return kwapimBsrCandidateRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressTypeValue,
                           kwapimBsrCandidateRPAddressType.len);

  /* retrieve key: pimBsrCandidateRPAddress */
  kwapimBsrCandidateRPAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPAddress,
                   (xLibU8_t *) &keypimBsrCandidateRPAddressValue, &kwapimBsrCandidateRPAddress.len);
  if (kwapimBsrCandidateRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPAddress);
    return kwapimBsrCandidateRPAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPAddressValue, kwapimBsrCandidateRPAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupAddress */
  kwapimBsrCandidateRPGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupAddress,
                   (xLibU8_t *) &keypimBsrCandidateRPGroupAddressValue,
                   &kwapimBsrCandidateRPGroupAddress.len);
  if (kwapimBsrCandidateRPGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupAddress);
    return kwapimBsrCandidateRPGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupAddressValue,
                           kwapimBsrCandidateRPGroupAddress.len);

  /* retrieve key: pimBsrCandidateRPGroupPrefixLength */
  kwapimBsrCandidateRPGroupPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrCandidateRPTable_pimBsrCandidateRPGroupPrefixLength,
                   (xLibU8_t *) & keypimBsrCandidateRPGroupPrefixLengthValue,
                   &kwapimBsrCandidateRPGroupPrefixLength.len);
  if (kwapimBsrCandidateRPGroupPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrCandidateRPGroupPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrCandidateRPGroupPrefixLength);
    return kwapimBsrCandidateRPGroupPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrCandidateRPGroupPrefixLengthValue,
                           kwapimBsrCandidateRPGroupPrefixLength.len);
  
  


  owa.l7rc = usmDbPimsmbsrCandRPInterfaceGet(keypimBsrCandidateRPAddressTypeValue,
                                        &keypimBsrCandidateRPAddressValue,
                                        &keypimBsrCandidateRPGroupAddressValue,
                                        keypimBsrCandidateRPGroupPrefixLengthValue.addr, &objCandRPInterfaceConfigValue);


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objCandRPInterfaceConfigValue,
                           sizeof (objCandRPInterfaceConfigValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

