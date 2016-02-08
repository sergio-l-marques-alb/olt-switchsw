/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basetraceRouteCtrlConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  21 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basetraceRouteCtrlConfig_obj.h"
#include "usmdb_traceroute_api.h"

/*********************************************************************
*  Set the Admin Status field
*
* @param    index   @b{(input)} Unit for this operation
* @param    testName @b{(input)} Unit for this operation
* @param    traceRouteAdminStatus   @b{(input)} Admin Status
*
* @returns  L7_SUCCESS if traceroute operation starts
*           L7_FAILURE if Row Status is not active or session is in progress.
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t
fpObjUtil_basetraceRouteCtrlConfig_adminStatusSet(L7_uchar8 *index, L7_uchar8 *testName,L7_uint32 traceRouteAdminStatus)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_ushort16 handle;
   L7_uint32 ipDa;

  
  switch (traceRouteAdminStatus)
  {
    case L7_ENABLE:

      rc = usmDbTraceRouteHandleGet( index,testName, &handle);

      if (rc == L7_SUCCESS)
      {

        if (usmDbTraceRouteDestAddrGet(handle, &ipDa)== L7_SUCCESS && ipDa != 0)
        {
          rc = usmDbTraceRouteStart(handle);
        }
        else
        {
          rc = L7_FAILURE;
        }
      }
      break;

    case L7_DISABLE:

      rc = usmDbTraceRouteHandleGet( index,testName, &handle);

      if (rc == L7_SUCCESS)
      {
        if (usmDbTraceRouteDestAddrGet(handle, &ipDa)== L7_SUCCESS && ipDa != 0)
        {
          rc = usmDbTraceRouteSessionEnd(handle);
        }
      }
      break;

    default :
      rc = L7_FAILURE;
      break;
  }

  return rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex
*
* @purpose Get 'traceRouteCtlOwnerIndex'
*
* @description [traceRouteCtlOwnerIndex] To facilitate the provisioning of access control by a security administrator using the View-Based Access Control Model (RFC 2575, VACM) for tables in which multiple users may need to independently create or modify entries, the initial index is used as an 'owner index'. Such an initial index has a syntax of SnmpAdminString, and can thus be trivially mapped to a securityName or groupName as defined in VACM, in accordance with a security policy. When used in conjunction with such a security policy all entries in the table belonging to a particular user (or group) will have the same value for this initial index. For a given user's entries in a particular table, the object identifiers for the information in these entries will have the same subidentifiers (except for the 'column' subidentifier) up to the end of the encoded owner index. To configure VACM to permit access to this portion of the table, one would create vacmViewTreeFamilyTable entries with the value of vacmViewTreeFamilySubtree including the owner index portion, and vacmViewTreeFamilyMask 'wildcarding' the column subidentifier. More elaborate configurations are possible.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex (void *wap, void *bufp)
{

  xLibStr256_t objtraceRouteCtlOwnerIndexValue;
  xLibStr256_t nextObjtraceRouteCtlOwnerIndexValue;
  xLibStr256_t objtraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

    L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;

  /* retrieve key: traceRouteCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                          (xLibU8_t *) objtraceRouteCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objtraceRouteCtlOwnerIndexValue,0x00,sizeof(objtraceRouteCtlOwnerIndexValue));
    memset(objtraceRouteCtlTestNameValue,0x00,sizeof(objtraceRouteCtlTestNameValue));
    handle = 0;
    owa.l7rc = usmDbTraceRouteGetNext( objtraceRouteCtlOwnerIndexValue, objtraceRouteCtlTestNameValue, &handle,
          &operStatus, &vrfId, &ipDa, 
          &probeSize, &probePerHop, 
          &probeInterval, &dontFrag, &port, 
          &maxTtl, &initTtl, &maxFail);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objtraceRouteCtlOwnerIndexValue, owa.len);
    strcpy( nextObjtraceRouteCtlOwnerIndexValue,objtraceRouteCtlOwnerIndexValue);
    do
    {
      memset(objtraceRouteCtlTestNameValue,0x00,sizeof(objtraceRouteCtlTestNameValue));
      owa.l7rc = usmDbTraceRouteGetNext( objtraceRouteCtlOwnerIndexValue, objtraceRouteCtlTestNameValue, &handle,
            &operStatus, &vrfId, &ipDa, 
            &probeSize, &probePerHop, 
            &probeInterval, &dontFrag, &port, 
            &maxTtl, &initTtl, &maxFail);
    }
    while ((strcmp(objtraceRouteCtlOwnerIndexValue, nextObjtraceRouteCtlOwnerIndexValue)==0)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  strcpy( nextObjtraceRouteCtlOwnerIndexValue,objtraceRouteCtlOwnerIndexValue);

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjtraceRouteCtlOwnerIndexValue, owa.len);

  /* return the object value: traceRouteCtlOwnerIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjtraceRouteCtlOwnerIndexValue,
                           strlen (objtraceRouteCtlOwnerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_traceRouteCtlTestName
*
* @purpose Get 'traceRouteCtlTestName'
*
* @description [traceRouteCtlTestName] The name of a traceroute test. This is locally unique, within the scope of an traceRouteCtlOwnerIndex.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_traceRouteCtlTestName (void *wap, void *bufp)
{

  xLibStr256_t objtraceRouteCtlOwnerIndexValue;
  xLibStr256_t nextObjtraceRouteCtlOwnerIndexValue;
  xLibStr256_t objtraceRouteCtlTestNameValue;
  xLibStr256_t nextObjtraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  /* retrieve key: traceRouteCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                          (xLibU8_t *) objtraceRouteCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objtraceRouteCtlOwnerIndexValue, owa.len);

  /* retrieve key: traceRouteCtlTestName */
  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                          (xLibU8_t *) objtraceRouteCtlTestNameValue, &owa.len);
  strcpy( nextObjtraceRouteCtlOwnerIndexValue,objtraceRouteCtlOwnerIndexValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objtraceRouteCtlTestNameValue,0x00,sizeof(objtraceRouteCtlTestNameValue));
    handle = 0;
    owa.rc = usmDbTraceRouteGetNext( objtraceRouteCtlOwnerIndexValue, objtraceRouteCtlTestNameValue, &handle,
          &operStatus, &vrfId, &ipDa, 
          &probeSize, &probePerHop, 
          &probeInterval, &dontFrag, &port, 
          &maxTtl, &initTtl, &maxFail);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objtraceRouteCtlTestNameValue, owa.len);

    owa.rc = usmDbTraceRouteGetNext( objtraceRouteCtlOwnerIndexValue, objtraceRouteCtlTestNameValue, &handle,
          &operStatus, &vrfId, &ipDa, 
          &probeSize, &probePerHop, 
          &probeInterval, &dontFrag, &port, 
          &maxTtl, &initTtl, &maxFail);

  }

  if ((strcmp(objtraceRouteCtlOwnerIndexValue, nextObjtraceRouteCtlOwnerIndexValue) != 0)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
   
  strcpy(nextObjtraceRouteCtlTestNameValue,objtraceRouteCtlTestNameValue);
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjtraceRouteCtlTestNameValue, owa.len);

  /* return the object value: traceRouteCtlTestName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjtraceRouteCtlTestNameValue,
                           strlen (objtraceRouteCtlTestNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex_traceRouteCtlTestName
*
* @purpose Get 'traceRouteCtlOwnerIndex + traceRouteCtlTestName +'
*
* @description [traceRouteCtlOwnerIndex]: To facilitate the provisioning of
*              access control by a security administrator using the View-Based
*              Access Control Model (RFC 2575, VACM) for tables in
*              which multiple users may need to independently create or modify
*              entries, the initial index is used as an 'owner index'.
*              Such an initial index has a syntax of SnmpAdminString, and
*              can thus be trivially mapped to a securityName or groupName
*              as defined in VACM, in accordance with a security policy.
*              When used in conjunction with such a security policy all
*              entries in the table belonging to a particular user (or group)
*              will have the same value for this initial index. For a
*              given user's entries in a particular table, the object identifiers
*              for the information in these entries will have the
*              same subidentifiers (except for the 'column' subidentifier)
*              up to the end of the encoded owner index. To configure VACM
*              to permit access to this portion of the table, one would
*              create vacmViewTreeFamilyTable entries with the value of vacmViewTreeFamilySubtree
*              including the owner index portion,
*              and vacmViewTreeFamilyMask 'wildcarding' the column subidentifier.
*              More elaborate configurations are possible. 
*              [traceRouteCtlTestName]: The name of a traceroute test. This
*              is locally unique, within the scope of an traceRouteCtlOwnerIndex.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex_traceRouteCtlTestName
(void *wap, void *bufp[], xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owatraceRouteCtlOwnerIndex = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtraceRouteCtlOwnerIndexValue,
    nextObjtraceRouteCtlOwnerIndexValue;
  fpObjWa_t owatraceRouteCtlTestName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtraceRouteCtlTestNameValue, nextObjtraceRouteCtlTestNameValue;
  void *outtraceRouteCtlOwnerIndex = (void *) bufp[--keyCount];
  void *outtraceRouteCtlTestName = (void *) bufp[--keyCount];

    L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  
  FPOBJ_TRACE_ENTER (outtraceRouteCtlOwnerIndex);
  FPOBJ_TRACE_ENTER (outtraceRouteCtlTestName);

  /* retrieve key: traceRouteCtlOwnerIndex */
  owatraceRouteCtlOwnerIndex.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) objtraceRouteCtlOwnerIndexValue,
                   &owatraceRouteCtlOwnerIndex.len);
  if (owatraceRouteCtlOwnerIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: traceRouteCtlTestName */
    owatraceRouteCtlTestName.rc =
      xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                     (xLibU8_t *) objtraceRouteCtlTestNameValue,
                     &owatraceRouteCtlTestName.len);
  }
  else
  {
     memset(objtraceRouteCtlOwnerIndexValue,0x00,sizeof(objtraceRouteCtlOwnerIndexValue));
     memset(objtraceRouteCtlTestNameValue,0x00,sizeof(objtraceRouteCtlTestNameValue));
     handle = 0;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteCtlOwnerIndex,
                           &objtraceRouteCtlOwnerIndexValue,
                           owatraceRouteCtlOwnerIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteCtlTestName,
                           &objtraceRouteCtlTestNameValue,
                           owatraceRouteCtlTestName.len);
  owa.rc = usmDbTraceRouteGetNext( objtraceRouteCtlOwnerIndexValue, objtraceRouteCtlTestNameValue, &handle,
        &operStatus, &vrfId, &ipDa, 
        &probeSize, &probePerHop, 
        &probeInterval, &dontFrag, &port, 
        &maxTtl, &initTtl, &maxFail);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outtraceRouteCtlOwnerIndex, owatraceRouteCtlOwnerIndex);
    FPOBJ_TRACE_EXIT (outtraceRouteCtlTestName, owatraceRouteCtlTestName);
    return owa.rc;
  }
  strcpy(nextObjtraceRouteCtlOwnerIndexValue, objtraceRouteCtlOwnerIndexValue);
  strcpy(nextObjtraceRouteCtlTestNameValue, objtraceRouteCtlTestNameValue);
  
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteCtlOwnerIndex,
                           &nextObjtraceRouteCtlOwnerIndexValue,
                           owatraceRouteCtlOwnerIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteCtlTestName,
                           &nextObjtraceRouteCtlTestNameValue,
                           owatraceRouteCtlTestName.len);

  /* return the object value: traceRouteCtlOwnerIndex */
  xLibBufDataSet (outtraceRouteCtlOwnerIndex,
                  (xLibU8_t *) nextObjtraceRouteCtlOwnerIndexValue,
                  strlen (nextObjtraceRouteCtlOwnerIndexValue));

  /* return the object value: traceRouteCtlTestName */
  xLibBufDataSet (outtraceRouteCtlTestName,
                  (xLibU8_t *) nextObjtraceRouteCtlTestNameValue,
                  strlen (nextObjtraceRouteCtlTestNameValue));
  FPOBJ_TRACE_EXIT (outtraceRouteCtlOwnerIndex, owatraceRouteCtlOwnerIndex);
  FPOBJ_TRACE_EXIT (outtraceRouteCtlTestName, owatraceRouteCtlTestName);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlTargetAddressType
*
* @purpose Get 'CtlTargetAddressType'
*
* @description [CtlTargetAddressType]: Specifies the type of host address
*              to be used on the traceroute request at the remote host. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlTargetAddressType (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlTargetAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  objCtlTargetAddressTypeValue = 1; /* always set to ipv4 */
    /*usmDbTraceRouteResultGet (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              &objCtlTargetAddressTypeValue); */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlTargetAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlTargetAddressTypeValue,
                           sizeof (objCtlTargetAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlTargetAddressType
*
* @purpose Set 'CtlTargetAddressType'
*
* @description [CtlTargetAddressType]: Specifies the type of host address
*              to be used on the traceroute request at the remote host. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlTargetAddressType (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlTargetAddressTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlTargetAddressType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlTargetAddressTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlTargetAddressTypeValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
  /*owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              objCtlTargetAddressTypeValue);*/

  owa.l7rc = L7_SUCCESS;
  objCtlTargetAddressTypeValue = 1; /* always set to ipv4 */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlTargetAddress
*
* @purpose Get 'CtlTargetAddress'
*
* @description [CtlTargetAddress]: Specifies the host address used on the
*              traceroute request at the remote host. The host address type
*              can be determined by the examining the value of the corresponding
*              traceRouteCtlTargetAddressType index element. A value
*              for this object MUST be set prior to transitioning its
*              corresponding traceRouteCtlEntry to active(1) via traceRouteCtlRowStatus.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlTargetAddress (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlTargetAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

   /* get the value from application */
  owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &objCtlTargetAddressValue, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlTargetAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlTargetAddressValue,
                           sizeof (objCtlTargetAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlTargetAddress
*
* @purpose Set 'CtlTargetAddress'
*
* @description [CtlTargetAddress]: Specifies the host address used on the
*              traceroute request at the remote host. The host address type
*              can be determined by the examining the value of the corresponding
*              traceRouteCtlTargetAddressType index element. A value
*              for this object MUST be set prior to transitioning its
*              corresponding traceRouteCtlEntry to active(1) via traceRouteCtlRowStatus.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlTargetAddress (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlTargetAddressValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_ushort16 handle;

  /* retrieve object: CtlTargetAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlTargetAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlTargetAddressValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

    /* set the value in application */

  owa.l7rc = usmDbTraceRouteHandleGet(keytraceRouteCtlOwnerIndexValue,
  	keytraceRouteCtlTestNameValue, &handle);

  if(owa.l7rc == L7_SUCCESS)
  {
     owa.l7rc = usmDbTraceRouteDestAddrSet(handle,objCtlTargetAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlByPassRouteTable
*
* @purpose Get 'CtlByPassRouteTable'
*
* @description [CtlByPassRouteTable]: The purpose of this object is to optionally
*              enable bypassing the route table. If enabled, the remote
*              host will bypass the normal routing tables and send directly
*              to a host on an attached network. If the host is not
*              on a directly-attached network, an error is returned. This
*              option can be used to perform the traceroute operation to
*              a local host through an interface that has no route defined
*              (e.g., after the interface was dropped by routed). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlByPassRouteTable (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlByPassRouteTableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
    /*usmDbTraceRouteResultGet (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              &objCtlByPassRouteTableValue); */
  objCtlByPassRouteTableValue = 0;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlByPassRouteTable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlByPassRouteTableValue,
                           sizeof (objCtlByPassRouteTableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlByPassRouteTable
*
* @purpose Set 'CtlByPassRouteTable'
*
* @description [CtlByPassRouteTable]: The purpose of this object is to optionally
*              enable bypassing the route table. If enabled, the remote
*              host will bypass the normal routing tables and send directly
*              to a host on an attached network. If the host is not
*              on a directly-attached network, an error is returned. This
*              option can be used to perform the traceroute operation to
*              a local host through an interface that has no route defined
*              (e.g., after the interface was dropped by routed). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlByPassRouteTable (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlByPassRouteTableValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlByPassRouteTable */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlByPassRouteTableValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlByPassRouteTableValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
  /*owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              objCtlByPassRouteTableValue);*/
  objCtlByPassRouteTableValue = 0;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlDataSize
*
* @purpose Get 'CtlDataSize'
*
* @description [CtlDataSize]: Specifies the size of the data portion of a
*              traceroute request in octets. A traceroute request is essentially
*              transmitted by encoding a UDP datagram into a IP packet.
*              So subtracting the size of a UDP header (8 octets) and
*              the size of a IP header (20 octets) yields a maximum of 65507
*              octets. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlDataSize (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlDataSizeValue;
    L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCtlDataSizeValue = probeSize;

  /* return the object value: CtlDataSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlDataSizeValue,
                           sizeof (objCtlDataSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlDataSize
*
* @purpose Set 'CtlDataSize'
*
* @description [CtlDataSize]: Specifies the size of the data portion of a
*              traceroute request in octets. A traceroute request is essentially
*              transmitted by encoding a UDP datagram into a IP packet.
*              So subtracting the size of a UDP header (8 octets) and
*              the size of a IP header (20 octets) yields a maximum of 65507
*              octets. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlDataSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlDataSizeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  L7_ushort16 handle;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlDataSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCtlDataSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlDataSizeValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */

  if (usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue, &handle)  == L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteProbeSizeSet(handle,objCtlDataSizeValue);
  }
  else
  {
    owa.l7rc = L7_FAILURE;
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlTimeOut
*
* @purpose Get 'CtlTimeOut'
*
* @description [CtlTimeOut]: Specifies the time-out value, in seconds, for
*              a traceroute request. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlTimeOut (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlTimeOutValue;

    L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCtlTimeOutValue = probeInterval;

  /* return the object value: CtlTimeOut */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlTimeOutValue,
                           sizeof (objCtlTimeOutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlTimeOut
*
* @purpose Set 'CtlTimeOut'
*
* @description [CtlTimeOut]: Specifies the time-out value, in seconds, for
*              a traceroute request. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlTimeOut (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlTimeOutValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
    L7_ushort16 handle;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlTimeOut */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCtlTimeOutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlTimeOutValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */

  if (usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteProbeIntervalSet(handle,objCtlTimeOutValue);
  }
  else
  {
    owa.l7rc  = L7_FAILURE;
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlProbesPerHop
*
* @purpose Get 'CtlProbesPerHop'
*
* @description [CtlProbesPerHop]: Specifies the number of times to reissue
*              a traceroute request with the same time-to-live (TTL) value.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlProbesPerHop (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlProbesPerHopValue;

    L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCtlProbesPerHopValue = probePerHop;

  /* return the object value: CtlProbesPerHop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlProbesPerHopValue,
                           sizeof (objCtlProbesPerHopValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlProbesPerHop
*
* @purpose Set 'CtlProbesPerHop'
*
* @description [CtlProbesPerHop]: Specifies the number of times to reissue
*              a traceroute request with the same time-to-live (TTL) value.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlProbesPerHop (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlProbesPerHopValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
    L7_ushort16 handle;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlProbesPerHop */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlProbesPerHopValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlProbesPerHopValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
if (usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteProbePerHopSet(handle,objCtlProbesPerHopValue);
  }
  else
  {
    owa.l7rc  = L7_FAILURE;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlPort
*
* @purpose Get 'CtlPort'
*
* @description [CtlPort]: Specifies the UDP port to send the traceroute request
*              to. Need to specify a port that is not in use at the
*              destination (target) host. The default value for this object
*              is the IANA assigned port, 33434, for the traceroute function.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlPort (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlPortValue;

    L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCtlPortValue = port;

  /* return the object value: CtlPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlPortValue,
                           sizeof (objCtlPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlPort
*
* @purpose Set 'CtlPort'
*
* @description [CtlPort]: Specifies the UDP port to send the traceroute request
*              to. Need to specify a port that is not in use at the
*              destination (target) host. The default value for this object
*              is the IANA assigned port, 33434, for the traceroute function.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlPortValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
    L7_ushort16 handle;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlPort */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCtlPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlPortValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
  if (usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteDestPortSet(handle,objCtlPortValue);
  }
  else
  {
    owa.l7rc  = L7_FAILURE;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlMaxTtl
*
* @purpose Get 'CtlMaxTtl'
*
* @description [CtlMaxTtl]: Specifies the maximum time-to-live value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlMaxTtl (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlMaxTtlValue;

    L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
 owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCtlMaxTtlValue = maxTtl;

  /* return the object value: CtlMaxTtl */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlMaxTtlValue,
                           sizeof (objCtlMaxTtlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlMaxTtl
*
* @purpose Set 'CtlMaxTtl'
*
* @description [CtlMaxTtl]: Specifies the maximum time-to-live value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlMaxTtl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlMaxTtlValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  L7_ushort16 handle;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlMaxTtl */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCtlMaxTtlValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlMaxTtlValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
  if (usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteMaxTtlSet(handle,objCtlMaxTtlValue);
  }
  else
  {
    owa.l7rc  = L7_FAILURE;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlDSField
*
* @purpose Get 'CtlDSField'
*
* @description [CtlDSField]: Specifies the value to store in the Differentiated
*              Services (DS) Field in the IP packet used to encapsulate
*              the traceroute probe. The DS Field is defined as the Type
*              of Service (TOS) octet in a IPv4 header or as the Traffic
*              Class octet in a IPv6 header. The value of this object must
*              be a decimal integer in the range from 0 to 255. This option
*              can be used to determine what effect an explicit DS Field
*              setting has on a traceroute response. Not all values are
*              legal or meaningful. DS Field usage is often not supported
*              by IP implementations. A value of 0 means that the function
*              represented by this option is not supported. Useful TOS
*              octet values are probably '16' (low delay) and '8' ( high throughput).
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlDSField (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlDSFieldValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbTraceRouteResultGet (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              &objCtlDSFieldValue);*/
  objCtlDSFieldValue = 0;
  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlDSField */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlDSFieldValue,
                           sizeof (objCtlDSFieldValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlDSField
*
* @purpose Set 'CtlDSField'
*
* @description [CtlDSField]: Specifies the value to store in the Differentiated
*              Services (DS) Field in the IP packet used to encapsulate
*              the traceroute probe. The DS Field is defined as the Type
*              of Service (TOS) octet in a IPv4 header or as the Traffic
*              Class octet in a IPv6 header. The value of this object must
*              be a decimal integer in the range from 0 to 255. This option
*              can be used to determine what effect an explicit DS Field
*              setting has on a traceroute response. Not all values are
*              legal or meaningful. DS Field usage is often not supported
*              by IP implementations. A value of 0 means that the function
*              represented by this option is not supported. Useful TOS
*              octet values are probably '16' (low delay) and '8' ( high throughput).
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlDSField (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlDSFieldValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlDSField */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCtlDSFieldValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlDSFieldValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlSourceAddressType
*
* @purpose Get 'CtlSourceAddressType'
*
* @description [CtlSourceAddressType]: Specifies the type of the source address,
*              traceRouteCtlSourceAddress, to be used at a remote host
*              when performing a traceroute operation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlSourceAddressType (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlSourceAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbTraceRouteResultGet (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              &objCtlSourceAddressTypeValue);*/
  objCtlSourceAddressTypeValue = 0; /*unknown type */
  owa.l7rc = L7_SUCCESS ;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlSourceAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlSourceAddressTypeValue,
                           sizeof (objCtlSourceAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlSourceAddressType
*
* @purpose Set 'CtlSourceAddressType'
*
* @description [CtlSourceAddressType]: Specifies the type of the source address,
*              traceRouteCtlSourceAddress, to be used at a remote host
*              when performing a traceroute operation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlSourceAddressType (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlSourceAddressTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlSourceAddressType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlSourceAddressTypeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlSourceAddressTypeValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
 /* owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              objCtlSourceAddressTypeValue);  */
   objCtlSourceAddressTypeValue= 0;      /*set to unknown ttye */ 
  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlSourceAddress
*
* @purpose Get 'CtlSourceAddress'
*
* @description [CtlSourceAddress]: Use the specified IP address (which must
*              be given as an IP number, not a hostname) as the source address
*              in outgoing probe packets. On hosts with more than one
*              IP address, this option can be used to force the source
*              address to be something other than the primary IP address of
*              the interface the probe packet is sent on. If the IP address
*              is not one of this machine's interface addresses, an error
*              is returned and nothing is sent. A zero length octet string
*              value for this object disables source address specification.
*              The address type (InetAddressType) that relates to this
*              object is specified by the corresponding value of traceRouteCtlSourceAddressType.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlSourceAddress (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlSourceAddress
*
* @purpose Set 'CtlSourceAddress'
*
* @description [CtlSourceAddress]: Use the specified IP address (which must
*              be given as an IP number, not a hostname) as the source address
*              in outgoing probe packets. On hosts with more than one
*              IP address, this option can be used to force the source
*              address to be something other than the primary IP address of
*              the interface the probe packet is sent on. If the IP address
*              is not one of this machine's interface addresses, an error
*              is returned and nothing is sent. A zero length octet string
*              value for this object disables source address specification.
*              The address type (InetAddressType) that relates to this
*              object is specified by the corresponding value of traceRouteCtlSourceAddressType.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlSourceAddress (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCtlSourceAddressValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlSourceAddress */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objCtlSourceAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objCtlSourceAddressValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlIfIndex
*
* @purpose Get 'CtlIfIndex'
*
* @description [CtlIfIndex]: Setting this object to an interface's ifIndex
*              prior to starting a remote traceroute operation directs the
*              traceroute probes to be transmitted over the specified interface.
*              A value of zero for this object implies that this
*              option is not enabled. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlIfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  objCtlIfIndexValue = 0;
  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc  != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlIfIndexValue,
                           sizeof (objCtlIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlIfIndex
*
* @purpose Set 'CtlIfIndex'
*
* @description [CtlIfIndex]: Setting this object to an interface's ifIndex
*              prior to starting a remote traceroute operation directs the
*              traceroute probes to be transmitted over the specified interface.
*              A value of zero for this object implies that this
*              option is not enabled. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlIfIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlIfIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlIfIndex */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCtlIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlIfIndexValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlMiscOptions
*
* @purpose Get 'CtlMiscOptions'
*
* @description [CtlMiscOptions]: Enables an application to specify implementation
*              dependent options. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlMiscOptions (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCtlMiscOptionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbTraceRouteResultGet (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              objCtlMiscOptionsValue);*/
  memset(objCtlMiscOptionsValue, 0x00, sizeof(objCtlMiscOptionsValue));
  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc  != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlMiscOptions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objCtlMiscOptionsValue,
                           strlen (objCtlMiscOptionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlMiscOptions
*
* @purpose Set 'CtlMiscOptions'
*
* @description [CtlMiscOptions]: Enables an application to specify implementation
*              dependent options. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlMiscOptions (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCtlMiscOptionsValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlMiscOptions */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objCtlMiscOptionsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objCtlMiscOptionsValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlMaxFailures
*
* @purpose Get 'CtlMaxFailures'
*
* @description [CtlMaxFailures]: The value of this object indicates the maximum
*              number of consecutive timeouts allowed before terminating
*              a remote traceroute request. A value of either 255 (maximum
*              hop count/possible TTL value) or a 0 indicates that the
*              function of terminating a remote traceroute request when
*              a specific number of successive timeouts are detected is disabled.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlMaxFailures (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlMaxFailuresValue;

    L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCtlMaxFailuresValue = maxFail;
  /* return the object value: CtlMaxFailures */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlMaxFailuresValue,
                           sizeof (objCtlMaxFailuresValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlMaxFailures
*
* @purpose Set 'CtlMaxFailures'
*
* @description [CtlMaxFailures]: The value of this object indicates the maximum
*              number of consecutive timeouts allowed before terminating
*              a remote traceroute request. A value of either 255 (maximum
*              hop count/possible TTL value) or a 0 indicates that the
*              function of terminating a remote traceroute request when
*              a specific number of successive timeouts are detected is disabled.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlMaxFailures (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlMaxFailuresValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  L7_ushort16 handle;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlMaxFailures */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlMaxFailuresValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlMaxFailuresValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
  if (usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteMaxFailSet(handle,objCtlMaxFailuresValue);
  }
  else
  {
    owa.l7rc  = L7_FAILURE;
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlDontFragment
*
* @purpose Get 'CtlDontFragment'
*
* @description [CtlDontFragment]: This object enables setting of the don't
*              fragment flag (DF) in the IP header for a probe. Use of this
*              object enables performing a manual PATH MTU test. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlDontFragment (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlDontFragmentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbTraceRouteResultGet (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              &objCtlDontFragmentValue);*/
  objCtlDontFragmentValue = L7_FALSE;
  owa.l7rc  = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlDontFragment */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlDontFragmentValue,
                           sizeof (objCtlDontFragmentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlDontFragment
*
* @purpose Set 'CtlDontFragment'
*
* @description [CtlDontFragment]: This object enables setting of the don't
*              fragment flag (DF) in the IP header for a probe. Use of this
*              object enables performing a manual PATH MTU test. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlDontFragment (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlDontFragmentValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  L7_ushort16 handle;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlDontFragment */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlDontFragmentValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlDontFragmentValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
  if (usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteMaxFailSet(handle,objCtlDontFragmentValue);
  }
  else
  {
    owa.l7rc  = L7_FAILURE;
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlInitialTtl
*
* @purpose Get 'CtlInitialTtl'
*
* @description [CtlInitialTtl]: The value of this object specifies the initial
*              TTL value to use. This enables bypassing the initial (often
*              well known) portion of a path. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlInitialTtl (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlInitialTtlValue;

  L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objCtlInitialTtlValue = initTtl;
  /* return the object value: CtlInitialTtl */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlInitialTtlValue,
                           sizeof (objCtlInitialTtlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlInitialTtl
*
* @purpose Set 'CtlInitialTtl'
*
* @description [CtlInitialTtl]: The value of this object specifies the initial
*              TTL value to use. This enables bypassing the initial (often
*              well known) portion of a path. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlInitialTtl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlInitialTtlValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  L7_ushort16 handle;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlInitialTtl */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlInitialTtlValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlInitialTtlValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
   if (usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteInitTtlSet(handle,objCtlInitialTtlValue);
  }
  else
  {
    owa.l7rc  = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlFrequency
*
* @purpose Get 'CtlFrequency'
*
* @description [CtlFrequency]: The number of seconds to wait before repeating
*              a traceroute test as defined by the value of the various
*              objects in the corresponding row. The number of hops in a
*              single traceroute test is determined by the value of the corresponding
*              traceRouteCtlProbesPerHop object. After a single
*              test completes the number of seconds as defined by the value
*              of traceRouteCtlFrequency MUST elapse before the next
*              traceroute test is started. A value of 0 for this object implies
*              that the test as defined by the corresponding entry will
*              not be repeated. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlFrequency (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlFrequencyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbTraceRouteResultGet (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              &objCtlFrequencyValue);*/
  objCtlFrequencyValue = 0;                            
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlFrequency */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlFrequencyValue,
                           sizeof (objCtlFrequencyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlFrequency
*
* @purpose Set 'CtlFrequency'
*
* @description [CtlFrequency]: The number of seconds to wait before repeating
*              a traceroute test as defined by the value of the various
*              objects in the corresponding row. The number of hops in a
*              single traceroute test is determined by the value of the corresponding
*              traceRouteCtlProbesPerHop object. After a single
*              test completes the number of seconds as defined by the value
*              of traceRouteCtlFrequency MUST elapse before the next
*              traceroute test is started. A value of 0 for this object implies
*              that the test as defined by the corresponding entry will
*              not be repeated. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlFrequency (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlFrequencyValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlFrequency */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCtlFrequencyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlFrequencyValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlStorageType
*
* @purpose Get 'CtlStorageType'
*
* @description [CtlStorageType]: The storage type for this conceptual row.
*              Conceptual rows having the value 'permanent' need not allow
*              write-access to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlStorageType (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlStorageType
*
* @purpose Set 'CtlStorageType'
*
* @description [CtlStorageType]: The storage type for this conceptual row.
*              Conceptual rows having the value 'permanent' need not allow
*              write-access to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlStorageType (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlStorageTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlStorageType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlStorageTypeValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlAdminStatus
*
* @purpose Get 'CtlAdminStatus'
*
* @description [CtlAdminStatus]: Reflects the desired state that an traceRouteCtlEntry
*              should be in: enabled(1) - Attempt to activate
*              the test as defined by this traceRouteCtlEntry. disabled(2)
*              - Deactivate the test as defined by this traceRouteCtlEntry.
*              Refer to the corresponding traceRouteResultsOperStatus
*              to determine the operational state of the test defined by this
*              entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlAdminStatus (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlAdminStatusValue;
    L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if(operStatus == L7_TRUE)
  	objCtlAdminStatusValue = L7_ENABLE;
  else
  	objCtlAdminStatusValue = L7_DISABLE;
  
  /* return the object value: CtlAdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlAdminStatusValue,
                           sizeof (objCtlAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlAdminStatus
*
* @purpose Set 'CtlAdminStatus'
*
* @description [CtlAdminStatus]: Reflects the desired state that an traceRouteCtlEntry
*              should be in: enabled(1) - Attempt to activate
*              the test as defined by this traceRouteCtlEntry. disabled(2)
*              - Deactivate the test as defined by this traceRouteCtlEntry.
*              Refer to the corresponding traceRouteResultsOperStatus
*              to determine the operational state of the test defined by this
*              entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlAdminStatus (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlAdminStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlAdminStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlAdminStatusValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = fpObjUtil_basetraceRouteCtrlConfig_adminStatusSet(keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              objCtlAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlDescr
*
* @purpose Get 'CtlDescr'
*
* @description [CtlDescr]: The purpose of this object is to provide a descriptive
*              name of the remote traceroute test. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlDescr (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlDescr
*
* @purpose Set 'CtlDescr'
*
* @description [CtlDescr]: The purpose of this object is to provide a descriptive
*              name of the remote traceroute test. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlDescr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCtlDescrValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlDescr */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objCtlDescrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objCtlDescrValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_MaxRows
*
* @purpose Get 'MaxRows'
*
* @description [MaxRows]: The maximum number of entries allowed in the traceRouteProbeHistoryTable.
*              An implementation of this MIB will
*              remove the oldest entry in the traceRouteProbeHistoryTable
*              to allow the addition of an new entry once the number of
*              rows in the traceRouteProbeHistoryTable reaches this value.
*              Old entries are not removed when a new test is started. Entries
*              are added to the traceRouteProbeHistoryTable until traceRouteCtlMaxRows
*              is reached before entries begin to be removed.
*              A value of 0 for this object disables creation of traceRouteProbeHistoryTable
*              entries. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_MaxRows (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxRowsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbTraceRouteResultGet (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue, &objMaxRowsValue);*/
  objMaxRowsValue = 50;
  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MaxRows */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxRowsValue,
                           sizeof (objMaxRowsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_MaxRows
*
* @purpose Set 'MaxRows'
*
* @description [MaxRows]: The maximum number of entries allowed in the traceRouteProbeHistoryTable.
*              An implementation of this MIB will
*              remove the oldest entry in the traceRouteProbeHistoryTable
*              to allow the addition of an new entry once the number of
*              rows in the traceRouteProbeHistoryTable reaches this value.
*              Old entries are not removed when a new test is started. Entries
*              are added to the traceRouteProbeHistoryTable until traceRouteCtlMaxRows
*              is reached before entries begin to be removed.
*              A value of 0 for this object disables creation of traceRouteProbeHistoryTable
*              entries. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_MaxRows (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxRowsValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MaxRows */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMaxRowsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxRowsValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlTrapGeneration
*
* @purpose Get 'CtlTrapGeneration'
*
* @description [CtlTrapGeneration]: The value of this object determines when
*              and if to to generate a notification for this entry: pathChange(0)
*              - Generate a traceRoutePathChange notification when
*              the current path varies from a previously determined path.
*              testFailure(1) - Generate a traceRouteTestFailed notification
*              when the full path to a target can't be determined.
*              testCompletion(2) - Generate a traceRouteTestCompleted notification
*              when the path to a target has been determined. The
*              value of this object defaults to zero, indicating that none
*              of the above options have been selected. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlTrapGeneration (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

 owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlTrapGeneration
*
* @purpose Set 'CtlTrapGeneration'
*
* @description [CtlTrapGeneration]: The value of this object determines when
*              and if to to generate a notification for this entry: pathChange(0)
*              - Generate a traceRoutePathChange notification when
*              the current path varies from a previously determined path.
*              testFailure(1) - Generate a traceRouteTestFailed notification
*              when the full path to a target can't be determined.
*              testCompletion(2) - Generate a traceRouteTestCompleted notification
*              when the path to a target has been determined. The
*              value of this object defaults to zero, indicating that none
*              of the above options have been selected. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlTrapGeneration (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCtlTrapGenerationValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlTrapGeneration */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objCtlTrapGenerationValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objCtlTrapGenerationValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlCreateHopsEntries
*
* @purpose Get 'CtlCreateHopsEntries'
*
* @description [CtlCreateHopsEntries]: The current path for a traceroute test
*              is kept in the traceRouteHopsTable on a per hop basis when
*              the value of this object is true(1). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlCreateHopsEntries (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlCreateHopsEntriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  /*owa.l7rc =
    usmDbTraceRouteResultGet (L7_UNIT_CURRENT, keytraceRouteCtlOwnerIndexValue,
                              keytraceRouteCtlTestNameValue,
                              &objCtlCreateHopsEntriesValue);*/
  objCtlCreateHopsEntriesValue = L7_FALSE; 
  owa.l7rc  = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: CtlCreateHopsEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCtlCreateHopsEntriesValue,
                           sizeof (objCtlCreateHopsEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlCreateHopsEntries
*
* @purpose Set 'CtlCreateHopsEntries'
*
* @description [CtlCreateHopsEntries]: The current path for a traceroute test
*              is kept in the traceRouteHopsTable on a per hop basis when
*              the value of this object is true(1). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlCreateHopsEntries (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCtlCreateHopsEntriesValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlCreateHopsEntries */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCtlCreateHopsEntriesValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCtlCreateHopsEntriesValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

 owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_CtlType
*
* @purpose Get 'CtlType'
*
* @description [CtlType]: The value of this object is used either to report
*              or select the implementation method to be used for performing
*              a traceroute operation. The value of this object may be
*              selected from traceRouteImplementationTypeDomains. Additional
*              implementation types should be allocated as required by
*              implementers of the DISMAN-TRACEROUTE-MIB under their enterprise
*              specific registration point and not beneath traceRouteImplementationTypeDomains.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_CtlType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_CtlType
*
* @purpose Set 'CtlType'
*
* @description [CtlType]: The value of this object is used either to report
*              or select the implementation method to be used for performing
*              a traceroute operation. The value of this object may be
*              selected from traceRouteImplementationTypeDomains. Additional
*              implementation types should be allocated as required by
*              implementers of the DISMAN-TRACEROUTE-MIB under their enterprise
*              specific registration point and not beneath traceRouteImplementationTypeDomains.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_CtlType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objCtlTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CtlType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objCtlTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objCtlTypeValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basetraceRouteCtrlConfig_TraceRouteCtrlRowStatus
*
* @purpose Get 'TraceRouteCtrlRowStatus'
*
* @description [TraceRouteCtrlRowStatus]: This object allows entries to be
*              created and deleted         in the traceRouteCtlTable.  Deletion
*              of an entry in         this table results in all corresponding
*              (same traceRouteCtlOwnerIndex and traceRouteCtlTestName
*               index values) traceRouteResultsTable, traceRouteProbeHistoryTable,
*              and traceRouteHopsTable entries being deleted.
*              A value MUST be specified for traceRouteCtlTargetAddress
*              prior to a transition to active(1) state being         accepted.
*                      Activation of a remote traceroute operation
*              is         controlled via traceRouteCtlAdminStatus and not
*                      by transitioning of this object's value to active(1).
*                      Transitions in and out of active(1) state are
*              not         allowed while an entry's traceRouteResultsOperStatus
*                      is active(1) with the exception that deletion
*              of         an entry in this table by setting its RowStatus
*                      object to destroy(6) will stop an active  traceroute
*              operation.The operational state of an traceroute operation
*               can be determined by examination of the corresponding traceRouteResultsOperStatus
*              object. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteCtrlConfig_TraceRouteCtrlRowStatus (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTraceRouteCtrlRowStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  L7_ushort16 handle;
  L7_BOOL operStatus;
  L7_ushort16 vrfId;
  L7_uint32 ipDa; 
  L7_ushort16 probeSize;
  L7_ushort16 probePerHop; 
  L7_ushort16 probeInterval;
  L7_BOOL dontFrag;
  L7_ushort16 port; 
  L7_ushort16 maxTtl;
  L7_ushort16 initTtl;
  L7_ushort16 maxFail;

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_TRACE_ROUTE_OWNER_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_TRACE_ROUTE_TEST_NAME_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  owa.l7rc =
    usmDbTraceRouteResultGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle, &operStatus, &vrfId,
                                 &ipDa, &probeSize, &probePerHop, &probeInterval, &dontFrag, &port,
                                 &maxTtl, &initTtl, &maxFail);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

 if(ipDa != 0)
    objTraceRouteCtrlRowStatusValue = L7_ROW_STATUS_ACTIVE;
 else
    objTraceRouteCtrlRowStatusValue = L7_ROW_STATUS_CREATE_AND_GO;

  /* return the object value: TraceRouteCtrlRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTraceRouteCtrlRowStatusValue,
                           sizeof (objTraceRouteCtrlRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basetraceRouteCtrlConfig_TraceRouteCtrlRowStatus
*
* @purpose Set 'TraceRouteCtrlRowStatus'
*
* @description [TraceRouteCtrlRowStatus]: This object allows entries to be
*              created and deleted         in the traceRouteCtlTable.  Deletion
*              of an entry in         this table results in all corresponding
*              (same traceRouteCtlOwnerIndex and traceRouteCtlTestName
*               index values) traceRouteResultsTable, traceRouteProbeHistoryTable,
*              and traceRouteHopsTable entries being deleted.
*              A value MUST be specified for traceRouteCtlTargetAddress
*              prior to a transition to active(1) state being         accepted.
*                      Activation of a remote traceroute operation
*              is         controlled via traceRouteCtlAdminStatus and not
*                      by transitioning of this object's value to active(1).
*                      Transitions in and out of active(1) state are
*              not         allowed while an entry's traceRouteResultsOperStatus
*                      is active(1) with the exception that deletion
*              of         an entry in this table by setting its RowStatus
*                      object to destroy(6) will stop an active  traceroute
*              operation.The operational state of an traceroute operation
*               can be determined by examination of the corresponding traceRouteResultsOperStatus
*              object. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basetraceRouteCtrlConfig_TraceRouteCtrlRowStatus (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTraceRouteCtrlRowStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  FPOBJ_TRACE_ENTER (bufp);

    L7_ushort16 handle;

  /* retrieve object: TraceRouteCtrlRowStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objTraceRouteCtrlRowStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTraceRouteCtrlRowStatusValue, owa.len);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_TRACE_ROUTE_OWNER_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: traceRouteCtlTestName */
  kwa2.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteCtrlConfig_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_TRACE_ROUTE_TEST_NAME_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objTraceRouteCtrlRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    /*owa.l7rc = usmDbTraceRouteAlloc( keytraceRouteCtlOwnerIndexValue,
                         keytraceRouteCtlTestNameValue, syncFlag, NULL, contextInfo, &handle ); */

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_TRACE_ROUTE_OWNER_ADD_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objTraceRouteCtrlRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue,
                                      keytraceRouteCtlTestNameValue, &handle);
      if(owa.l7rc == L7_SUCCESS)
        owa.l7rc  = usmDbTraceRouteFree( handle );

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_TRACE_ROUTE_OWNER_DEL_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}

