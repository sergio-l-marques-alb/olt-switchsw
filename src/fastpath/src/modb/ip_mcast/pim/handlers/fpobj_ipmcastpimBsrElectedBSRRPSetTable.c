
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimBsrElectedBSRRPSetTable.c
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
#include "_xe_ipmcastpimBsrElectedBSRRPSetTable_obj.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType
*
* @purpose Get 'pimBsrElectedBSRGrpMappingAddrType'
 *@description  [pimBsrElectedBSRGrpMappingAddrType] The Inet address type of
* the IP multicast group prefix.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType (void *wap,
                                                                                        void *bufp)
{

#ifdef NOT_IMPLEMENTED
  xLibU32_t objpimBsrElectedBSRGrpMappingAddrTypeValue;
  xLibU32_t nextObjpimBsrElectedBSRGrpMappingAddrTypeValue;
  xLibStr256_t objpimBsrElectedBSRGrpMappingGrpAddrValue;
  xLibU32_t nextObjpimBsrElectedBSRGrpMappingGrpAddrValue;
  xLibU32_t objpimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  xLibU32_t nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  xLibStr256_t objpimBsrElectedBSRGrpMappingRPAddrValue;
  xLibU32_t nextObjpimBsrElectedBSRGrpMappingRPAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRGrpMappingAddrType */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType,
                   (xLibU8_t *) & objpimBsrElectedBSRGrpMappingAddrTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objpimBsrElectedBSRGrpMappingAddrTypeValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingAddrTypeValue));
    memset (objpimBsrElectedBSRGrpMappingGrpAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingGrpAddrValue));
    memset (&objpimBsrElectedBSRGrpMappingGrpPrefixLenValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingGrpPrefixLenValue));
    memset (objpimBsrElectedBSRGrpMappingRPAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingRPAddrValue));
    owa.l7rc =
      usmDbGetFirstUnknown (L7_UNIT_CURRENT, objpimBsrElectedBSRGrpMappingAddrTypeValue,
                            objpimBsrElectedBSRGrpMappingGrpAddrValue,
                            objpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                            objpimBsrElectedBSRGrpMappingRPAddrValue,
                            &nextObjpimBsrElectedBSRGrpMappingAddrTypeValue,
                            nextObjpimBsrElectedBSRGrpMappingGrpAddrValue,
                            &nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                            nextObjpimBsrElectedBSRGrpMappingRPAddrValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrElectedBSRGrpMappingAddrTypeValue, owa.len);
    memset (objpimBsrElectedBSRGrpMappingGrpAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingGrpAddrValue));
    memset (&objpimBsrElectedBSRGrpMappingGrpPrefixLenValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingGrpPrefixLenValue));
    memset (objpimBsrElectedBSRGrpMappingRPAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingRPAddrValue));
    do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objpimBsrElectedBSRGrpMappingAddrTypeValue,
                                      objpimBsrElectedBSRGrpMappingGrpAddrValue,
                                      objpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                                      objpimBsrElectedBSRGrpMappingRPAddrValue,
                                      &nextObjpimBsrElectedBSRGrpMappingAddrTypeValue,
                                      nextObjpimBsrElectedBSRGrpMappingGrpAddrValue,
                                      &nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                                      nextObjpimBsrElectedBSRGrpMappingRPAddrValue);
    }
    while ((objpimBsrElectedBSRGrpMappingAddrTypeValue ==
            nextObjpimBsrElectedBSRGrpMappingAddrTypeValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBsrElectedBSRGrpMappingAddrTypeValue, owa.len);

  /* return the object value: pimBsrElectedBSRGrpMappingAddrType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimBsrElectedBSRGrpMappingAddrTypeValue,
                           sizeof (objpimBsrElectedBSRGrpMappingAddrTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpAddr
*
* @purpose Get 'pimBsrElectedBSRGrpMappingGrpAddr'
 *@description  [pimBsrElectedBSRGrpMappingGrpAddr] The IP multicast group
* address which, when combined with
* pimBsrElectedBSRGrpMappingGrpPrefixLen, gives the group prefix for this mapping. The InetAddressType
* is given by the pimBsrElectedBSRGrpMappingAddrType object. This
* address object is only significant up to
* pimBsrElectedBSRGrpMappingGrpPrefixLen bits. The remainder of the address bits are zero.
* This is especially important for this field, which is part of the
* index of this entry. Any non-zero bits would signify an entirely
* different entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpAddr (void *wap,
                                                                                       void *bufp)
{

#ifdef NOT_IMPLEMENTED
  xLibU32_t objpimBsrElectedBSRGrpMappingAddrTypeValue;
  xLibStr256_t nextObjpimBsrElectedBSRGrpMappingAddrTypeValue;
  xLibStr256_t objpimBsrElectedBSRGrpMappingGrpAddrValue;
  xLibStr256_t nextObjpimBsrElectedBSRGrpMappingGrpAddrValue;
  xLibU32_t objpimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  xLibStr256_t nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  xLibStr256_t objpimBsrElectedBSRGrpMappingRPAddrValue;
  xLibStr256_t nextObjpimBsrElectedBSRGrpMappingRPAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRGrpMappingAddrType */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType,
                   (xLibU8_t *) & objpimBsrElectedBSRGrpMappingAddrTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrElectedBSRGrpMappingAddrTypeValue, owa.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpAddr */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpAddr,
                   (xLibU8_t *) objpimBsrElectedBSRGrpMappingGrpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objpimBsrElectedBSRGrpMappingGrpAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingGrpAddrValue));
    memset (&objpimBsrElectedBSRGrpMappingGrpPrefixLenValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingGrpPrefixLenValue));
    memset (objpimBsrElectedBSRGrpMappingRPAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingRPAddrValue));
    owa.l7rc =
      usmDbGetFirstUnknown (L7_UNIT_CURRENT, objpimBsrElectedBSRGrpMappingAddrTypeValue,
                            objpimBsrElectedBSRGrpMappingGrpAddrValue,
                            objpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                            objpimBsrElectedBSRGrpMappingRPAddrValue,
                            &nextObjpimBsrElectedBSRGrpMappingAddrTypeValue,
                            nextObjpimBsrElectedBSRGrpMappingGrpAddrValue,
                            &nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                            nextObjpimBsrElectedBSRGrpMappingRPAddrValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpimBsrElectedBSRGrpMappingGrpAddrValue, owa.len);
    memset (&objpimBsrElectedBSRGrpMappingGrpPrefixLenValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingGrpPrefixLenValue));
    memset (objpimBsrElectedBSRGrpMappingRPAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingRPAddrValue));
    do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objpimBsrElectedBSRGrpMappingAddrTypeValue,
                                      objpimBsrElectedBSRGrpMappingGrpAddrValue,
                                      objpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                                      objpimBsrElectedBSRGrpMappingRPAddrValue,
                                      &nextObjpimBsrElectedBSRGrpMappingAddrTypeValue,
                                      nextObjpimBsrElectedBSRGrpMappingGrpAddrValue,
                                      &nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                                      nextObjpimBsrElectedBSRGrpMappingRPAddrValue);
    }
    while ((objpimBsrElectedBSRGrpMappingAddrTypeValue ==
            nextObjpimBsrElectedBSRGrpMappingAddrTypeValue)
           && (objpimBsrElectedBSRGrpMappingGrpAddrValue ==
               nextObjpimBsrElectedBSRGrpMappingGrpAddrValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objpimBsrElectedBSRGrpMappingAddrTypeValue != nextObjpimBsrElectedBSRGrpMappingAddrTypeValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpimBsrElectedBSRGrpMappingGrpAddrValue, owa.len);

  /* return the object value: pimBsrElectedBSRGrpMappingGrpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpimBsrElectedBSRGrpMappingGrpAddrValue,
                           strlen (objpimBsrElectedBSRGrpMappingGrpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpPrefixLen
*
* @purpose Get 'pimBsrElectedBSRGrpMappingGrpPrefixLen'
 *@description  [pimBsrElectedBSRGrpMappingGrpPrefixLen] The multicast group
* prefix length that, when combined with
* pimBsrElectedBSRGrpMappingGrpAddr, gives the group prefix for this mapping. The
* InetAddressType is given by the pimBsrElectedBSRGrpMappingAddrType object. If
* pimBsrElectedBSRGrpMappingAddrType is 'ipv4' or 'ipv4z', this
* object must be in the range 4..32. If
* pimBsrElectedBSRGrpMappingAddrType is 'ipv6' or 'ipv6z', this object must be in the range 8..128.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpPrefixLen (void
                                                                                            *wap,
                                                                                            void
                                                                                            *bufp)
{

#ifdef NOT_IMPLEMENTED
  xLibU32_t objpimBsrElectedBSRGrpMappingAddrTypeValue;
  xLibU32_t nextObjpimBsrElectedBSRGrpMappingAddrTypeValue;
  xLibStr256_t objpimBsrElectedBSRGrpMappingGrpAddrValue;
  xLibU32_t nextObjpimBsrElectedBSRGrpMappingGrpAddrValue;
  xLibU32_t objpimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  xLibU32_t nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  xLibStr256_t objpimBsrElectedBSRGrpMappingRPAddrValue;
  xLibU32_t nextObjpimBsrElectedBSRGrpMappingRPAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRGrpMappingAddrType */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType,
                   (xLibU8_t *) & objpimBsrElectedBSRGrpMappingAddrTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrElectedBSRGrpMappingAddrTypeValue, owa.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpAddr */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpAddr,
                   (xLibU8_t *) objpimBsrElectedBSRGrpMappingGrpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objpimBsrElectedBSRGrpMappingGrpAddrValue, owa.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpPrefixLen */
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpPrefixLen,
                   (xLibU8_t *) & objpimBsrElectedBSRGrpMappingGrpPrefixLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objpimBsrElectedBSRGrpMappingGrpPrefixLenValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingGrpPrefixLenValue));
    memset (objpimBsrElectedBSRGrpMappingRPAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingRPAddrValue));
    owa.l7rc =
      usmDbGetFirstUnknown (L7_UNIT_CURRENT, objpimBsrElectedBSRGrpMappingAddrTypeValue,
                            objpimBsrElectedBSRGrpMappingGrpAddrValue,
                            objpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                            objpimBsrElectedBSRGrpMappingRPAddrValue,
                            &nextObjpimBsrElectedBSRGrpMappingAddrTypeValue,
                            nextObjpimBsrElectedBSRGrpMappingGrpAddrValue,
                            &nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                            nextObjpimBsrElectedBSRGrpMappingRPAddrValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrElectedBSRGrpMappingGrpPrefixLenValue, owa.len);
    memset (objpimBsrElectedBSRGrpMappingRPAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingRPAddrValue));
    do
    {
      owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                      objpimBsrElectedBSRGrpMappingAddrTypeValue,
                                      objpimBsrElectedBSRGrpMappingGrpAddrValue,
                                      objpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                                      objpimBsrElectedBSRGrpMappingRPAddrValue,
                                      &nextObjpimBsrElectedBSRGrpMappingAddrTypeValue,
                                      nextObjpimBsrElectedBSRGrpMappingGrpAddrValue,
                                      &nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                                      nextObjpimBsrElectedBSRGrpMappingRPAddrValue);
    }
    while ((objpimBsrElectedBSRGrpMappingAddrTypeValue ==
            nextObjpimBsrElectedBSRGrpMappingAddrTypeValue)
           && (objpimBsrElectedBSRGrpMappingGrpAddrValue ==
               nextObjpimBsrElectedBSRGrpMappingGrpAddrValue)
           && (objpimBsrElectedBSRGrpMappingGrpPrefixLenValue ==
               nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((objpimBsrElectedBSRGrpMappingAddrTypeValue != nextObjpimBsrElectedBSRGrpMappingAddrTypeValue)
      || (objpimBsrElectedBSRGrpMappingGrpAddrValue !=
          nextObjpimBsrElectedBSRGrpMappingGrpAddrValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue, owa.len);

  /* return the object value: pimBsrElectedBSRGrpMappingGrpPrefixLen */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                           sizeof (objpimBsrElectedBSRGrpMappingGrpPrefixLenValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingRPAddr
*
* @purpose Get 'pimBsrElectedBSRGrpMappingRPAddr'
 *@description  [pimBsrElectedBSRGrpMappingRPAddr] The IP address of the RP to
* be used for groups within this group prefix. The InetAddressType
* is given by the pimBsrElectedBSRGrpMappingAddrType object.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingRPAddr (void *wap,
                                                                                      void *bufp)
{

#ifdef NOT_IMPLEMENTED
  xLibU32_t objpimBsrElectedBSRGrpMappingAddrTypeValue;
  xLibStr256_t nextObjpimBsrElectedBSRGrpMappingAddrTypeValue;
  xLibStr256_t objpimBsrElectedBSRGrpMappingGrpAddrValue;
  xLibStr256_t nextObjpimBsrElectedBSRGrpMappingGrpAddrValue;
  xLibU32_t objpimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  xLibStr256_t nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  xLibStr256_t objpimBsrElectedBSRGrpMappingRPAddrValue;
  xLibStr256_t nextObjpimBsrElectedBSRGrpMappingRPAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRGrpMappingAddrType */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType,
                   (xLibU8_t *) & objpimBsrElectedBSRGrpMappingAddrTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrElectedBSRGrpMappingAddrTypeValue, owa.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpAddr */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpAddr,
                   (xLibU8_t *) objpimBsrElectedBSRGrpMappingGrpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objpimBsrElectedBSRGrpMappingGrpAddrValue, owa.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpPrefixLen */
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpPrefixLen,
                   (xLibU8_t *) & objpimBsrElectedBSRGrpMappingGrpPrefixLenValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimBsrElectedBSRGrpMappingGrpPrefixLenValue, owa.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingRPAddr */
  owa.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingRPAddr,
                   (xLibU8_t *) objpimBsrElectedBSRGrpMappingRPAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objpimBsrElectedBSRGrpMappingRPAddrValue, 0,
            sizeof (objpimBsrElectedBSRGrpMappingRPAddrValue));
    owa.l7rc =
      usmDbGetFirstUnknown (L7_UNIT_CURRENT, objpimBsrElectedBSRGrpMappingAddrTypeValue,
                            objpimBsrElectedBSRGrpMappingGrpAddrValue,
                            objpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                            objpimBsrElectedBSRGrpMappingRPAddrValue,
                            &nextObjpimBsrElectedBSRGrpMappingAddrTypeValue,
                            nextObjpimBsrElectedBSRGrpMappingGrpAddrValue,
                            &nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                            nextObjpimBsrElectedBSRGrpMappingRPAddrValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpimBsrElectedBSRGrpMappingRPAddrValue, owa.len);

    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT,
                                    objpimBsrElectedBSRGrpMappingAddrTypeValue,
                                    objpimBsrElectedBSRGrpMappingGrpAddrValue,
                                    objpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                                    objpimBsrElectedBSRGrpMappingRPAddrValue,
                                    &nextObjpimBsrElectedBSRGrpMappingAddrTypeValue,
                                    nextObjpimBsrElectedBSRGrpMappingGrpAddrValue,
                                    &nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                                    nextObjpimBsrElectedBSRGrpMappingRPAddrValue);

  }

  if ((objpimBsrElectedBSRGrpMappingAddrTypeValue != nextObjpimBsrElectedBSRGrpMappingAddrTypeValue)
      || (objpimBsrElectedBSRGrpMappingGrpAddrValue !=
          nextObjpimBsrElectedBSRGrpMappingGrpAddrValue)
      || (objpimBsrElectedBSRGrpMappingGrpPrefixLenValue !=
          nextObjpimBsrElectedBSRGrpMappingGrpPrefixLenValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpimBsrElectedBSRGrpMappingRPAddrValue, owa.len);

  /* return the object value: pimBsrElectedBSRGrpMappingRPAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpimBsrElectedBSRGrpMappingRPAddrValue,
                           strlen (objpimBsrElectedBSRGrpMappingRPAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRRPSetPriority
*
* @purpose Get 'pimBsrElectedBSRRPSetPriority'
 *@description  [pimBsrElectedBSRRPSetPriority] The priority for RP. Numerically
* higher values for this object indicate lower priorities, with
* the value zero denoting the highest priority.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRRPSetPriority (void *wap,
                                                                                   void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrElectedBSRGrpMappingAddrType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRGrpMappingAddrTypeValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingGrpAddr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrElectedBSRGrpMappingGrpAddrValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingGrpPrefixLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingRPAddr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrElectedBSRGrpMappingRPAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrElectedBSRRPSetPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRGrpMappingAddrType */
  kwapimBsrElectedBSRGrpMappingAddrType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType,
                   (xLibU8_t *) & keypimBsrElectedBSRGrpMappingAddrTypeValue,
                   &kwapimBsrElectedBSRGrpMappingAddrType.len);
  if (kwapimBsrElectedBSRGrpMappingAddrType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingAddrType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingAddrType);
    return kwapimBsrElectedBSRGrpMappingAddrType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRGrpMappingAddrTypeValue,
                           kwapimBsrElectedBSRGrpMappingAddrType.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpAddr */
  kwapimBsrElectedBSRGrpMappingGrpAddr.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpAddr,
                   (xLibU8_t *) keypimBsrElectedBSRGrpMappingGrpAddrValue,
                   &kwapimBsrElectedBSRGrpMappingGrpAddr.len);
  if (kwapimBsrElectedBSRGrpMappingGrpAddr.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingGrpAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingGrpAddr);
    return kwapimBsrElectedBSRGrpMappingGrpAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrElectedBSRGrpMappingGrpAddrValue,
                           kwapimBsrElectedBSRGrpMappingGrpAddr.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpPrefixLen */
  kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc =
    xLibFilterGet (wap,
                   XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpPrefixLen,
                   (xLibU8_t *) & keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                   &kwapimBsrElectedBSRGrpMappingGrpPrefixLen.len);
  if (kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingGrpPrefixLen);
    return kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                           kwapimBsrElectedBSRGrpMappingGrpPrefixLen.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingRPAddr */
  kwapimBsrElectedBSRGrpMappingRPAddr.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingRPAddr,
                   (xLibU8_t *) keypimBsrElectedBSRGrpMappingRPAddrValue,
                   &kwapimBsrElectedBSRGrpMappingRPAddr.len);
  if (kwapimBsrElectedBSRGrpMappingRPAddr.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingRPAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingRPAddr);
    return kwapimBsrElectedBSRGrpMappingRPAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrElectedBSRGrpMappingRPAddrValue,
                           kwapimBsrElectedBSRGrpMappingRPAddr.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrElectedBSRGrpMappingAddrTypeValue,
                              keypimBsrElectedBSRGrpMappingGrpAddrValue,
                              keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                              keypimBsrElectedBSRGrpMappingRPAddrValue,
                              &objpimBsrElectedBSRRPSetPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrElectedBSRRPSetPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrElectedBSRRPSetPriorityValue,
                           sizeof (objpimBsrElectedBSRRPSetPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRRPSetHoldtime
*
* @purpose Get 'pimBsrElectedBSRRPSetHoldtime'
 *@description  [pimBsrElectedBSRRPSetHoldtime] The holdtime for RP   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRRPSetHoldtime (void *wap,
                                                                                   void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrElectedBSRGrpMappingAddrType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRGrpMappingAddrTypeValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingGrpAddr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrElectedBSRGrpMappingGrpAddrValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingGrpPrefixLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingRPAddr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrElectedBSRGrpMappingRPAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrElectedBSRRPSetHoldtimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRGrpMappingAddrType */
  kwapimBsrElectedBSRGrpMappingAddrType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType,
                   (xLibU8_t *) & keypimBsrElectedBSRGrpMappingAddrTypeValue,
                   &kwapimBsrElectedBSRGrpMappingAddrType.len);
  if (kwapimBsrElectedBSRGrpMappingAddrType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingAddrType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingAddrType);
    return kwapimBsrElectedBSRGrpMappingAddrType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRGrpMappingAddrTypeValue,
                           kwapimBsrElectedBSRGrpMappingAddrType.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpAddr */
  kwapimBsrElectedBSRGrpMappingGrpAddr.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpAddr,
                   (xLibU8_t *) keypimBsrElectedBSRGrpMappingGrpAddrValue,
                   &kwapimBsrElectedBSRGrpMappingGrpAddr.len);
  if (kwapimBsrElectedBSRGrpMappingGrpAddr.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingGrpAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingGrpAddr);
    return kwapimBsrElectedBSRGrpMappingGrpAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrElectedBSRGrpMappingGrpAddrValue,
                           kwapimBsrElectedBSRGrpMappingGrpAddr.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpPrefixLen */
  kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc =
    xLibFilterGet (wap,
                   XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpPrefixLen,
                   (xLibU8_t *) & keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                   &kwapimBsrElectedBSRGrpMappingGrpPrefixLen.len);
  if (kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingGrpPrefixLen);
    return kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                           kwapimBsrElectedBSRGrpMappingGrpPrefixLen.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingRPAddr */
  kwapimBsrElectedBSRGrpMappingRPAddr.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingRPAddr,
                   (xLibU8_t *) keypimBsrElectedBSRGrpMappingRPAddrValue,
                   &kwapimBsrElectedBSRGrpMappingRPAddr.len);
  if (kwapimBsrElectedBSRGrpMappingRPAddr.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingRPAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingRPAddr);
    return kwapimBsrElectedBSRGrpMappingRPAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrElectedBSRGrpMappingRPAddrValue,
                           kwapimBsrElectedBSRGrpMappingRPAddr.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrElectedBSRGrpMappingAddrTypeValue,
                              keypimBsrElectedBSRGrpMappingGrpAddrValue,
                              keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                              keypimBsrElectedBSRGrpMappingRPAddrValue,
                              &objpimBsrElectedBSRRPSetHoldtimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrElectedBSRRPSetHoldtime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrElectedBSRRPSetHoldtimeValue,
                           sizeof (objpimBsrElectedBSRRPSetHoldtimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRRPSetExpiryTime
*
* @purpose Get 'pimBsrElectedBSRRPSetExpiryTime'
 *@description  [pimBsrElectedBSRRPSetExpiryTime] The minimum time remaining
* before this entry will be aged out. The value zero indicates that
* this entry will never be aged out.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRRPSetExpiryTime (void *wap,
                                                                                     void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrElectedBSRGrpMappingAddrType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRGrpMappingAddrTypeValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingGrpAddr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrElectedBSRGrpMappingGrpAddrValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingGrpPrefixLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingRPAddr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrElectedBSRGrpMappingRPAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrElectedBSRRPSetExpiryTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRGrpMappingAddrType */
  kwapimBsrElectedBSRGrpMappingAddrType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType,
                   (xLibU8_t *) & keypimBsrElectedBSRGrpMappingAddrTypeValue,
                   &kwapimBsrElectedBSRGrpMappingAddrType.len);
  if (kwapimBsrElectedBSRGrpMappingAddrType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingAddrType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingAddrType);
    return kwapimBsrElectedBSRGrpMappingAddrType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRGrpMappingAddrTypeValue,
                           kwapimBsrElectedBSRGrpMappingAddrType.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpAddr */
  kwapimBsrElectedBSRGrpMappingGrpAddr.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpAddr,
                   (xLibU8_t *) keypimBsrElectedBSRGrpMappingGrpAddrValue,
                   &kwapimBsrElectedBSRGrpMappingGrpAddr.len);
  if (kwapimBsrElectedBSRGrpMappingGrpAddr.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingGrpAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingGrpAddr);
    return kwapimBsrElectedBSRGrpMappingGrpAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrElectedBSRGrpMappingGrpAddrValue,
                           kwapimBsrElectedBSRGrpMappingGrpAddr.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpPrefixLen */
  kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc =
    xLibFilterGet (wap,
                   XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpPrefixLen,
                   (xLibU8_t *) & keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                   &kwapimBsrElectedBSRGrpMappingGrpPrefixLen.len);
  if (kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingGrpPrefixLen);
    return kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                           kwapimBsrElectedBSRGrpMappingGrpPrefixLen.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingRPAddr */
  kwapimBsrElectedBSRGrpMappingRPAddr.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingRPAddr,
                   (xLibU8_t *) keypimBsrElectedBSRGrpMappingRPAddrValue,
                   &kwapimBsrElectedBSRGrpMappingRPAddr.len);
  if (kwapimBsrElectedBSRGrpMappingRPAddr.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingRPAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingRPAddr);
    return kwapimBsrElectedBSRGrpMappingRPAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrElectedBSRGrpMappingRPAddrValue,
                           kwapimBsrElectedBSRGrpMappingRPAddr.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrElectedBSRGrpMappingAddrTypeValue,
                              keypimBsrElectedBSRGrpMappingGrpAddrValue,
                              keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                              keypimBsrElectedBSRGrpMappingRPAddrValue,
                              &objpimBsrElectedBSRRPSetExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrElectedBSRRPSetExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrElectedBSRRPSetExpiryTimeValue,
                           sizeof (objpimBsrElectedBSRRPSetExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRRPSetGrpBidir
*
* @purpose Get 'pimBsrElectedBSRRPSetGrpBidir'
 *@description  [pimBsrElectedBSRRPSetGrpBidir] If this object is TRUE, this
* group range with this RP is a BIDIR-PIM group range. If it is set to
* FALSE, it is a PIM-SM group range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRRPSetGrpBidir (void *wap,
                                                                                   void *bufp)
{

#ifdef NOT_IMPLEMENTED
  fpObjWa_t kwapimBsrElectedBSRGrpMappingAddrType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRGrpMappingAddrTypeValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingGrpAddr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrElectedBSRGrpMappingGrpAddrValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingGrpPrefixLen = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimBsrElectedBSRGrpMappingGrpPrefixLenValue;
  fpObjWa_t kwapimBsrElectedBSRGrpMappingRPAddr = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypimBsrElectedBSRGrpMappingRPAddrValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimBsrElectedBSRRPSetGrpBidirValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimBsrElectedBSRGrpMappingAddrType */
  kwapimBsrElectedBSRGrpMappingAddrType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingAddrType,
                   (xLibU8_t *) & keypimBsrElectedBSRGrpMappingAddrTypeValue,
                   &kwapimBsrElectedBSRGrpMappingAddrType.len);
  if (kwapimBsrElectedBSRGrpMappingAddrType.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingAddrType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingAddrType);
    return kwapimBsrElectedBSRGrpMappingAddrType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRGrpMappingAddrTypeValue,
                           kwapimBsrElectedBSRGrpMappingAddrType.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpAddr */
  kwapimBsrElectedBSRGrpMappingGrpAddr.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpAddr,
                   (xLibU8_t *) keypimBsrElectedBSRGrpMappingGrpAddrValue,
                   &kwapimBsrElectedBSRGrpMappingGrpAddr.len);
  if (kwapimBsrElectedBSRGrpMappingGrpAddr.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingGrpAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingGrpAddr);
    return kwapimBsrElectedBSRGrpMappingGrpAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrElectedBSRGrpMappingGrpAddrValue,
                           kwapimBsrElectedBSRGrpMappingGrpAddr.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingGrpPrefixLen */
  kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc =
    xLibFilterGet (wap,
                   XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingGrpPrefixLen,
                   (xLibU8_t *) & keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                   &kwapimBsrElectedBSRGrpMappingGrpPrefixLen.len);
  if (kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingGrpPrefixLen);
    return kwapimBsrElectedBSRGrpMappingGrpPrefixLen.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                           kwapimBsrElectedBSRGrpMappingGrpPrefixLen.len);

  /* retrieve key: pimBsrElectedBSRGrpMappingRPAddr */
  kwapimBsrElectedBSRGrpMappingRPAddr.rc =
    xLibFilterGet (wap, XOBJ_ipmcastpimBsrElectedBSRRPSetTable_pimBsrElectedBSRGrpMappingRPAddr,
                   (xLibU8_t *) keypimBsrElectedBSRGrpMappingRPAddrValue,
                   &kwapimBsrElectedBSRGrpMappingRPAddr.len);
  if (kwapimBsrElectedBSRGrpMappingRPAddr.rc != XLIBRC_SUCCESS)
  {
    kwapimBsrElectedBSRGrpMappingRPAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimBsrElectedBSRGrpMappingRPAddr);
    return kwapimBsrElectedBSRGrpMappingRPAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keypimBsrElectedBSRGrpMappingRPAddrValue,
                           kwapimBsrElectedBSRGrpMappingRPAddr.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keypimBsrElectedBSRGrpMappingAddrTypeValue,
                              keypimBsrElectedBSRGrpMappingGrpAddrValue,
                              keypimBsrElectedBSRGrpMappingGrpPrefixLenValue,
                              keypimBsrElectedBSRGrpMappingRPAddrValue,
                              &objpimBsrElectedBSRRPSetGrpBidirValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimBsrElectedBSRRPSetGrpBidir */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimBsrElectedBSRRPSetGrpBidirValue,
                           sizeof (objpimBsrElectedBSRRPSetGrpBidirValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
