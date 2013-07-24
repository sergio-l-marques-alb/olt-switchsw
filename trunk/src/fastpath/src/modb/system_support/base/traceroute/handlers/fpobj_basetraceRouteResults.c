/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basetraceRouteResults.c
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
#include "_xe_basetraceRouteResults_obj.h"
#include "usmdb_traceroute_api.h"


/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_traceRouteCtlOwnerIndex
*
* @purpose Get 'traceRouteCtlOwnerIndex'
*
* @description [traceRouteCtlOwnerIndex] To facilitate the provisioning of access control by a security administrator using the View-Based Access Control Model (RFC 2575, VACM) for tables in which multiple users may need to independently create or modify entries, the initial index is used as an 'owner index'. Such an initial index has a syntax of SnmpAdminString, and can thus be trivially mapped to a securityName or groupName as defined in VACM, in accordance with a security policy. When used in conjunction with such a security policy all entries in the table belonging to a particular user (or group) will have the same value for this initial index. For a given user's entries in a particular table, the object identifiers for the information in these entries will have the same subidentifiers (except for the 'column' subidentifier) up to the end of the encoded owner index. To configure VACM to permit access to this portion of the table, one would create vacmViewTreeFamilyTable entries with the value of vacmViewTreeFamilySubtree including the owner index portion, and vacmViewTreeFamilyMask 'wildcarding' the column subidentifier. More elaborate configurations are possible.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_traceRouteCtlOwnerIndex (void *wap, void *bufp)
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
  memset(objtraceRouteCtlOwnerIndexValue, 0x00, sizeof(objtraceRouteCtlOwnerIndexValue));
  memset(nextObjtraceRouteCtlTestNameValue, 0x00, sizeof(nextObjtraceRouteCtlTestNameValue));
  handle = 0;

  /* retrieve key: traceRouteCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
                          (xLibU8_t *) objtraceRouteCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc =
  	  usmDbTraceRouteGetNext( objtraceRouteCtlOwnerIndexValue, objtraceRouteCtlTestNameValue, &handle,
        &operStatus, &vrfId, &ipDa, 
        &probeSize, &probePerHop, 
        &probeInterval, &dontFrag, &port, 
        &maxTtl, &initTtl, &maxFail);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objtraceRouteCtlOwnerIndexValue, owa.len);
    strcpy(nextObjtraceRouteCtlOwnerIndexValue,objtraceRouteCtlOwnerIndexValue);
    do
    {
      
      memset(nextObjtraceRouteCtlTestNameValue, 0x00, sizeof(nextObjtraceRouteCtlTestNameValue));
      owa.l7rc =
  	    usmDbTraceRouteGetNext( objtraceRouteCtlOwnerIndexValue, objtraceRouteCtlTestNameValue, &handle,
          &operStatus, &vrfId, &ipDa, 
          &probeSize, &probePerHop, 
          &probeInterval, &dontFrag, &port, 
          &maxTtl, &initTtl, &maxFail);
    }
    while ((strcmp(objtraceRouteCtlOwnerIndexValue,nextObjtraceRouteCtlOwnerIndexValue)==0)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  strcpy(nextObjtraceRouteCtlOwnerIndexValue,objtraceRouteCtlOwnerIndexValue);
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjtraceRouteCtlOwnerIndexValue, owa.len);

  /* return the object value: traceRouteCtlOwnerIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjtraceRouteCtlOwnerIndexValue,
                           strlen (objtraceRouteCtlOwnerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_traceRouteCtlTestName
*
* @purpose Get 'traceRouteCtlTestName'
*
* @description [traceRouteCtlTestName] The name of a traceroute test. This is locally unique, within the scope of an traceRouteCtlOwnerIndex.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_traceRouteCtlTestName (void *wap, void *bufp)
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
  memset(objtraceRouteCtlOwnerIndexValue, 0x00, sizeof(objtraceRouteCtlOwnerIndexValue));
  memset(nextObjtraceRouteCtlTestNameValue, 0x00, sizeof(nextObjtraceRouteCtlTestNameValue));
  handle = 0;
  /* retrieve key: traceRouteCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
                          (xLibU8_t *) objtraceRouteCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objtraceRouteCtlOwnerIndexValue, owa.len);

  /* retrieve key: traceRouteCtlTestName */
  owa.rc = xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlTestName,
                          (xLibU8_t *) objtraceRouteCtlTestNameValue, &owa.len);

  strcpy(nextObjtraceRouteCtlOwnerIndexValue,objtraceRouteCtlOwnerIndexValue);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(nextObjtraceRouteCtlTestNameValue, 0x00, sizeof(nextObjtraceRouteCtlTestNameValue));
    owa.l7rc =
  	  usmDbTraceRouteGetNext( objtraceRouteCtlOwnerIndexValue, objtraceRouteCtlTestNameValue, &handle,
        &operStatus, &vrfId, &ipDa, 
        &probeSize, &probePerHop, 
        &probeInterval, &dontFrag, &port, 
        &maxTtl, &initTtl, &maxFail);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objtraceRouteCtlTestNameValue, owa.len);

    owa.l7rc =
  	  usmDbTraceRouteGetNext( objtraceRouteCtlOwnerIndexValue, objtraceRouteCtlTestNameValue, &handle,
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
/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_OperStatus
*
* @purpose Get 'OperStatus'
*
* @description [OperStatus]: Reflects the operational state of an traceRouteCtlEntry:
*              enabled(1) - Test is active. disabled(2) - Test
*              has stopped. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_OperStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOperStatusValue;

  L7_ushort16 handle;
  L7_BOOL operStatus; 
  L7_ushort16 currTtl;
  L7_ushort16 currHopCount; 
  L7_ushort16 currProbeCount;
  L7_ushort16 testAttempt; 
  L7_ushort16 testSuccess;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle)== L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteQuery(  handle, &operStatus, 
        &currTtl, &currHopCount, 
        &currProbeCount, &testAttempt, 
        &testSuccess );
    if (owa.l7rc != L7_SUCCESS)
    {  
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(operStatus == L7_TRUE)
    {
      objOperStatusValue = L7_ENABLE;
    }
    else
    {
      objOperStatusValue = L7_DISABLE;
    }

    /* return the object value: OperStatus */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOperStatusValue,
                           sizeof (objOperStatusValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_CurHopCount
*
* @purpose Get 'CurHopCount'
*
* @description [CurHopCount]: Reflects the current TTL value (range from 1
*              to 255) for a remote traceroute operation. Maximum TTL value
*              is determined by traceRouteCtlMaxTtl. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_CurHopCount (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCurHopCountValue;
  
  L7_ushort16 handle;
  L7_BOOL operStatus; 
  L7_ushort16 currTtl;
  L7_ushort16 currHopCount; 
  L7_ushort16 currProbeCount;
  L7_ushort16 testAttempt; 
  L7_ushort16 testSuccess;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  if(usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle)== L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteQuery(  handle, &operStatus, 
        &currTtl, &currHopCount, 
        &currProbeCount, &testAttempt, 
        &testSuccess );
    if (owa.l7rc != L7_SUCCESS)
    {  
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    objCurHopCountValue = currHopCount;

    /* return the object value: CurHopCount */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCurHopCountValue,
                           sizeof (objCurHopCountValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
}

/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_CurProbeCount
*
* @purpose Get 'CurProbeCount'
*
* @description [CurProbeCount]: Reflects the current probe count (1..10) for
*              a remote traceroute operation. The maximum probe count is
*              determined by traceRouteCtlProbesPerHop. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_CurProbeCount (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCurProbeCountValue;

  L7_ushort16 handle;
  L7_BOOL operStatus; 
  L7_ushort16 currTtl;
  L7_ushort16 currHopCount; 
  L7_ushort16 currProbeCount;
  L7_ushort16 testAttempt; 
  L7_ushort16 testSuccess;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle)== L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteQuery(  handle, &operStatus, 
        &currTtl, &currHopCount, 
        &currProbeCount, &testAttempt, 
        &testSuccess );
    if (owa.l7rc != L7_SUCCESS)
    {  
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    objCurProbeCountValue = currProbeCount;	
    /* return the object value: CurProbeCount */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCurProbeCountValue,
                           sizeof (objCurProbeCountValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_IpTgtAddrType
*
* @purpose Get 'IpTgtAddrType'
*
* @description [IpTgtAddrType]: This objects indicates the type of address
*              stored in the corresponding traceRouteResultsIpTgtAddr object.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_IpTgtAddrType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpTgtAddrTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlTestName,
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
  objIpTgtAddrTypeValue = 1;  /*set to ipv4 always */
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IpTgtAddrType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpTgtAddrTypeValue,
                           sizeof (objIpTgtAddrTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_IpTgtAddr
*
* @purpose Get 'IpTgtAddr'
*
* @description [IpTgtAddr]: This objects reports the IP address associated
*              with a traceRouteCtlTargetAddress value when the destination
*              address is specified as a DNS name. The value of this object
*              should be a zero length octet string when a DNS name is
*              not specified or when a specified DNS name fails to resolve.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_IpTgtAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objIpTgtAddrValue;

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
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlTestName,
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
     usmDbTraceRouteResultGet( keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle,
        &operStatus, &vrfId, &ipDa, 
        &probeSize, &probePerHop, 
        &probeInterval, &dontFrag, &port, 
        &maxTtl, &initTtl, &maxFail);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiInetNtoa(ipDa, objIpTgtAddrValue);

  /* return the object value: IpTgtAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIpTgtAddrValue,
                           strlen (objIpTgtAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_TestAttempts
*
* @purpose Get 'TestAttempts'
*
* @description [TestAttempts]: The current number of attempts to determine
*              a path to a target. The value of this object MUST be started
*              at 0. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_TestAttempts (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTestAttemptsValue;
  
  L7_ushort16 handle;
  L7_BOOL operStatus; 
  L7_ushort16 currTtl;
  L7_ushort16 currHopCount; 
  L7_ushort16 currProbeCount;
  L7_ushort16 testAttempt; 
  L7_ushort16 testSuccess;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  if(usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle)== L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteQuery(  handle, &operStatus, 
        &currTtl, &currHopCount, 
        &currProbeCount, &testAttempt, 
        &testSuccess );
    if (owa.l7rc != L7_SUCCESS)
    {  
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    objTestAttemptsValue =  testAttempt;
    /* return the object value: TestAttempts */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTestAttemptsValue,
                           sizeof (objTestAttemptsValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_TestSuccesses
*
* @purpose Get 'TestSuccesses'
*
* @description [TestSuccesses]: The current number of attempts to determine
*              a path to a target that have succeeded. The value of this
*              object MUST be reported as 0 when no attempts have succeeded.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_TestSuccesses (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTestSuccessesValue;

  L7_ushort16 handle;
  L7_BOOL operStatus; 
  L7_ushort16 currTtl;
  L7_ushort16 currHopCount; 
  L7_ushort16 currProbeCount;
  L7_ushort16 testAttempt; 
  L7_ushort16 testSuccess;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* get the value from application */
  if(usmDbTraceRouteHandleGet( keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle)== L7_SUCCESS)
  {
    owa.l7rc = usmDbTraceRouteQuery(  handle, &operStatus, 
        &currTtl, &currHopCount, 
        &currProbeCount, &testAttempt, 
        &testSuccess );
    if (owa.l7rc != L7_SUCCESS)
    {  
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    objTestSuccessesValue = testSuccess;
    /* return the object value: TestSuccesses */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTestSuccessesValue,
                           sizeof (objTestSuccessesValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
}


/*******************************************************************************
* @function fpObjGet_basetraceRouteResults_LastGoodPath
*
* @purpose Get 'LastGoodPath'
*
* @description [LastGoodPath]: The date and time when the last complete path
*              was determined. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_LastGoodPath (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_basetraceRouteResults_traceRouteCtlTestName,
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
* @function fpObjGet_basetraceRouteResults_MaxConcurrentRequests
*
* @purpose Get 'MaxConcurrentRequests'
*
* @description [MaxConcurrentRequests]: The maximum number of concurrent active
*              traceroute requests that are allowed within an agent
*              implementation. A value of 0 for this object implies that there
*              is no limit for the number of concurrent active requests
*              in effect. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basetraceRouteResults_MaxConcurrentRequests (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxConcurrentRequestsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbTraceRouteMaxSessionsGet (&objMaxConcurrentRequestsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxConcurrentRequestsValue,
                     sizeof (objMaxConcurrentRequestsValue));

  /* return the object value: MaxConcurrentRequests */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxConcurrentRequestsValue,
                           sizeof (objMaxConcurrentRequestsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
