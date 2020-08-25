/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseTraceRouteHopStats.c
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
#include "_xe_baseTraceRouteHopStats_obj.h"
#include "usmdb_traceroute_api.h"
#include "traceroute_exports.h"

/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_traceRouteCtlOwnerIndex
*
* @purpose Get 'traceRouteCtlOwnerIndex'
*
* @description [traceRouteCtlOwnerIndex] To facilitate the provisioning of access control by a security administrator using the View-Based Access Control Model (RFC 2575, VACM) for tables in which multiple users may need to independently create or modify entries, the initial index is used as an 'owner index'. Such an initial index has a syntax of SnmpAdminString, and can thus be trivially mapped to a securityName or groupName as defined in VACM, in accordance with a security policy. When used in conjunction with such a security policy all entries in the table belonging to a particular user (or group) will have the same value for this initial index. For a given user's entries in a particular table, the object identifiers for the information in these entries will have the same subidentifiers (except for the 'column' subidentifier) up to the end of the encoded owner index. To configure VACM to permit access to this portion of the table, one would create vacmViewTreeFamilyTable entries with the value of vacmViewTreeFamilySubtree including the owner index portion, and vacmViewTreeFamilyMask 'wildcarding' the column subidentifier. More elaborate configurations are possible.
*
* @*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_traceRouteCtlOwnerIndex (void *wap, void *bufp)
{
  xLibStr256_t objtraceRouteCtlOwnerIndexValue;
  xLibStr256_t nextObjtraceRouteCtlOwnerIndexValue;
  xLibStr256_t objtraceRouteCtlTestNameValue;
  L7_ushort16 handle;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
                          (xLibU8_t *) objtraceRouteCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objtraceRouteCtlOwnerIndexValue, 0x00, sizeof(objtraceRouteCtlOwnerIndexValue));
    memset(objtraceRouteCtlTestNameValue, 0x00, sizeof(objtraceRouteCtlTestNameValue));
    owa.l7rc = usmDbTraceRouteSessionHandleGetNext (nextObjtraceRouteCtlOwnerIndexValue, 
                                                    objtraceRouteCtlTestNameValue,
                                                    &handle);
   
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objtraceRouteCtlOwnerIndexValue, owa.len);
    memcpy(nextObjtraceRouteCtlOwnerIndexValue,objtraceRouteCtlOwnerIndexValue,sizeof(nextObjtraceRouteCtlOwnerIndexValue));
    do
    {
      memset(objtraceRouteCtlTestNameValue, 0x00, sizeof(objtraceRouteCtlTestNameValue));
      owa.l7rc = usmDbTraceRouteSessionHandleGetNext (nextObjtraceRouteCtlOwnerIndexValue,
                                                      objtraceRouteCtlTestNameValue,&handle);
      
    }
    while ((strcmp(objtraceRouteCtlOwnerIndexValue, nextObjtraceRouteCtlOwnerIndexValue) == 0)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjtraceRouteCtlOwnerIndexValue, owa.len);

  /* return the object value: traceRouteCtlOwnerIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjtraceRouteCtlOwnerIndexValue,
                           strlen (objtraceRouteCtlOwnerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_traceRouteCtlTestName
*
* @purpose Get 'traceRouteCtlTestName'
*
* @description [traceRouteCtlTestName] The name of a traceroute test. This is locally unique, within the scope of an traceRouteCtlOwnerIndex.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_traceRouteCtlTestName (void *wap, void *bufp)
{

  xLibStr256_t objtraceRouteCtlOwnerIndexValue;
  xLibStr256_t objtraceRouteCtlTestNameValue;
  xLibStr256_t nextObjtraceRouteCtlTestNameValue;
  L7_ushort16 handle;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
                          (xLibU8_t *) objtraceRouteCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objtraceRouteCtlOwnerIndexValue, owa.len);

  /* retrieve key: traceRouteCtlTestName */
  owa.rc = xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                          (xLibU8_t *) objtraceRouteCtlTestNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objtraceRouteCtlTestNameValue, 0x00, sizeof(objtraceRouteCtlTestNameValue));
    owa.l7rc = usmDbTraceRouteSessionHandleGetNext (objtraceRouteCtlOwnerIndexValue,
                                                    nextObjtraceRouteCtlTestNameValue,
                                                    &handle);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objtraceRouteCtlTestNameValue, owa.len);

    owa.l7rc = usmDbTraceRouteSessionHandleGetNext (objtraceRouteCtlOwnerIndexValue,
                                                    nextObjtraceRouteCtlTestNameValue,
                                                    &handle);

  }

  if ((strcmp(objtraceRouteCtlTestNameValue, nextObjtraceRouteCtlTestNameValue) != 0)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjtraceRouteCtlTestNameValue, owa.len);

  /* return the object value: traceRouteCtlTestName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjtraceRouteCtlTestNameValue,
                           strlen (objtraceRouteCtlTestNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_traceRouteHopsHopIndex
*
* @purpose Get 'traceRouteHopsHopIndex'
*
* @description [traceRouteHopsHopIndex] Specifies the hop index for a traceroute hop. Values for this object with respect to the same traceRouteCtlOwnerIndex and traceRouteCtlTestName MUST start at 1 and increase monotonically. The traceRouteHopsTable keeps the current traceroute path per traceRouteCtlEntry if enabled by setting the corresponding traceRouteCtlCreateHopsEntries to true(1). All hops (traceRouteHopsTable entries) in a traceroute path MUST be updated at the same time when a traceroute operation completes. Care needs to be applied when either a path changes or can't be determined. The initial portion of the path, up to the first hop change, MUST retain the same traceRouteHopsHopIndex values. The remaining portion of the path SHOULD be assigned new traceRouteHopsHopIndex values.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_traceRouteHopsHopIndex (void *wap, void *bufp)
{

  fpObjWa_t kwatraceRouteCtlOwnerIndex = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwatraceRouteCtlTestName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  L7_ushort16 handle,ttl;
  L7_uint32 hopDa, minRtt, maxRtt, avgRtt;
  L7_ushort16 probeSent, probeRecvd ;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objtraceRouteHopsHopIndexValue;
  xLibU32_t nextObjtraceRouteHopsHopIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwatraceRouteCtlOwnerIndex.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) keytraceRouteCtlOwnerIndexValue, &kwatraceRouteCtlOwnerIndex.len);
  if (kwatraceRouteCtlOwnerIndex.rc != XLIBRC_SUCCESS)
  {
    kwatraceRouteCtlOwnerIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwatraceRouteCtlOwnerIndex);
    return kwatraceRouteCtlOwnerIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlOwnerIndexValue, kwatraceRouteCtlOwnerIndex.len);

  /* retrieve key: traceRouteCtlTestName */
  kwatraceRouteCtlTestName.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwatraceRouteCtlTestName.len);
  if (kwatraceRouteCtlTestName.rc != XLIBRC_SUCCESS)
  {
    kwatraceRouteCtlTestName.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwatraceRouteCtlTestName);
    return kwatraceRouteCtlTestName.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwatraceRouteCtlTestName.len);

  /* retrieve key: traceRouteHopsHopIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                          (xLibU8_t *) & objtraceRouteHopsHopIndexValue, &owa.len);
 
  if(usmDbTraceRouteHandleGet(keytraceRouteCtlOwnerIndexValue,keytraceRouteCtlTestNameValue,&handle)== L7_SUCCESS)
  {

   nextObjtraceRouteHopsHopIndexValue=objtraceRouteHopsHopIndexValue;
    if (owa.rc != XLIBRC_SUCCESS)
   {
      FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
      owa.l7rc = usmDbTraceRouteHopGetFirst(handle,(L7_ushort16*)&nextObjtraceRouteHopsHopIndexValue, &ttl,
                                      &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
                                      &probeRecvd);

 
   }
   else
   {
      FPOBJ_TRACE_CURRENT_KEY (bufp, &objtraceRouteHopsHopIndexValue, owa.len);
      owa.l7rc = usmDbTraceRouteHopGetNext(handle,(L7_ushort16*)&nextObjtraceRouteHopsHopIndexValue, &ttl,
                                      &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
                                      &probeRecvd);
   }
  }
  else
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjtraceRouteHopsHopIndexValue, owa.len);

  /* return the object value: traceRouteHopsHopIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjtraceRouteHopsHopIndexValue,
                           sizeof (objtraceRouteHopsHopIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
xLibRC_t fpObjUtil_baseTraceRouteHopStats_IndexNextGet (L7_char8  *ownerIndex,
	                    L7_char8 *nameIndex,  L7_ushort16 *hopIndex)
{

  L7_ushort16 ttl;
  L7_uint32 hopDa;
  L7_uint32 minRtt;   
  L7_uint32 maxRtt;
  L7_uint32 avgRtt;
  L7_ushort16 probeSent;
   L7_ushort16 probeRecvd;
   L7_ushort16 handle;
   L7_ushort16 tempHopIndex = *hopIndex;

    if (usmDbTraceRouteHandleGet( ownerIndex, nameIndex, &handle)== L7_SUCCESS)
    {
      if ((usmDbTraceRouteHopGet( handle, tempHopIndex, &ttl,
              &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
              &probeRecvd ) != L7_SUCCESS) &&
          (usmDbTraceRouteHopGetNext( handle,hopIndex, &ttl,
                                      &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
                                      &probeRecvd) != L7_SUCCESS))
      {
        if (usmDbTraceRouteSessionHandleGetNext( ownerIndex, nameIndex, &handle)== L7_SUCCESS)
        {
          tempHopIndex = 0;
	   hopIndex = 0;	  
          if( (usmDbTraceRouteHopGet( handle,tempHopIndex, &ttl,
                  &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
                  &probeRecvd ) != L7_SUCCESS))
          {
            return XLIBRC_FAILURE;
          }
        }  
        else 
        {
          return XLIBRC_FAILURE;
        }
      } 
    }
    else if (usmDbTraceRouteSessionHandleGetNext( ownerIndex, nameIndex, &handle)== L7_SUCCESS)
    {
      if ((usmDbTraceRouteHopGet( handle,tempHopIndex, &ttl,
              &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
              &probeRecvd ) != L7_SUCCESS) &&
          (usmDbTraceRouteHopGetNext( handle,hopIndex, &ttl,
                                      &hopDa, &minRtt,&maxRtt, &avgRtt, &probeSent,
                                      &probeRecvd) != L7_SUCCESS))
      {
        return XLIBRC_FAILURE;
      }
    }
    else
    {
      return XLIBRC_FAILURE;
    }
    return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_traceRouteCtlOwnerIndex_traceRouteCtlTestName_traceRouteHopsHopIndex
*
* @purpose Get 'traceRouteCtlOwnerIndex + traceRouteCtlTestName + traceRouteHopsHopIndex +'
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
*              [traceRouteHopsHopIndex]: Specifies the hop index for a traceroute
*              hop. Values for this object with respect to the same
*              traceRouteCtlOwnerIndex and traceRouteCtlTestName MUST start
*              at 1 and increase monotonically. The traceRouteHopsTable
*              keeps the current traceroute path per traceRouteCtlEntry
*              if enabled by setting the corresponding traceRouteCtlCreateHopsEntries
*              to true(1). All hops (traceRouteHopsTable entries)
*              in a traceroute path MUST be updated at the same time when
*              a traceroute operation completes. Care needs to be applied
*              when either a path changes or can't be determined. The
*              initial portion of the path, up to the first hop change, MUST
*              retain the same traceRouteHopsHopIndex values. The remaining
*              portion of the path SHOULD be assigned new traceRouteHopsHopIndex
*              values. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_baseTraceRouteHopStats_traceRouteCtlOwnerIndex_traceRouteCtlTestName_traceRouteHopsHopIndex
(void *wap, void *bufp[], xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owatraceRouteCtlOwnerIndex = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtraceRouteCtlOwnerIndexValue,
    nextObjtraceRouteCtlOwnerIndexValue;
  fpObjWa_t owatraceRouteCtlTestName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objtraceRouteCtlTestNameValue, nextObjtraceRouteCtlTestNameValue;
  fpObjWa_t owatraceRouteHopsHopIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objtraceRouteHopsHopIndexValue, nextObjtraceRouteHopsHopIndexValue;
  void *outtraceRouteCtlOwnerIndex = (void *) bufp[--keyCount];
  void *outtraceRouteCtlTestName = (void *) bufp[--keyCount];
  void *outtraceRouteHopsHopIndex = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outtraceRouteCtlOwnerIndex);
  FPOBJ_TRACE_ENTER (outtraceRouteCtlTestName);
  FPOBJ_TRACE_ENTER (outtraceRouteHopsHopIndex);

  /* retrieve key: traceRouteCtlOwnerIndex */
  owatraceRouteCtlOwnerIndex.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
                   (xLibU8_t *) objtraceRouteCtlOwnerIndexValue,
                   &owatraceRouteCtlOwnerIndex.len);
  if (owatraceRouteCtlOwnerIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: traceRouteCtlTestName */
    owatraceRouteCtlTestName.rc =
      xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                     (xLibU8_t *) objtraceRouteCtlTestNameValue,
                     &owatraceRouteCtlTestName.len);
    if (owatraceRouteCtlTestName.rc == XLIBRC_SUCCESS)
    {
      /* retrieve key: traceRouteHopsHopIndex */
      owatraceRouteHopsHopIndex.rc =
        xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                       (xLibU8_t *) & objtraceRouteHopsHopIndexValue,
                       &owatraceRouteHopsHopIndex.len);
    }
  }
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteCtlOwnerIndex,
                           &objtraceRouteCtlOwnerIndexValue,
                           owatraceRouteCtlOwnerIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteCtlTestName,
                           &objtraceRouteCtlTestNameValue,
                           owatraceRouteCtlTestName.len);
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteHopsHopIndex,
                           &objtraceRouteHopsHopIndexValue,
                           owatraceRouteHopsHopIndex.len);
  owa.rc = 
  	fpObjUtil_baseTraceRouteHopStats_IndexNextGet (objtraceRouteCtlOwnerIndexValue,
                         objtraceRouteCtlTestNameValue,
                         (xLibU16_t *)&objtraceRouteHopsHopIndexValue);
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outtraceRouteCtlOwnerIndex, owatraceRouteCtlOwnerIndex);
    FPOBJ_TRACE_EXIT (outtraceRouteCtlTestName, owatraceRouteCtlTestName);
    FPOBJ_TRACE_EXIT (outtraceRouteHopsHopIndex, owatraceRouteHopsHopIndex);
    return owa.rc;
  }
  strcpy(nextObjtraceRouteCtlOwnerIndexValue,objtraceRouteCtlOwnerIndexValue);
  strcpy(nextObjtraceRouteCtlTestNameValue,objtraceRouteCtlTestNameValue);
  nextObjtraceRouteHopsHopIndexValue = objtraceRouteHopsHopIndexValue;
  
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteCtlOwnerIndex,
                           &nextObjtraceRouteCtlOwnerIndexValue,
                           owatraceRouteCtlOwnerIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteCtlTestName,
                           &nextObjtraceRouteCtlTestNameValue,
                           owatraceRouteCtlTestName.len);
  FPOBJ_TRACE_CURRENT_KEY (outtraceRouteHopsHopIndex,
                           &nextObjtraceRouteHopsHopIndexValue,
                           owatraceRouteHopsHopIndex.len);

  /* return the object value: traceRouteCtlOwnerIndex */
  xLibBufDataSet (outtraceRouteCtlOwnerIndex,
                  (xLibU8_t *) nextObjtraceRouteCtlOwnerIndexValue,
                  strlen (nextObjtraceRouteCtlOwnerIndexValue));

  /* return the object value: traceRouteCtlTestName */
  xLibBufDataSet (outtraceRouteCtlTestName,
                  (xLibU8_t *) nextObjtraceRouteCtlTestNameValue,
                  strlen (nextObjtraceRouteCtlTestNameValue));

  /* return the object value: traceRouteHopsHopIndex */
  xLibBufDataSet (outtraceRouteHopsHopIndex,
                  (xLibU8_t *) & nextObjtraceRouteHopsHopIndexValue,
                  sizeof (nextObjtraceRouteHopsHopIndexValue));
  FPOBJ_TRACE_EXIT (outtraceRouteCtlOwnerIndex, owatraceRouteCtlOwnerIndex);
  FPOBJ_TRACE_EXIT (outtraceRouteCtlTestName, owatraceRouteCtlTestName);
  FPOBJ_TRACE_EXIT (outtraceRouteHopsHopIndex, owatraceRouteHopsHopIndex);
  return XLIBRC_SUCCESS;
}

#endif
/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_HopsIpTgtAddressType
*
* @purpose Get 'HopsIpTgtAddressType'
*
* @description [HopsIpTgtAddressType]: This objects indicates the type of
*              address stored in the corresponding traceRouteHopsIpTargetAddress
*              object. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_HopsIpTgtAddressType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keytraceRouteHopsHopIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHopsIpTgtAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* retrieve key: traceRouteHopsHopIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                   (xLibU8_t *) & keytraceRouteHopsHopIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytraceRouteHopsHopIndexValue, kwa3.len);

  /* get the value from application */
  objHopsIpTgtAddressTypeValue = 1;  /*always supports ipv4 */
  owa.l7rc = L7_SUCCESS ;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HopsIpTgtAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHopsIpTgtAddressTypeValue,
                           sizeof (objHopsIpTgtAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_HopsIpTgtAddress
*
* @purpose Get 'HopsIpTgtAddress'
*
* @description [HopsIpTgtAddress]: This object reports the IP address associated
*              with the hop. A value for this object should be reported
*              as a numeric IP address and not as a DNS name. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_HopsIpTgtAddress (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keytraceRouteHopsHopIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objHopsIpTgtAddressValue;

  L7_ushort16 ttl;
  L7_uint32 hopDa;
  L7_uint32 minRtt;   
  L7_uint32 maxRtt;
  L7_uint32 avgRtt;
  L7_ushort16 probeSent;
   L7_ushort16 probeRecvd;
   L7_ushort16 handle;
   L7_RC_t rc;
   
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* retrieve key: traceRouteHopsHopIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                   (xLibU8_t *) & keytraceRouteHopsHopIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytraceRouteHopsHopIndexValue, kwa3.len);

  /* get the value from application */
  rc = usmDbTraceRouteHandleGet(keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle);
  if(rc != L7_SUCCESS)
  {
    if (owa.l7rc != L7_SUCCESS)
   {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  owa.l7rc =
    usmDbTraceRouteHopGet (handle,keytraceRouteHopsHopIndexValue,
                           &ttl, &hopDa, &minRtt, &maxRtt, &avgRtt, &probeSent,&probeRecvd );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   osapiInetNtoa(hopDa, objHopsIpTgtAddressValue);

  /* return the object value: HopsIpTgtAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objHopsIpTgtAddressValue,
                           strlen (objHopsIpTgtAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_HopsMinRtt
*
* @purpose Get 'HopsMinRtt'
*
* @description [HopsMinRtt]: The minimum traceroute round-trip-time (RTT)
*              received for this hop. A value of 0 for this object implies
*              that no RTT has been received. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_HopsMinRtt (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keytraceRouteHopsHopIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHopsMinRttValue;

  L7_ushort16 ttl;
  L7_uint32 hopDa;
  L7_uint32 minRtt;   
  L7_uint32 maxRtt;
  L7_uint32 avgRtt;
  L7_ushort16 probeSent;
  L7_ushort16 probeRecvd;
  L7_ushort16 handle;
  L7_RC_t rc;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* retrieve key: traceRouteHopsHopIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                   (xLibU8_t *) & keytraceRouteHopsHopIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytraceRouteHopsHopIndexValue, kwa3.len);

  /* Get the value from application */

  rc = usmDbTraceRouteHandleGet(keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle);
  if(rc != L7_SUCCESS)
  {
    if (owa.l7rc != L7_SUCCESS)
   {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  owa.l7rc =
    usmDbTraceRouteHopGet (handle,keytraceRouteHopsHopIndexValue,
                           &ttl, &hopDa, &minRtt, &maxRtt, &avgRtt, &probeSent,&probeRecvd );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objHopsMinRttValue = minRtt/TRACEROUTE_RTT_MULTIPLIER;
  
  /* return the object value: HopsMinRtt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHopsMinRttValue,
                           sizeof (objHopsMinRttValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_HopsMaxRtt
*
* @purpose Get 'HopsMaxRtt'
*
* @description [HopsMaxRtt]: The maximum traceroute round-trip-time (RTT)
*              received for this hop. A value of 0 for this object implies
*              that no RTT has been received. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_HopsMaxRtt (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keytraceRouteHopsHopIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHopsMaxRttValue;

  L7_ushort16 ttl;
  L7_uint32 hopDa;
  L7_uint32 minRtt;   
  L7_uint32 maxRtt;
  L7_uint32 avgRtt;
  L7_ushort16 probeSent;
  L7_ushort16 probeRecvd;
  L7_ushort16 handle;
  L7_RC_t rc;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* retrieve key: traceRouteHopsHopIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                   (xLibU8_t *) & keytraceRouteHopsHopIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytraceRouteHopsHopIndexValue, kwa3.len);

  /* get the value from application */
  rc = usmDbTraceRouteHandleGet(keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle);
  if(rc != L7_SUCCESS)
  {
    if (owa.l7rc != L7_SUCCESS)
   {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  owa.l7rc =
    usmDbTraceRouteHopGet (handle,keytraceRouteHopsHopIndexValue,
                           &ttl, &hopDa, &minRtt, &maxRtt, &avgRtt, &probeSent,&probeRecvd );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objHopsMaxRttValue = maxRtt/TRACEROUTE_RTT_MULTIPLIER;

  /* return the object value: HopsMaxRtt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHopsMaxRttValue,
                           sizeof (objHopsMaxRttValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_HopsAverageRtt
*
* @purpose Get 'HopsAverageRtt'
*
* @description [HopsAverageRtt]: The current average traceroute round-trip-time
*              (RTT) for this hop. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_HopsAverageRtt (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keytraceRouteHopsHopIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHopsAverageRttValue;

    L7_ushort16 ttl;
  L7_uint32 hopDa;
  L7_uint32 minRtt;   
  L7_uint32 maxRtt;
  L7_uint32 avgRtt;
  L7_ushort16 probeSent;
  L7_ushort16 probeRecvd;
  L7_ushort16 handle;
  L7_RC_t rc;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* retrieve key: traceRouteHopsHopIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                   (xLibU8_t *) & keytraceRouteHopsHopIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytraceRouteHopsHopIndexValue, kwa3.len);

  /* get the value from application */
  rc = usmDbTraceRouteHandleGet(keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle);
  if(rc != L7_SUCCESS)
  {
    if (owa.l7rc != L7_SUCCESS)
   {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  owa.l7rc =
    usmDbTraceRouteHopGet (handle,keytraceRouteHopsHopIndexValue,
                           &ttl, &hopDa, &minRtt, &maxRtt, &avgRtt, &probeSent,&probeRecvd );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objHopsAverageRttValue = avgRtt/TRACEROUTE_RTT_MULTIPLIER;
  
  /* return the object value: HopsAverageRtt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHopsAverageRttValue,
                           sizeof (objHopsAverageRttValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_HopsRttSumOfSquares
*
* @purpose Get 'HopsRttSumOfSquares'
*
* @description [HopsRttSumOfSquares]: This object contains the sum of all
*              traceroute responses received for this hop. Its purpose is
*              to enable standard deviation calculation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_HopsRttSumOfSquares (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keytraceRouteHopsHopIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* retrieve key: traceRouteHopsHopIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                   (xLibU8_t *) & keytraceRouteHopsHopIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytraceRouteHopsHopIndexValue, kwa3.len);

  /* get the value from application */
 owa.rc = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_HopsSentProbes
*
* @purpose Get 'HopsSentProbes'
*
* @description [HopsSentProbes]: The value of this object reflects the number
*              of probes sent for this hop during this traceroute test.
*              The value of this object should start at 0. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_HopsSentProbes (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keytraceRouteHopsHopIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHopsSentProbesValue;

  L7_ushort16 ttl;
  L7_uint32 hopDa;
  L7_uint32 minRtt;   
  L7_uint32 maxRtt;
  L7_uint32 avgRtt;
  L7_ushort16 probeSent;
  L7_ushort16 probeRecvd;
  L7_ushort16 handle;
  L7_RC_t rc;  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* retrieve key: traceRouteHopsHopIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                   (xLibU8_t *) & keytraceRouteHopsHopIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytraceRouteHopsHopIndexValue, kwa3.len);

  /* get the value from application */

  rc = usmDbTraceRouteHandleGet(keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle);
  if(rc != L7_SUCCESS)
  {
    if (owa.l7rc != L7_SUCCESS)
   {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  owa.l7rc =
    usmDbTraceRouteHopGet (handle,keytraceRouteHopsHopIndexValue,
                           &ttl, &hopDa, &minRtt, &maxRtt, &avgRtt, &probeSent,&probeRecvd );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objHopsSentProbesValue = probeSent;

  /* return the object value: HopsSentProbes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHopsSentProbesValue,
                           sizeof (objHopsSentProbesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_HopsProbeResponses
*
* @purpose Get 'HopsProbeResponses'
*
* @description [HopsProbeResponses]: Number of responses received for this
*              hop during this traceroute test. This value of this object
*              should start at 0. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_HopsProbeResponses (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keytraceRouteHopsHopIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHopsProbeResponsesValue;

  L7_ushort16 ttl;
  L7_uint32 hopDa;
  L7_uint32 minRtt;   
  L7_uint32 maxRtt;
  L7_uint32 avgRtt;
  L7_ushort16 probeSent;
  L7_ushort16 probeRecvd;
  L7_ushort16 handle;
  L7_RC_t rc;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* retrieve key: traceRouteHopsHopIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                   (xLibU8_t *) & keytraceRouteHopsHopIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytraceRouteHopsHopIndexValue, kwa3.len);

  /* get the value from application */
    rc = usmDbTraceRouteHandleGet(keytraceRouteCtlOwnerIndexValue, keytraceRouteCtlTestNameValue, &handle);
  if(rc != L7_SUCCESS)
  {
    if (owa.l7rc != L7_SUCCESS)
   {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  owa.l7rc =
    usmDbTraceRouteHopGet (handle,keytraceRouteHopsHopIndexValue,
                           &ttl, &hopDa, &minRtt, &maxRtt, &avgRtt, &probeSent,&probeRecvd );
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objHopsProbeResponsesValue = probeRecvd;

  /* return the object value: HopsProbeResponses */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHopsProbeResponsesValue,
                           sizeof (objHopsProbeResponsesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseTraceRouteHopStats_HopsLastGoodProbe
*
* @purpose Get 'HopsLastGoodProbe'
*
* @description [HopsLastGoodProbe]: Date and time was the last response was
*              received for a probe for this hop during this traceroute
*              test. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTraceRouteHopStats_HopsLastGoodProbe (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keytraceRouteCtlTestNameValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keytraceRouteHopsHopIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: traceRouteCtlOwnerIndex */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlOwnerIndex,
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
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteCtlTestName,
                   (xLibU8_t *) keytraceRouteCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keytraceRouteCtlTestNameValue, kwa2.len);

  /* retrieve key: traceRouteHopsHopIndex */
  kwa3.rc =
    xLibFilterGet (wap, XOBJ_baseTraceRouteHopStats_traceRouteHopsHopIndex,
                   (xLibU8_t *) & keytraceRouteHopsHopIndexValue, &kwa3.len);
  if (kwa3.l7rc != L7_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keytraceRouteHopsHopIndexValue, kwa3.len);

  owa.rc  = XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

