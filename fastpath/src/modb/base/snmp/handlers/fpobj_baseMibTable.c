/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseMibTable.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  6 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseMibTable_obj.h"
#include "_xe_baseSystemInfo_obj.h"
#include "usmdb_snmp_api.h"

/*******************************************************************************
* @function fpObjGet_baseMibTable_SnmpSupportedMibTableIndex
*
* @purpose Get 'SnmpSupportedMibTableIndex'
*
* @description The valid index of an entry in the Supported Mibs table 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseMibTable_SnmpSupportedMibTableIndex (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnmpSupportedMibTableIndexValue;
  xLibU32_t nextObjSnmpSupportedMibTableIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnmpSupportedMibTableIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseMibTable_SnmpSupportedMibTableIndex,
                          (xLibU8_t *) & objSnmpSupportedMibTableIndexValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjSnmpSupportedMibTableIndexValue = 1;	
    owa.l7rc = usmDbSnmpSupportedMibTableEntryGet (L7_UNIT_CURRENT,
                                     nextObjSnmpSupportedMibTableIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSnmpSupportedMibTableIndexValue,
                             owa.len);
    objSnmpSupportedMibTableIndexValue++;
    owa.l7rc =
      usmDbSnmpSupportedMibTableEntryNextGet (L7_UNIT_CURRENT,
                                              &objSnmpSupportedMibTableIndexValue);
    if(owa.l7rc  == L7_SUCCESS)
    {
      nextObjSnmpSupportedMibTableIndexValue = objSnmpSupportedMibTableIndexValue;
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSnmpSupportedMibTableIndexValue, owa.len);

  /* return the object value: SnmpSupportedMibTableIndex */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSnmpSupportedMibTableIndexValue,
                    sizeof (objSnmpSupportedMibTableIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseMibTable_SnmpSupportedMibTableMibName
*
* @purpose Get 'SnmpSupportedMibTableMibName'
*
* @description The name of the Supported Mib entry 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseMibTable_SnmpSupportedMibTableMibName (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnmpSupportedMibTableIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSnmpSupportedMibTableMibNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnmpSupportedMibTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseMibTable_SnmpSupportedMibTableIndex,
                          (xLibU8_t *) & keySnmpSupportedMibTableIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnmpSupportedMibTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnmpSupportedMibTableMibNameGet (L7_UNIT_CURRENT,
                                          keySnmpSupportedMibTableIndexValue,
                                          objSnmpSupportedMibTableMibNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnmpSupportedMibTableMibName */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objSnmpSupportedMibTableMibNameValue,
                    strlen (objSnmpSupportedMibTableMibNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseMibTable_SnmpSupportedMibTableMibOID
*
* @purpose Get 'SnmpSupportedMibTableMibOID'
*
* @description The OID of the Supported Mib entry 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseMibTable_SnmpSupportedMibTableMibOID (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnmpSupportedMibTableIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSnmpSupportedMibTableMibOIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnmpSupportedMibTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseMibTable_SnmpSupportedMibTableIndex,
                          (xLibU8_t *) & keySnmpSupportedMibTableIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnmpSupportedMibTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnmpSupportedMibTableMibOIDGet (L7_UNIT_CURRENT,
                                         keySnmpSupportedMibTableIndexValue,
                                         objSnmpSupportedMibTableMibOIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnmpSupportedMibTableMibOID */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objSnmpSupportedMibTableMibOIDValue,
                    strlen (objSnmpSupportedMibTableMibOIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseMibTable_sysSnmpSupportedMibTableMibDescription
*
* @purpose Get 'sysSnmpSupportedMibTableMibDescription'
*
* @description The description of the Supported Mib entry 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseMibTable_sysSnmpSupportedMibTableMibDescription (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnmpSupportedMibTableIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysSnmpSupportedMibTableMibDescriptionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnmpSupportedMibTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseMibTable_SnmpSupportedMibTableIndex,
                          (xLibU8_t *) & keySnmpSupportedMibTableIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnmpSupportedMibTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnmpSupportedMibTableMibDescriptionGet (L7_UNIT_CURRENT,
                                                 keySnmpSupportedMibTableIndexValue,
                                                 objsysSnmpSupportedMibTableMibDescriptionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: sysSnmpSupportedMibTableMibDescription */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) objsysSnmpSupportedMibTableMibDescriptionValue,
                    strlen (objsysSnmpSupportedMibTableMibDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseMibTable_SnmpSupportedMibTableMibEntryCreationTime
*
* @purpose Get 'SnmpSupportedMibTableMibEntryCreationTime'
*
* @description The entry creation time of the Supported Mib entry 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseMibTable_SnmpSupportedMibTableMibEntryCreationTime (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySnmpSupportedMibTableIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnmpSupportedMibTableMibEntryCreationTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SnmpSupportedMibTableIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseMibTable_SnmpSupportedMibTableIndex,
                          (xLibU8_t *) & keySnmpSupportedMibTableIndexValue,
                          &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySnmpSupportedMibTableIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSnmpSupportedMibTableMibEntryCreationTimeGet (L7_UNIT_CURRENT,
                                                       keySnmpSupportedMibTableIndexValue,
                                                       &objSnmpSupportedMibTableMibEntryCreationTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SnmpSupportedMibTableMibEntryCreationTime */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objSnmpSupportedMibTableMibEntryCreationTimeValue,
                    sizeof (objSnmpSupportedMibTableMibEntryCreationTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseMibTable_SnmpSupportedMibTableLastChangeTime
*
* @purpose Get 'SnmpSupportedMibTableLastChangeTime'
*
* @description The time of the last changed to the Supported Mib table 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseMibTable_SnmpSupportedMibTableLastChangeTime (void *wap,
                                                                    void *bufp)
{
#if 0
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSnmpSupportedMibTableLastChangeTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSnmpSupportedMibTableLastChangeTimeGet (L7_UNIT_CURRENT,
                                                 &objSnmpSupportedMibTableLastChangeTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSnmpSupportedMibTableLastChangeTimeValue,
                     sizeof (objSnmpSupportedMibTableLastChangeTimeValue));

  /* return the object value: SnmpSupportedMibTableLastChangeTime */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objSnmpSupportedMibTableLastChangeTimeValue,
                    sizeof (objSnmpSupportedMibTableLastChangeTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#else
  return fpObjGet_baseSystemInfo_sysUpTime(wap, bufp);
#endif
}
