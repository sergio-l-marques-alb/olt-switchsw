/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDhcpServerAddressConflict.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to snmp-object.xml
*
* @create  9 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseDhcpServerAddressConflict_obj.h"
#include "usmdb_dhcps_api.h"

/*******************************************************************************
* @function fpObjGet_baseDhcpServerAddressConflict_IPAddr
*
* @purpose Get 'IPAddr'
*
* @description This specifies the conflicting IP address assigned to the client 
*              by DHCP server. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerAddressConflict_IPAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPAddrValue;
  xLibU32_t nextObjIPAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerAddressConflict_IPAddr,
                          (xLibU8_t *) & objIPAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDhcpsConflictGetFirst (L7_UNIT_CURRENT, &nextObjIPAddrValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIPAddrValue, owa.len);
    owa.l7rc = usmDbDhcpsConflictGetNext (L7_UNIT_CURRENT, objIPAddrValue,
                                          &nextObjIPAddrValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIPAddrValue, owa.len);

  /* return the object value: IPAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIPAddrValue,
                           sizeof (objIPAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerAddressConflict_DhcpServerAddressConflictDetectionType
*
* @purpose Get 'DhcpServerAddressConflictDetectionType'
*
* @description This specifies the address-conflict detection-method. 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_baseDhcpServerAddressConflict_DhcpServerAddressConflictDetectionType
(void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerAddressConflictDetectionTypeValue;
  xLibU32_t tempDetectionTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddr */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerAddressConflict_IPAddr,
                          (xLibU8_t *) & keyIPAddrValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddrValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDhcpsConflictGet (L7_UNIT_CURRENT, keyIPAddrValue,
                                    &tempDetectionTimeValue,
                                    &objDhcpServerAddressConflictDetectionTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DhcpServerAddressConflictDetectionType */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDhcpServerAddressConflictDetectionTypeValue,
                    sizeof (objDhcpServerAddressConflictDetectionTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerAddressConflict_DhcpServerAddressConflictDetectionTime
*
* @purpose Get 'DhcpServerAddressConflictDetectionTime'
*
* @description This specifies the sysUpTime at which the address-conflict was 
*              detected. 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_baseDhcpServerAddressConflict_DhcpServerAddressConflictDetectionTime
(void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerAddressConflictDetectionTimeValue;
  xLibU32_t tempDetectionTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddr */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerAddressConflict_IPAddr,
                          (xLibU8_t *) & keyIPAddrValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddrValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDhcpsConflictGet (L7_UNIT_CURRENT, keyIPAddrValue,
                                    &objDhcpServerAddressConflictDetectionTimeValue,
                                    &tempDetectionTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DhcpServerAddressConflictDetectionTime */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDhcpServerAddressConflictDetectionTimeValue,
                    sizeof (objDhcpServerAddressConflictDetectionTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerAddressConflict_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This specifies the row-status of the conflicting-address
*              entry. Implemented values  are - active(1) and destroy(6).
*              For a valid range, the row-satus will return active(1).
*              When it is set to destroy(6), the correspo 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerAddressConflict_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddr */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerAddressConflict_IPAddr,
                          (xLibU8_t *) & keyIPAddrValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddrValue, kwa.len);

  /* get the value from application */
  objStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerAddressConflict_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This specifies the row-status of the conflicting-address
*              entry. Implemented values  are - active(1) and destroy(6).
*              For a valid range, the row-satus will return active(1).
*              When it is set to destroy(6), the correspo 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerAddressConflict_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: IPAddr */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerAddressConflict_IPAddr,
                          (xLibU8_t *) & keyIPAddrValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddrValue, kwa.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_ACTIVE)
  {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbDhcpsClearConflict(L7_UNIT_CURRENT, keyIPAddrValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_DHCPS_SPECIFIC_ADDCONFLICT_DEL_FAILED;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}



/*******************************************************************************
* @function fpObjSet_baseDhcpServerAddressConflict_DhcpServerClearAllAddressConflicts
*
* @purpose Set 'DhcpServerClearAllAddressConflicts'
*
* @description Clears all the address-conflicts found by DHCP server,when set 
*              to enable. 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_baseDhcpServerAddressConflict_DhcpServerClearAllAddressConflicts (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerClearAllAddressConflictsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DhcpServerClearAllAddressConflicts */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objDhcpServerClearAllAddressConflictsValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerClearAllAddressConflictsValue,
                     owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDhcpsClearAllConflict (L7_UNIT_CURRENT);
  if (owa.l7rc != L7_SUCCESS)
  {
    objDhcpServerClearAllAddressConflictsValue = L7_FALSE;
    owa.rc = XLIBRC_DHCPS_CLEAR_CONFLICTS_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objDhcpServerClearAllAddressConflictsValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerAddressConflict_DhcpServerAddressConflictLogging
*
* @purpose Get 'DhcpServerAddressConflictLogging'
*
* @description  It is used to enable, or disables the logging of address-conflicts 
*              on a DHCP Server. 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_baseDhcpServerAddressConflict_DhcpServerAddressConflictLogging (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerAddressConflictLoggingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsConflictLoggingGet (L7_UNIT_CURRENT,
                                  &objDhcpServerAddressConflictLoggingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerAddressConflictLoggingValue,
                     sizeof (objDhcpServerAddressConflictLoggingValue));

  /* return the object value: DhcpServerAddressConflictLogging */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDhcpServerAddressConflictLoggingValue,
                    sizeof (objDhcpServerAddressConflictLoggingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerAddressConflict_DhcpServerAddressConflictLogging
*
* @purpose Set 'DhcpServerAddressConflictLogging'
*
* @description  It is used to enable, or disables the logging of address-conflicts 
*              on a DHCP Server. 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_baseDhcpServerAddressConflict_DhcpServerAddressConflictLogging (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerAddressConflictLoggingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DhcpServerAddressConflictLogging */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objDhcpServerAddressConflictLoggingValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerAddressConflictLoggingValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDhcpsConflictLoggingSet (L7_UNIT_CURRENT,
                                  objDhcpServerAddressConflictLoggingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
