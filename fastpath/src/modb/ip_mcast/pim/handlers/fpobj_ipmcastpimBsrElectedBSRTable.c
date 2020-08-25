
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimBsrElectedBSRTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  23 May 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimBsrElectedBSRTable_obj.h"
#include "usmdb_1213_api.h"
#include "usmdb_mib_pimsm_api.h"
#include "l7utils_inet_addr_api.h"
#include "usmdb_pimsm_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRZoneIndex
*
* @purpose Get 'pimBsrElectedBSRZoneIndex'
 *@description  [pimBsrElectedBSRZoneIndex] The zone index uniquely identifies
* the zone on a device to which this Elected BSR is attached. There
* is one entry for each zone in ipMcastZoneTable. Scope-level
* information for this zone can be extracted from ipMcastZoneTable in IP
* MCAST MIB. Zero is a special value used to request the default
* zone for a given scope. Zero is not a valid value for this object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRZoneIndex (void *wap, void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrElectedBSRZoneIndexValue;
  xLibU32_t nextObjpimBsrElectedBSRZoneIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRZoneIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRZoneIndex,
                          (xLibU8_t *) & objpimBsrElectedBSRZoneIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjpimBsrElectedBSRZoneIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrElectedBSRZoneIndexValue, owa.len);
    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT, objpimBsrElectedBSRZoneIndexValue,
                                    &nextObjpimBsrElectedBSRZoneIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBsrElectedBSRZoneIndexValue, owa.len);

  /* return the object value: pimBsrElectedBSRZoneIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimBsrElectedBSRZoneIndexValue,
                           sizeof (objpimBsrElectedBSRZoneIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddressType
*
* @purpose Get 'pimBsrElectedBSRAddressType'
 *@description  [pimBsrElectedBSRAddressType] The address type of the elected
* BSR.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddressType (void *wap, void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrElectedBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrElectedBSRAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRZoneIndex */
  kwapimBsrElectedBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrElectedBSRZoneIndexValue,
                   &kwapimBsrElectedBSRZoneIndex.len);
  if (kwapimBsrElectedBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRZoneIndex);
    return kwapimBsrElectedBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRZoneIndexValue,
                           kwapimBsrElectedBSRZoneIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrElectedBSRZoneIndexValue,
                              &objpimBsrElectedBSRAddressTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrElectedBSRAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrElectedBSRAddressTypeValue,
                           sizeof (objpimBsrElectedBSRAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddressType
*
* @purpose Set 'pimBsrElectedBSRAddressType'
* @description  [pimBsrElectedBSRAddressType] 
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddressType(void *wap, void *bufp)
{
  /* The address type need to be overrriden in the web xml based on IPV4 or IPV6 PIM being accessed.*/

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddress
*
* @purpose Get 'pimBsrElectedBSRAddress'
 *@description  [pimBsrElectedBSRAddress] The (unicast) address of the elected
* BSR. The InetAddressType is given by the
* pimBsrElectedBSRAddressType object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimBsrElectedBSRAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  xLibU32_t objpimBsrElectedBSRAddressType;


      /* retrieve address type: pimBsrElectedBSRAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddressType,
                          (xLibU8_t *) & objpimBsrElectedBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  inetAddressZeroSet(objpimBsrElectedBSRAddressType, &objpimBsrElectedBSRAddressValue);

  /* get the value from application */
  owa.l7rc = usmDbPimsmElectedBSRAddressGet (L7_UNIT_CURRENT,objpimBsrElectedBSRAddressType,
                             &objpimBsrElectedBSRAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (inetAddrHtop(&objpimBsrElectedBSRAddressValue, buf) == L7_SUCCESS)
  {
    /* return the object value: pimBsrElectedBSRAddress */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objpimBsrElectedBSRAddressValue,
                           sizeof (objpimBsrElectedBSRAddressValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRPriority
*
* @purpose Get 'pimBsrElectedBSRPriority'
 *@description  [pimBsrElectedBSRPriority] The priority value for the elected
* BSR for this address type. Numerically higher values for this
* object indicate higher priorities.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRPriority (void *wap, void *bufp)
{

  fpObjWa_t kwapimBsrElectedBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrElectedBSRPriorityValue;
  xLibU32_t objpimBsrElectedBSRAddressType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRZoneIndex */
  kwapimBsrElectedBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrElectedBSRZoneIndexValue,
                   &kwapimBsrElectedBSRZoneIndex.len);
  if (kwapimBsrElectedBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRZoneIndex);
    return kwapimBsrElectedBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRZoneIndexValue,
                           kwapimBsrElectedBSRZoneIndex.len);

      /* retrieve address type: pimBsrElectedBSRAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddressType,
                          (xLibU8_t *) & objpimBsrElectedBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbPimsmElectedBSRPriorityGet (L7_UNIT_CURRENT,keypimBsrElectedBSRZoneIndexValue,
                              &objpimBsrElectedBSRPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrElectedBSRPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrElectedBSRPriorityValue,
                           sizeof (objpimBsrElectedBSRPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRHashMaskLength
*
* @purpose Get 'pimBsrElectedBSRHashMaskLength'
 *@description  [pimBsrElectedBSRHashMaskLength] The hash mask length (used in
* the RP hash function) advertised by the elected BSR for this zone.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRHashMaskLength (void *wap,
                                                                               void *bufp)
{

  fpObjWa_t kwapimBsrElectedBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrElectedBSRHashMaskLengthValue;
  xLibU32_t objpimBsrElectedBSRAddressType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRZoneIndex */
  kwapimBsrElectedBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrElectedBSRZoneIndexValue,
                   &kwapimBsrElectedBSRZoneIndex.len);
  if (kwapimBsrElectedBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRZoneIndex);
    return kwapimBsrElectedBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRZoneIndexValue,
                           kwapimBsrElectedBSRZoneIndex.len);

      /* retrieve address type: pimBsrElectedBSRAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddressType,
                          (xLibU8_t *) & objpimBsrElectedBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbPimsmElectedBSRHashMaskLengthGet (L7_UNIT_CURRENT,keypimBsrElectedBSRZoneIndexValue,
                              &objpimBsrElectedBSRHashMaskLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrElectedBSRHashMaskLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrElectedBSRHashMaskLengthValue,
                           sizeof (objpimBsrElectedBSRHashMaskLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRExpiryTime
*
* @purpose Get 'pimBsrElectedBSRExpiryTime'
 *@description  [pimBsrElectedBSRExpiryTime] The minimum time remaining before
* the elected BSR for this zone will be declared down.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRExpiryTime (void *wap, void *bufp)
{

  fpObjWa_t kwapimBsrElectedBSRZoneIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRZoneIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (usmDbTimeSpec_t));
  xLibU32_t objpimBsrElectedBSRExpiryTimeValue;
  xLibU32_t objpimBsrElectedBSRAddressType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRZoneIndex */
  kwapimBsrElectedBSRZoneIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRZoneIndex,
                   (xLibU8_t *) & keypimBsrElectedBSRZoneIndexValue,
                   &kwapimBsrElectedBSRZoneIndex.len);
  if (kwapimBsrElectedBSRZoneIndex.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRZoneIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRZoneIndex);
    return kwapimBsrElectedBSRZoneIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRZoneIndexValue,
                           kwapimBsrElectedBSRZoneIndex.len);

      /* retrieve address type: pimBsrElectedBSRAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRTable_pimBsrElectedBSRAddressType,
                          (xLibU8_t *) & objpimBsrElectedBSRAddressType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbPimsmElectedBSRExpiryTimeGet (L7_UNIT_CURRENT,keypimBsrElectedBSRZoneIndexValue,
                              &objpimBsrElectedBSRExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrElectedBSRExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrElectedBSRExpiryTimeValue,
                           sizeof (objpimBsrElectedBSRExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
