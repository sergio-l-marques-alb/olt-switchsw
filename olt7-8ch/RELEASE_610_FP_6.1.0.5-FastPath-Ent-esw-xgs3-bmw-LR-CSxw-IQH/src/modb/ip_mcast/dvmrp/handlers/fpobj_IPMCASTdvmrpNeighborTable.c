
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTdvmrpNeighborTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to IPMCAST-object.xml
*
* @create  01 May 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_IPMCASTdvmrpNeighborTable_obj.h"
#include "usmdb_mib_dvmrp_api.h"
#include "l3_mcast_commdefs.h"

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex
*
* @purpose Get 'dvmrpNeighborIfIndex'
*
* @description [dvmrpNeighborIfIndex] The value of ifIndex for the virtual interface used to reach this DVMRP neighbor.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex (void *wap, void *bufp)
{

  xLibU32_t objdvmrpNeighborIfIndexValue;
  xLibU32_t nextObjdvmrpNeighborIfIndexValue;
  L7_inet_addr_t nextObjdvmrpNeighborAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                          (xLibU8_t *) & objdvmrpNeighborIfIndexValue, &owa.len);
  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpNeighborAddressValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjdvmrpNeighborIfIndexValue = 0;

    owa.l7rc = usmDbDvmrpNeighborEntryNextGet (L7_UNIT_CURRENT,
                                             &nextObjdvmrpNeighborIfIndexValue,
                                             &nextObjdvmrpNeighborAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpNeighborIfIndexValue, owa.len);
    nextObjdvmrpNeighborIfIndexValue = objdvmrpNeighborIfIndexValue;
    
    do
    {
      owa.l7rc = usmDbDvmrpNeighborEntryNextGet (L7_UNIT_CURRENT,
                                                 &nextObjdvmrpNeighborIfIndexValue,
                                                 &nextObjdvmrpNeighborAddressValue);
    }
    while ((objdvmrpNeighborIfIndexValue == nextObjdvmrpNeighborIfIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpNeighborIfIndexValue, owa.len);

  /* return the object value: dvmrpNeighborIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpNeighborIfIndexValue,
                           sizeof (objdvmrpNeighborIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress
*
* @purpose Get 'dvmrpNeighborAddress'
*
* @description [dvmrpNeighborAddress] The IP address of the DVMRP neighbor for which this entry contains information.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress (void *wap, void *bufp)
{

  xLibU32_t objdvmrpNeighborIfIndexValue;
  xLibU32_t nextObjdvmrpNeighborIfIndexValue;
  
  L7_inet_addr_t objdvmrpNeighborAddressValue;
  L7_inet_addr_t nextObjdvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                          (xLibU8_t *) & objdvmrpNeighborIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpNeighborIfIndexValue, owa.len);


  owa.len = sizeof(L7_inet_addr_t);
  /* retrieve key: dvmrpNeighborAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                          (xLibU8_t *) & objdvmrpNeighborAddressValue, &owa.len);

  inetAddressZeroSet(L7_AF_INET, &nextObjdvmrpNeighborAddressValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    nextObjdvmrpNeighborIfIndexValue = objdvmrpNeighborIfIndexValue;   

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpNeighborAddressValue, owa.len);

    nextObjdvmrpNeighborIfIndexValue = objdvmrpNeighborIfIndexValue; 
    memcpy(&nextObjdvmrpNeighborAddressValue, &objdvmrpNeighborAddressValue, sizeof(L7_inet_addr_t));

  }

  do
  {
    owa.l7rc = usmDbDvmrpNeighborEntryNextGet (L7_UNIT_CURRENT,
                                               &nextObjdvmrpNeighborIfIndexValue,
                                               &nextObjdvmrpNeighborAddressValue);
  }while (owa.l7rc == L7_SUCCESS &&
          nextObjdvmrpNeighborIfIndexValue != objdvmrpNeighborIfIndexValue);

  if ((objdvmrpNeighborIfIndexValue != nextObjdvmrpNeighborIfIndexValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpNeighborAddressValue, owa.len);

  /* return the object value: dvmrpNeighborAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpNeighborAddressValue,
                           sizeof (objdvmrpNeighborAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborUpTime
*
* @purpose Get 'dvmrpNeighborUpTime'
*
* @description [dvmrpNeighborUpTime] The time since this DVMRP neighbor (last) became a neighbor of the local router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibStr256_t objdvmrpNeighborUpTimeValue;
  xLibUL32_t   NeighborUpTimeValue; 
  L7_timespec timeSpec;
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNeighborUpTimeGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &NeighborUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiConvertRawUpTime(NeighborUpTimeValue,(L7_timespec *)&timeSpec);
  osapiSnprintf(objdvmrpNeighborUpTimeValue, sizeof(objdvmrpNeighborUpTimeValue), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);
  /* return the object value: dvmrpNeighborUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNeighborUpTimeValue,
                           sizeof (objdvmrpNeighborUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborExpiryTime
*
* @purpose Get 'dvmrpNeighborExpiryTime'
*
* @description [dvmrpNeighborExpiryTime] The minimum time remaining before this DVMRP neighbor will be aged out.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborExpiryTime (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibStr256_t objdvmrpNeighborExpiryTimeValue;
  xLibUL32_t NeighborExpiryTimeValue;
  L7_timespec timeSpec;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNeighborExpiryTimeGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &NeighborExpiryTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  osapiConvertRawUpTime(NeighborExpiryTimeValue,(L7_timespec *)&timeSpec);
  osapiSnprintf(objdvmrpNeighborExpiryTimeValue, sizeof(objdvmrpNeighborExpiryTimeValue), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);
  /* return the object value: dvmrpNeighborExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNeighborExpiryTimeValue,
                           sizeof (objdvmrpNeighborExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborGenerationId
*
* @purpose Get 'dvmrpNeighborGenerationId'
*
* @description [dvmrpNeighborGenerationId] The neighboring router's generation identifier.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborGenerationId (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibUL32_t objdvmrpNeighborGenerationIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNeighborGenIdGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &objdvmrpNeighborGenerationIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpNeighborGenerationId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNeighborGenerationIdValue,
                           sizeof (objdvmrpNeighborGenerationIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborMajorVersion
*
* @purpose Get 'dvmrpNeighborMajorVersion'
*
* @description [dvmrpNeighborMajorVersion] The neighboring router's major DVMRP version number.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborMajorVersion (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU16_t));
  xLibU16_t objdvmrpNeighborMajorVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNeighborMajorVersionGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &objdvmrpNeighborMajorVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpNeighborMajorVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNeighborMajorVersionValue,
                           sizeof (objdvmrpNeighborMajorVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborMinorVersion
*
* @purpose Get 'dvmrpNeighborMinorVersion'
*
* @description [dvmrpNeighborMinorVersion] The neighboring router's minor DVMRP version number.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborMinorVersion (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU16_t));
  xLibU16_t objdvmrpNeighborMinorVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNeighborMinorVersionGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &objdvmrpNeighborMinorVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpNeighborMinorVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNeighborMinorVersionValue,
                           sizeof (objdvmrpNeighborMinorVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborCapabilities
*
* @purpose Get 'dvmrpNeighborCapabilities'
*
* @description [dvmrpNeighborCapabilities] This object describes the neighboring router's capabilities. The leaf bit indicates that the neighbor has only one interface with neighbors. The prune bit indicates that the neighbor supports pruning. The generationID b
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborCapabilities (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdvmrpNeighborCapabilitiesValue;
  xLibU16_t capability;

  xLibS8_t *pStrInfo_ipmcast_Leaf = "Leaf ";
  xLibS8_t *pStrInfo_ipmcast_Prune = "Prune ";
  xLibS8_t *pStrInfo_ipmcast_Genid = "GenID ";
  xLibS8_t *pStrInfo_ipmcast_Missing11441 = "Missing 11441";
  xLibS8_t *pStrInfo_common_None = "None";

  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  memset(objdvmrpNeighborCapabilitiesValue, L7_NULL, sizeof(objdvmrpNeighborCapabilitiesValue));

  owa.l7rc = usmDbDvmrpNeighborCapabilitiesGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &capability);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(capability != L7_NULL)
  {
    if (capability & (0x01 << L7_DVMRP_CAPABILITIES_LEAF))
    {
      osapiStrncat(objdvmrpNeighborCapabilitiesValue, pStrInfo_ipmcast_Leaf, (sizeof(objdvmrpNeighborCapabilitiesValue) - strlen(objdvmrpNeighborCapabilitiesValue) - 1));
    }
    if (capability & (0x01 << L7_DVMRP_CAPABILITIES_PRUNE))
    {
        osapiStrncat(objdvmrpNeighborCapabilitiesValue, pStrInfo_ipmcast_Prune, (sizeof(objdvmrpNeighborCapabilitiesValue) - strlen(objdvmrpNeighborCapabilitiesValue) - 1));
    }
    if (capability & (0x01 << L7_DVMRP_CAPABILITIES_GENID))
    {
      osapiStrncat(objdvmrpNeighborCapabilitiesValue, pStrInfo_ipmcast_Genid, (sizeof(objdvmrpNeighborCapabilitiesValue) - strlen(objdvmrpNeighborCapabilitiesValue) - 1));
    }
    if (capability & (0x01 << L7_DVMRP_CAPABILITIES_MTRACE))
    {
      osapiStrncat(objdvmrpNeighborCapabilitiesValue, pStrInfo_ipmcast_Missing11441, (sizeof(objdvmrpNeighborCapabilitiesValue) - strlen(objdvmrpNeighborCapabilitiesValue) - 1));
    }
  }
  else
  {
      osapiSnprintf (objdvmrpNeighborCapabilitiesValue, sizeof(pStrInfo_common_None), "%s", pStrInfo_common_None);
  }

  /* return the object value: dvmrpNeighborCapabilities */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdvmrpNeighborCapabilitiesValue,
                           strlen (objdvmrpNeighborCapabilitiesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborRcvRoutes
*
* @purpose Get 'dvmrpNeighborRcvRoutes'
*
* @description [dvmrpNeighborRcvRoutes] The total number of routes received in valid DVMRP packets received from this neighbor. This can be used to diagnose problems such as unicast route injection, as well as giving an indication of the level of DVMRP route ex
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborRcvRoutes (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibUL32_t objdvmrpNeighborRcvRoutesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNeighborRcvRoutesGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &objdvmrpNeighborRcvRoutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpNeighborRcvRoutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNeighborRcvRoutesValue,
                           sizeof (objdvmrpNeighborRcvRoutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborRcvBadPkts
*
* @purpose Get 'dvmrpNeighborRcvBadPkts'
*
* @description [dvmrpNeighborRcvBadPkts] The number of packet received from this neighbor which were discarded as invalid.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborRcvBadPkts (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibUL32_t objdvmrpNeighborRcvBadPktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNeighborRcvBadPktsGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &objdvmrpNeighborRcvBadPktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpNeighborRcvBadPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNeighborRcvBadPktsValue,
                           sizeof (objdvmrpNeighborRcvBadPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborRcvBadRoutes
*
* @purpose Get 'dvmrpNeighborRcvBadRoutes'
*
* @description [dvmrpNeighborRcvBadRoutes] The number of routes, in valid DVMRP packets received from this neighbor, which were ignored because the entry was invalid.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborRcvBadRoutes (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibUL32_t objdvmrpNeighborRcvBadRoutesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNeighborRcvBadRoutesGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &objdvmrpNeighborRcvBadRoutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpNeighborRcvBadRoutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNeighborRcvBadRoutesValue,
                           sizeof (objdvmrpNeighborRcvBadRoutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborState
*
* @purpose Get 'dvmrpNeighborState'
*
* @description [dvmrpNeighborState] State of the neighbor adjacency.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpNeighborTable_dvmrpNeighborState (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpNeighborIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpNeighborIfIndexValue;
  fpObjWa_t kwadvmrpNeighborAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keydvmrpNeighborAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibUL32_t objdvmrpNeighborStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpNeighborIfIndex */
  kwadvmrpNeighborIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborIfIndex,
                   (xLibU8_t *) & keydvmrpNeighborIfIndexValue, &kwadvmrpNeighborIfIndex.len);
  if (kwadvmrpNeighborIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborIfIndex);
    return kwadvmrpNeighborIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborIfIndexValue, kwadvmrpNeighborIfIndex.len);

  /* retrieve key: dvmrpNeighborAddress */
  kwadvmrpNeighborAddress.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpNeighborTable_dvmrpNeighborAddress,
                   (xLibU8_t *) & keydvmrpNeighborAddressValue, &kwadvmrpNeighborAddress.len);
  if (kwadvmrpNeighborAddress.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpNeighborAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpNeighborAddress);
    return kwadvmrpNeighborAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpNeighborAddressValue, kwadvmrpNeighborAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNeighborStateGet (L7_UNIT_CURRENT, keydvmrpNeighborIfIndexValue,
                              &keydvmrpNeighborAddressValue, &objdvmrpNeighborStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpNeighborState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNeighborStateValue,
                           sizeof (objdvmrpNeighborStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
