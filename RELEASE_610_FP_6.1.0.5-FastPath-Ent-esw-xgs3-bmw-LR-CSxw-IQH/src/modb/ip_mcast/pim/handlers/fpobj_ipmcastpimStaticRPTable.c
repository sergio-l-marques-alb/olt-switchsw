
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimStaticRPTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimStaticRPTable_obj.h"
#include "usmdb_mib_pimsm_api.h"
#include "usmdb_pimsm_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimStaticRPTable_pimStaticRPAddressType
*
* @purpose Get 'pimStaticRPAddressType'
*
* @description [pimStaticRPAddressType] The address type of this entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStaticRPTable_pimStaticRPAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPAddressTypeValue;
  xLibU32_t nextObjpimStaticRPAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStaticRPAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                          (xLibU8_t *) & objpimStaticRPAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimStaticRPAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStaticRPAddressTypeValue, owa.len);

    if (objpimStaticRPAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimStaticRPAddressTypeValue = objpimStaticRPAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStaticRPAddressTypeValue, owa.len);

  /* return the object value: pimStaticRPAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimStaticRPAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStaticRPTable_pimStaticRPGrpAddress
*
* @purpose Get 'pimStaticRPGrpAddress'
*
* @description [pimStaticRPGrpAddress] The multicast group address that, when combined with pimStaticRPGrpPrefixLength, gives the group prefix for this entry. The InetAddressType is given by the pimStaticRPAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStaticRPTable_pimStaticRPGrpAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;

  L7_inet_addr_t objpimStaticRPGrpAddressValue;
  L7_inet_addr_t nextObjpimStaticRPGrpAddressValue;

  static  xLibU8_t nextObjpimStaticRPGrpPrefixLengthValue=0;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                          (xLibU8_t *) &objpimStaticRPGrpAddressValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjpimStaticRPGrpPrefixLengthValue = 0;
    inetAddressZeroSet(keypimStaticRPAddressTypeValue, &nextObjpimStaticRPGrpAddressValue);
    
    owa.l7rc = usmDbPimsmStaticRPEntryNextGet (keypimStaticRPAddressTypeValue,
                                                                         &nextObjpimStaticRPGrpAddressValue,
                                                                         (xLibU8_t *)&nextObjpimStaticRPGrpPrefixLengthValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStaticRPGrpAddressValue, owa.len);

    /*nextObjpimStaticRPGrpPrefixLengthValue = 0;*/
    memcpy(&nextObjpimStaticRPGrpAddressValue, &objpimStaticRPGrpAddressValue, sizeof(L7_inet_addr_t));
    
    do
    {
      owa.l7rc = usmDbPimsmStaticRPEntryNextGet (keypimStaticRPAddressTypeValue,
                                                                           &nextObjpimStaticRPGrpAddressValue,
                                                                           (xLibU8_t *)&nextObjpimStaticRPGrpPrefixLengthValue);
    }
    while (!memcmp(&objpimStaticRPGrpAddressValue, &nextObjpimStaticRPGrpAddressValue, sizeof(L7_inet_addr_t))
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStaticRPGrpAddressValue, owa.len);

  /* return the object value: pimStaticRPGrpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimStaticRPGrpAddressValue,
                           sizeof (objpimStaticRPGrpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength
*
* @purpose Get 'pimStaticRPGrpPrefixLength'
*
* @description [pimStaticRPGrpPrefixLength] The multicast group prefix length that, when combined with pimStaticRPGrpAddress, gives the group prefix for this entry.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength (void *wap, void *bufp)
{
  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;

  L7_inet_addr_t objpimStaticRPGrpAddressValue;
  L7_inet_addr_t nextObjpimStaticRPGrpAddressValue;
  
  xLibV4V6Mask_t objpimStaticRPGrpPrefixLengthValue;
  xLibV4V6Mask_t nextObjpimStaticRPGrpPrefixLengthValue;
  L7_uchar8 maskLen=0;


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);


  memset(&nextObjpimStaticRPGrpPrefixLengthValue,0,sizeof(xLibV4V6Mask_t));
  memset(&objpimStaticRPGrpPrefixLengthValue,0,sizeof(xLibV4V6Mask_t));
  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  owa.len = sizeof(L7_inet_addr_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                          (xLibU8_t *) &objpimStaticRPGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStaticRPGrpAddressValue, owa.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  owa.len = sizeof(xLibV4V6Mask_t);
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                          (xLibU8_t *) &objpimStaticRPGrpPrefixLengthValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    inetAddressZeroSet(keypimStaticRPAddressTypeValue, &nextObjpimStaticRPGrpAddressValue);
    nextObjpimStaticRPGrpPrefixLengthValue.addr = 0;
    maskLen=0;

    do
    {
      owa.l7rc = usmDbPimsmStaticRPEntryNextGet (keypimStaticRPAddressTypeValue,
                                                 &nextObjpimStaticRPGrpAddressValue,&maskLen);
      nextObjpimStaticRPGrpPrefixLengthValue.addr = maskLen;
    } while(memcmp(&objpimStaticRPGrpAddressValue, &nextObjpimStaticRPGrpAddressValue, sizeof(L7_inet_addr_t)) && (owa.l7rc == L7_SUCCESS));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStaticRPGrpPrefixLengthValue, owa.len);
    memcpy(&nextObjpimStaticRPGrpAddressValue, &objpimStaticRPGrpAddressValue, sizeof(L7_inet_addr_t));
   /* nextObjpimStaticRPGrpPrefixLengthValue.addr = objpimStaticRPGrpPrefixLengthValue.addr;*/
    maskLen = objpimStaticRPGrpPrefixLengthValue.addr;

    owa.l7rc = usmDbPimsmStaticRPEntryNextGet (keypimStaticRPAddressTypeValue,
                                              &nextObjpimStaticRPGrpAddressValue,
                                              (xLibU8_t *)&maskLen);
   nextObjpimStaticRPGrpPrefixLengthValue.addr =maskLen;
  }

  if (memcmp(&objpimStaticRPGrpAddressValue, &nextObjpimStaticRPGrpAddressValue, sizeof(L7_inet_addr_t))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStaticRPGrpPrefixLengthValue, owa.len);


  nextObjpimStaticRPGrpPrefixLengthValue.family =keypimStaticRPAddressTypeValue;
  /* return the object value: pimStaticRPGrpPrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimStaticRPGrpPrefixLengthValue,
                           sizeof (xLibV4V6Mask_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStaticRPTable_pimStaticRPRPAddress
*
* @purpose Get 'pimStaticRPRPAddress'
*
* @description [pimStaticRPRPAddress] The IP address of the RP to be used for groups within this group prefix. The InetAddressType is given by the pimStaticRPAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStaticRPTable_pimStaticRPRPAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStaticRPGrpAddressValue;

  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimStaticRPGrpPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStaticRPRPAddressValue;
  FPOBJ_TRACE_ENTER (bufp);
  

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) &keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStaticRPAddrGet (keypimStaticRPAddressTypeValue,
                                                                &keypimStaticRPGrpAddressValue,
                                                                keypimStaticRPGrpPrefixLengthValue.addr, &objpimStaticRPRPAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStaticRPRPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimStaticRPRPAddressValue,
                           sizeof (objpimStaticRPRPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastpimStaticRPTable_pimStaticRPRPAddress
*
* @purpose Set 'pimStaticRPRPAddress'
*
* @description [pimStaticRPRPAddress] The IP address of the RP to be used for groups within this group prefix. The InetAddressType is given by the pimStaticRPAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimStaticRPTable_pimStaticRPRPAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStaticRPRPAddressValue;

  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStaticRPGrpAddressValue;

  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimStaticRPGrpPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimStaticRPRPAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &objpimStaticRPRPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objpimStaticRPRPAddressValue, owa.len);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) &keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

/* Commenting this out ...
 * This API actually overwrites the pimStaticRPOverrideDynamic Field.
 * Instead we are setting this in the RowStatus Set.
 */
#if 0
  /* set the value in application */
  owa.l7rc = usmDbPimsmStaticRPAddrSet (keypimStaticRPAddressTypeValue,
                                                              &keypimStaticRPGrpAddressValue,
                                                              keypimStaticRPGrpPrefixLengthValue.addr, &objpimStaticRPRPAddressValue);
#endif

  owa.l7rc = L7_SUCCESS;

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStaticRPTable_pimStaticRPPimMode
*
* @purpose Get 'pimStaticRPPimMode'
*
* @description [pimStaticRPPimMode] The PIM mode to be used for groups in this group prefix.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStaticRPTable_pimStaticRPPimMode (void *wap, void *bufp)
{
  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStaticRPGrpAddressValue;

  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimStaticRPGrpPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPPimModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) &keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStaticRPPimModeGet (keypimStaticRPAddressTypeValue,
                                                                      &keypimStaticRPGrpAddressValue,
                                                                      keypimStaticRPGrpPrefixLengthValue.addr, &objpimStaticRPPimModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStaticRPPimMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStaticRPPimModeValue,
                           sizeof (objpimStaticRPPimModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimStaticRPTable_pimStaticRPPimMode
*
* @purpose Set 'pimStaticRPPimMode'
*
* @description [pimStaticRPPimMode] The PIM mode to be used for groups in this group prefix.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimStaticRPTable_pimStaticRPPimMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPPimModeValue;

  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStaticRPGrpAddressValue;

  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimStaticRPGrpPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimStaticRPPimMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimStaticRPPimModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimStaticRPPimModeValue, owa.len);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) &keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbPimsmStaticRPPimModeGet (keypimStaticRPAddressTypeValue,
                                                                      &keypimStaticRPGrpAddressValue,
                                                                      keypimStaticRPGrpPrefixLengthValue.addr, &objpimStaticRPPimModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStaticRPTable_pimStaticRPOverrideDynamic
*
* @purpose Get 'pimStaticRPOverrideDynamic'
*
* @description [pimStaticRPOverrideDynamic] Whether this static RP configuration will override other group mappings in this group prefix. If this object is TRUE, then it will override: - RP information learned dynamically for groups in this group prefix. - RP information configured in pimStaticRPTable with pimStaticRPOverrideDynamic set to FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStaticRPTable_pimStaticRPOverrideDynamic (void *wap, void *bufp)
{

  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStaticRPGrpAddressValue;

  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimStaticRPGrpPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPOverrideDynamicValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) &keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStaticRPOverrideDynamicGet (keypimStaticRPAddressTypeValue,
                                                                                  &keypimStaticRPGrpAddressValue,
                                                                                  keypimStaticRPGrpPrefixLengthValue.addr,
                                                                                  &objpimStaticRPOverrideDynamicValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStaticRPOverrideDynamic */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStaticRPOverrideDynamicValue,
                           sizeof (objpimStaticRPOverrideDynamicValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastpimStaticRPTable_pimStaticRPOverrideDynamic
*
* @purpose Set 'pimStaticRPOverrideDynamic'
*
* @description [pimStaticRPOverrideDynamic] Whether this static RP configuration will override other group mappings in this group prefix. If this object is TRUE, then it will override: - RP information learned dynamically for groups in this group prefix. - RP information configured in pimStaticRPTable with pimStaticRPOverrideDynamic set to FALSE.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimStaticRPTable_pimStaticRPOverrideDynamic (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPOverrideDynamicValue;

  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStaticRPGrpAddressValue;

  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimStaticRPGrpPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimStaticRPOverrideDynamic */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimStaticRPOverrideDynamicValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimStaticRPOverrideDynamicValue, owa.len);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) &keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

#if 0
  /* set the value in application */
  owa.l7rc = usmDbPimsmStaticRPOverrideDynamicSet (keypimStaticRPAddressTypeValue,
                                                                                  &keypimStaticRPGrpAddressValue,
                                                                                  keypimStaticRPGrpPrefixLengthValue.addr,
                                                                                  objpimStaticRPOverrideDynamicValue);
#endif
  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStaticRPTable_pimStaticRPPrecedence
*
* @purpose Get 'pimStaticRPPrecedence'
*
* @description [pimStaticRPPrecedence] The value for pimGroupMappingPrecedence to be used for this static RP configuration. This allows fine control over which configuration is overridden by this static configuration.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStaticRPTable_pimStaticRPPrecedence (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimStaticRPGrpAddressValue;
  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPGrpPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPPrecedenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimStaticRPAddressTypeValue,
                              keypimStaticRPGrpAddressValue,
                              keypimStaticRPGrpPrefixLengthValue, &objpimStaticRPPrecedenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStaticRPPrecedence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStaticRPPrecedenceValue,
                           sizeof (objpimStaticRPPrecedenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimStaticRPTable_pimStaticRPPrecedence
*
* @purpose Set 'pimStaticRPPrecedence'
*
* @description [pimStaticRPPrecedence] The value for pimGroupMappingPrecedence to be used for this static RP configuration. This allows fine control over which configuration is overridden by this static configuration.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimStaticRPTable_pimStaticRPPrecedence (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPPrecedenceValue;

  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimStaticRPGrpAddressValue;
  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPGrpPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimStaticRPPrecedence */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimStaticRPPrecedenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimStaticRPPrecedenceValue, owa.len);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimStaticRPAddressTypeValue,
                              keypimStaticRPGrpAddressValue,
                              keypimStaticRPGrpPrefixLengthValue, objpimStaticRPPrecedenceValue);

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
* @function fpObjGet_ipmcastpimStaticRPTable_pimStaticRPRowStatus
*
* @purpose Get 'pimStaticRPRowStatus'
*
* @description [pimStaticRPRowStatus] The status of this row, by which rows in this table can be created and destroyed.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStaticRPTable_pimStaticRPRowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStaticRPGrpAddressValue;

  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimStaticRPGrpPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPRowStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) &keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmStaticRPEntryGet (keypimStaticRPAddressTypeValue,
                                                                &keypimStaticRPGrpAddressValue,
                                                                keypimStaticRPGrpPrefixLengthValue.addr);
  if (owa.l7rc == L7_SUCCESS)
  {
    objpimStaticRPRowStatusValue = L7_ROW_STATUS_ACTIVE;
  }
  else
  {
    objpimStaticRPRowStatusValue = L7_ROW_STATUS_NOT_IN_SERVICE;
  }

  /* return the object value: pimStaticRPRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStaticRPRowStatusValue,
                           sizeof (objpimStaticRPRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_ipmcastpimStaticRPTable_pimStaticRPRowStatus
*
* @purpose Set 'pimStaticRPRowStatus'
*
* @description [pimStaticRPRowStatus] The status of this row, by which rows in this table can be created and destroyed.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimStaticRPTable_pimStaticRPRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPRowStatusValue;

  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;\
  
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStaticRPGrpAddressValue;

  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibV4V6Mask_t));
  xLibV4V6Mask_t keypimStaticRPGrpPrefixLengthValue;
  
  fpObjWa_t kwapimStaticRPRPAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStaticRPRPAddressValue;

  fpObjWa_t kwapimStaticRPOverrideDynamic = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPOverrideDynamicValue;

  L7_uchar8 buf[48];
  memset(buf,0,48);

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimStaticRPRowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimStaticRPRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimStaticRPRowStatusValue, owa.len);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) &keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }
  
  

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* retrieve field: pimStaticRPRPAddress */
  kwapimStaticRPRPAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPRPAddress,
           (xLibU8_t *) & keypimStaticRPRPAddressValue, &kwapimStaticRPRPAddress.len);
  if (kwapimStaticRPRPAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPRPAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPRPAddress);
    return kwapimStaticRPRPAddress.rc;
  }

  /* retrieve field: pimStaticRPOverride */
  kwapimStaticRPOverrideDynamic.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPOverrideDynamic,
                   (xLibU8_t *) & keypimStaticRPOverrideDynamicValue, &kwapimStaticRPOverrideDynamic.len);
  if (kwapimStaticRPOverrideDynamic.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPOverrideDynamic.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPOverrideDynamic);
    return kwapimStaticRPOverrideDynamic.rc;
  }

  owa.l7rc = L7_SUCCESS;

  if (objpimStaticRPRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbPimsmStaticRPSet (keypimStaticRPAddressTypeValue,
                                      &keypimStaticRPRPAddressValue,
                                      &keypimStaticRPGrpAddressValue,
                                      keypimStaticRPGrpPrefixLengthValue.addr,
                                      keypimStaticRPOverrideDynamicValue);
  }
  else if (objpimStaticRPRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbPimsmStaticRPRemove (keypimStaticRPAddressTypeValue,
                                         &keypimStaticRPRPAddressValue,
                                         &keypimStaticRPGrpAddressValue,
                                         keypimStaticRPGrpPrefixLengthValue.addr);
  }
  if (owa.l7rc == L7_ALREADY_CONFIGURED)
  {
   owa.rc = XLIBRC_PIM_RP_STATIC_ADDR_DUPLICATE;
  }
  else if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  else
  {
    /* Do Nothing */
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimStaticRPTable_pimStaticRPStorageType
*
* @purpose Get 'pimStaticRPStorageType'
*
* @description [pimStaticRPStorageType] The storage type for this row. Rows having the value 'permanent' need not allow write-access to any columnar objects in the row.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStaticRPTable_pimStaticRPStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimStaticRPGrpAddressValue;
  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPGrpPrefixLengthValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPStorageTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimStaticRPAddressTypeValue,
                              keypimStaticRPGrpAddressValue,
                              keypimStaticRPGrpPrefixLengthValue, &objpimStaticRPStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimStaticRPStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStaticRPStorageTypeValue,
                           sizeof (objpimStaticRPStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimStaticRPTable_pimStaticRPStorageType
*
* @purpose Set 'pimStaticRPStorageType'
*
* @description [pimStaticRPStorageType] The storage type for this row. Rows having the value 'permanent' need not allow write-access to any columnar objects in the row.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimStaticRPTable_pimStaticRPStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStaticRPStorageTypeValue;

  fpObjWa_t kwapimStaticRPAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPAddressTypeValue;
  fpObjWa_t kwapimStaticRPGrpAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimStaticRPGrpAddressValue;
  fpObjWa_t kwapimStaticRPGrpPrefixLength = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStaticRPGrpPrefixLengthValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimStaticRPStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimStaticRPStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimStaticRPStorageTypeValue, owa.len);

  /* retrieve key: pimStaticRPAddressType */
  kwapimStaticRPAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPAddressType,
                   (xLibU8_t *) & keypimStaticRPAddressTypeValue, &kwapimStaticRPAddressType.len);
  if (kwapimStaticRPAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPAddressType);
    return kwapimStaticRPAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPAddressTypeValue, kwapimStaticRPAddressType.len);

  /* retrieve key: pimStaticRPGrpAddress */
  kwapimStaticRPGrpAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpAddress,
                   (xLibU8_t *) keypimStaticRPGrpAddressValue, &kwapimStaticRPGrpAddress.len);
  if (kwapimStaticRPGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpAddress);
    return kwapimStaticRPGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpAddressValue, kwapimStaticRPGrpAddress.len);

  /* retrieve key: pimStaticRPGrpPrefixLength */
  kwapimStaticRPGrpPrefixLength.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimStaticRPTable_pimStaticRPGrpPrefixLength,
                   (xLibU8_t *) & keypimStaticRPGrpPrefixLengthValue,
                   &kwapimStaticRPGrpPrefixLength.len);
  if (kwapimStaticRPGrpPrefixLength.rc != XLIBRC_SUCCESS)
  {
    kwapimStaticRPGrpPrefixLength.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStaticRPGrpPrefixLength);
    return kwapimStaticRPGrpPrefixLength.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStaticRPGrpPrefixLengthValue,
                           kwapimStaticRPGrpPrefixLength.len);

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keypimStaticRPAddressTypeValue,
                              keypimStaticRPGrpAddressValue,
                              keypimStaticRPGrpPrefixLengthValue, objpimStaticRPStorageTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
