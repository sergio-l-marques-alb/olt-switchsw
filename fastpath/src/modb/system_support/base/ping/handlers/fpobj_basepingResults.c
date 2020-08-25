/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basepingResults.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  19 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basepingResults_obj.h"
#include "usmdb_ping_api.h"


/*******************************************************************************
* @function fpObjGet_basepingResults_pingCtlOwnerIndex
*
* @purpose Get 'pingCtlOwnerIndex'
*
* @description [pingCtlOwnerIndex] To facilitate the provisioning of access control by a security administrator using the View-Based Access Control Model (RFC 2575, VACM) for tables in which multiple users may need to independently create or modify entries, the initial index is used as an 'owner index'. Such an initial index has a syntax of SnmpAdminString, and can thus be trivially mapped to a securityName or groupName as defined in VACM, in accordance with a security policy. When used in conjunction with such a security policy all entries in the table belonging to a particular user (or group) will have the same value for this initial index. For a given user's entries in a particular table, the object identifiers for the information in these entries will have the same subidentifiers (except for the 'column' subidentifier) up to the end of the encoded owner index. To configure VACM to permit access to this portion of the table, one would create vacmViewTreeFamilyTable entries with the value of vacmViewTreeFamilySubtree including the owner index portion, and vacmViewTreeFamilyMask 'wildcarding' the column subidentifier. More elaborate configurations are possible.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_pingCtlOwnerIndex (void *wap, void *bufp)
{

  xLibStr256_t objpingCtlOwnerIndexValue;
  xLibStr256_t nextObjpingCtlOwnerIndexValue;
  xLibStr256_t objpingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  xLibU16_t handle;
  L7_BOOL operStatus;
  xLibU32_t vrfId;
  xLibU32_t ipDa;
  xLibU16_t count;
  xLibU16_t size;
  xLibU16_t interval;
  
  handle = 0;
  operStatus = XLIB_FALSE;
  vrfId = 0;
  ipDa = 0;
  count = 0;
  size = 0;
  interval = 0;
  
  /* retrieve key: pingCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                          (xLibU8_t *) objpingCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objpingCtlOwnerIndexValue, 0x00, sizeof(objpingCtlOwnerIndexValue));  
    memset(objpingCtlTestNameValue, 0x00, sizeof(objpingCtlTestNameValue)); 
    owa.l7rc = usmDbPingSessionGetNext( objpingCtlOwnerIndexValue, 
  	    objpingCtlTestNameValue, &handle,
           &operStatus, &vrfId, &ipDa, &count, &size, &interval );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpingCtlOwnerIndexValue, owa.len);
    strcpy(nextObjpingCtlOwnerIndexValue,objpingCtlOwnerIndexValue);
    do
    {
      memset(objpingCtlTestNameValue, 0x00, sizeof(objpingCtlTestNameValue));
      owa.l7rc = usmDbPingSessionGetNext( objpingCtlOwnerIndexValue, 
  	      objpingCtlTestNameValue, &handle,
             &operStatus, &vrfId, &ipDa, &count, &size, &interval );
    }
    while ((strcmp(objpingCtlOwnerIndexValue,nextObjpingCtlOwnerIndexValue) == 0)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  strcpy(nextObjpingCtlOwnerIndexValue,objpingCtlOwnerIndexValue);
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpingCtlOwnerIndexValue, owa.len);

  /* return the object value: pingCtlOwnerIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpingCtlOwnerIndexValue,
                           strlen (objpingCtlOwnerIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basepingResults_pingCtlTestName
*
* @purpose Get 'pingCtlTestName'
*
* @description [pingCtlTestName] The name of the ping test. This is locally unique, within the scope of an pingCtlOwnerIndex.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_pingCtlTestName (void *wap, void *bufp)
{

  xLibStr256_t objpingCtlOwnerIndexValue;
  xLibStr256_t nextObjpingCtlOwnerIndexValue;
  xLibStr256_t objpingCtlTestNameValue;
  xLibStr256_t nextObjpingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  xLibU16_t handle;
  L7_BOOL operStatus;
  xLibU32_t vrfId;
  xLibU32_t ipDa;
  xLibU16_t count;
  xLibU16_t size;
  xLibU16_t interval;
  
  handle = 0;
  operStatus = XLIB_FALSE;
  vrfId = 0;
  ipDa = 0;
  count = 0;
  size = 0;
  interval = 0;
  
  /* retrieve key: pingCtlOwnerIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                          (xLibU8_t *) objpingCtlOwnerIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objpingCtlOwnerIndexValue, owa.len);

  /* retrieve key: pingCtlTestName */
  owa.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlTestName,
                          (xLibU8_t *) objpingCtlTestNameValue, &owa.len);
  strcpy(nextObjpingCtlOwnerIndexValue,objpingCtlOwnerIndexValue);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objpingCtlTestNameValue, 0x00, sizeof(objpingCtlTestNameValue));
    owa.l7rc = usmDbPingSessionGetNext( objpingCtlOwnerIndexValue, 
  	    objpingCtlTestNameValue, &handle,
           &operStatus, &vrfId, &ipDa, &count, &size, &interval );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpingCtlTestNameValue, owa.len);

    owa.l7rc = usmDbPingSessionGetNext( objpingCtlOwnerIndexValue, 
  	    objpingCtlTestNameValue, &handle,
           &operStatus, &vrfId, &ipDa, &count, &size, &interval );

  }

  if ((strcmp(objpingCtlOwnerIndexValue, nextObjpingCtlOwnerIndexValue) != 0) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  strcpy(nextObjpingCtlTestNameValue,objpingCtlTestNameValue);
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpingCtlTestNameValue, owa.len);

  /* return the object value: pingCtlTestName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpingCtlTestNameValue,
                           strlen (objpingCtlTestNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_basepingResults_pingCtlOwnerIndex_pingCtlTestName
*
* @purpose Get 'pingCtlOwnerIndex + pingCtlTestName +'
*
* @description [pingCtlOwnerIndex]: To facilitate the provisioning of access
*              control by a security administrator using the View-Based
*              Access Control Model (RFC 2575, VACM) for tables in which
*              multiple users may need to independently create or modify entries,
*              the initial index is used as an 'owner index'. Such
*              an initial index has a syntax of SnmpAdminString, and can
*              thus be trivially mapped to a securityName or groupName as
*              defined in VACM, in accordance with a security policy. When
*              used in conjunction with such a security policy all entries
*              in the table belonging to a particular user (or group) will
*              have the same value for this initial index. For a given
*              user's entries in a particular table, the object identifiers
*              for the information in these entries will have the same subidentifiers
*              (except for the 'column' subidentifier) up to
*              the end of the encoded owner index. To configure VACM to permit
*              access to this portion of the table, one would create
*              vacmViewTreeFamilyTable entries with the value of vacmViewTreeFamilySubtree
*              including the owner index portion, and vacmViewTreeFamilyMask
*              'wildcarding' the column subidentifier.
*              More elaborate configurations are possible. 
*              [pingCtlTestName]: The name of the ping test. This is locally
*              unique, within the scope of an pingCtlOwnerIndex. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_pingCtlOwnerIndex_pingCtlTestName (void *wap,
                                                                     void
                                                                     *bufp[],
                                                                     xLibU16_t
                                                                     keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owapingCtlOwnerIndex = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpingCtlOwnerIndexValue, nextObjpingCtlOwnerIndexValue;
  fpObjWa_t owapingCtlTestName = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpingCtlTestNameValue, nextObjpingCtlTestNameValue;
  void *outpingCtlOwnerIndex = (void *) bufp[--keyCount];
  void *outpingCtlTestName = (void *) bufp[--keyCount];

  xLibU16_t handle;
  L7_BOOL operStatus;
  xLibU32_t vrfId;
  xLibU32_t ipDa;
  xLibU16_t count;
  xLibU16_t size;
  xLibU16_t interval;
  
  handle = 0;
  operStatus = XLIB_FALSE;
  vrfId = 0;
  ipDa = 0;
  count = 0;
  size = 0;
  interval = 0;
  
  FPOBJ_TRACE_ENTER (outpingCtlOwnerIndex);
  FPOBJ_TRACE_ENTER (outpingCtlTestName);

  /* retrieve key: pingCtlOwnerIndex */
  owapingCtlOwnerIndex.rc =
    xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                   (xLibU8_t *) objpingCtlOwnerIndexValue,
                   &owapingCtlOwnerIndex.len);
  if (owapingCtlOwnerIndex.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: pingCtlTestName */
    owapingCtlTestName.rc =
      xLibFilterGet (wap, XOBJ_basepingResults_pingCtlTestName,
                     (xLibU8_t *) objpingCtlTestNameValue,
                     &owapingCtlTestName.len);
  }
  FPOBJ_TRACE_CURRENT_KEY (outpingCtlOwnerIndex, &objpingCtlOwnerIndexValue,
                           owapingCtlOwnerIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outpingCtlTestName, &objpingCtlTestNameValue,
                           owapingCtlTestName.len);
  
  owa.rc = usmDbPingSessionGetNext( objpingCtlOwnerIndexValue, 
  	  objpingCtlTestNameValue, &handle,
         &operStatus, &vrfId, &ipDa, &count, &size, &interval );
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outpingCtlOwnerIndex, owapingCtlOwnerIndex);
    FPOBJ_TRACE_EXIT (outpingCtlTestName, owapingCtlTestName);
    return owa.rc;
  }
  strcpy(nextObjpingCtlOwnerIndexValue,objpingCtlOwnerIndexValue);
  strcpy(nextObjpingCtlTestNameValue,objpingCtlTestNameValue);
  
  FPOBJ_TRACE_CURRENT_KEY (outpingCtlOwnerIndex, &nextObjpingCtlOwnerIndexValue,
                           owapingCtlOwnerIndex.len);
  FPOBJ_TRACE_CURRENT_KEY (outpingCtlTestName, &nextObjpingCtlTestNameValue,
                           owapingCtlTestName.len);

  /* return the object value: pingCtlOwnerIndex */
  xLibBufDataSet (outpingCtlOwnerIndex,
                  (xLibU8_t *) nextObjpingCtlOwnerIndexValue,
                  strlen (nextObjpingCtlOwnerIndexValue));

  /* return the object value: pingCtlTestName */
  xLibBufDataSet (outpingCtlTestName,
                  (xLibU8_t *) nextObjpingCtlTestNameValue,
                  strlen (nextObjpingCtlTestNameValue));
  FPOBJ_TRACE_EXIT (outpingCtlOwnerIndex, owapingCtlOwnerIndex);
  FPOBJ_TRACE_EXIT (outpingCtlTestName, owapingCtlTestName);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_basepingResults_OperStatus
*
* @purpose Get 'OperStatus'
*
* @description [OperStatus]: Reflects the operational state of a pingCtlEntry:
*              enabled(1) - Test is active. disabled(2) - Test has stopped.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_OperStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOperStatusValue;
  L7_BOOL tempStatus;
  xLibU16_t handle;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc =
      usmDbPingQueryOperStatus (handle, &tempStatus);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if(tempStatus == L7_TRUE)
    {
       objOperStatusValue = L7_ENABLE;
    }
    else
    {
       objOperStatusValue = L7_DISABLE;
    }
	
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value: OperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOperStatusValue,
                           sizeof (objOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingResults_IpTargetAddress
*
* @purpose Get 'IpTargetAddress'
*
* @description [IpTargetAddress]: This objects reports the IP address associated
*              with a pingCtlTargetAddress value when the destination
*              address is specified as a DNS name. The value of this object
*              should be a zero length octet string when a DNS name is
*              not specified or when a specified DNS name fails to resolve.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_IpTargetAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpTargetAddressValue;
  xLibU16_t handle;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc =
      usmDbPingQueryTargetAddrs (handle,&objIpTargetAddressValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value: IpTargetAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpTargetAddressValue,
                           sizeof (objIpTargetAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basepingResults_MinRtt
*
* @purpose Get 'MinRtt'
*
* @description [MinRtt]: The minimum ping round-trip-time (RTT) received.
*              A value of 0 for this object implies that no RTT has been received.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_MinRtt (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMinRttValue;
  xLibU16_t handle;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbPingQueryMinRtt (handle, &objMinRttValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value: MinRtt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMinRttValue,
                           sizeof (objMinRttValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingResults_MaxRtt
*
* @purpose Get 'MaxRtt'
*
* @description [MaxRtt]: The maximum ping round-trip-time (RTT) received.
*              A value of 0 for this object implies that no RTT has been received.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_MaxRtt (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxRttValue;
  xLibU16_t handle;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc = usmDbPingQueryMaxRtt (handle, &objMaxRttValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }
  

  /* return the object value: MaxRtt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxRttValue,
                           sizeof (objMaxRttValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingResults_AverageRtt
*
* @purpose Get 'AverageRtt'
*
* @description [AverageRtt]: The current average ping round-trip-time (RTT).
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_AverageRtt (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAverageRttValue;
  xLibU16_t handle;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */
  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
     owa.l7rc = usmDbPingQueryAvgRtt (handle, &objAverageRttValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value: AverageRtt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAverageRttValue,
                           sizeof (objAverageRttValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingResults_ProbeResponses
*
* @purpose Get 'ProbeResponses'
*
* @description [ProbeResponses]: Number of responses received for the corresponding
*              pingCtlEntry and pingResultsEntry. The value of this
*              object MUST be reported as 0 when no probe responses have
*              been received. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_ProbeResponses (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProbeResponsesValue;
  xLibU16_t handle;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
     owa.l7rc =
      usmDbPingQueryProbeResponse (handle,&objProbeResponsesValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* return the object value: ProbeResponses */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objProbeResponsesValue,
                           sizeof (objProbeResponsesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingResults_SentProbes
*
* @purpose Get 'SentProbes'
*
* @description [SentProbes]: The value of this object reflects the number
*              of probes sent for the corresponding pingCtlEntry and pingResultsEntry.
*              The value of this object MUST be reported as 0
*              when no probes have been sent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_SentProbes (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlOwnerIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keypingCtlTestNameValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSentProbesValue;
  xLibU16_t handle;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pingCtlOwnerIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlOwnerIndex,
                           (xLibU8_t *) keypingCtlOwnerIndexValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlOwnerIndexValue, kwa1.len);

  /* retrieve key: pingCtlTestName */
  kwa2.rc = xLibFilterGet (wap, XOBJ_basepingResults_pingCtlTestName,
                           (xLibU8_t *) keypingCtlTestNameValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keypingCtlTestNameValue, kwa2.len);

  /* get the value from application */

  if(usmDbPingSessionHandleFromIndexTestNameGet(keypingCtlOwnerIndexValue, keypingCtlTestNameValue, &handle) == L7_SUCCESS)
  {
    owa.l7rc =
      usmDbPingQueryProbeSent (handle, &objSentProbesValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
      owa.l7rc = L7_FAILURE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }
  

  /* return the object value: SentProbes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSentProbesValue,
                           sizeof (objSentProbesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basepingResults_pingMaxConcurrentRequests
*
* @purpose Get 'pingMaxConcurrentRequests'
*
* @description [pingMaxConcurrentRequests]: The maximum number of concurrent
*              active ping requests that are allowed within an agent implementation.
*              A value of 0 for this object implies that there
*              is no limit for the number of concurrent active requests
*              in effect. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basepingResults_pingMaxConcurrentRequests (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpingMaxConcurrentRequestsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbMaxPingSessionsGet (
                             &objpingMaxConcurrentRequestsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpingMaxConcurrentRequestsValue,
                     sizeof (objpingMaxConcurrentRequestsValue));

  /* return the object value: pingMaxConcurrentRequests */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objpingMaxConcurrentRequestsValue,
                    sizeof (objpingMaxConcurrentRequestsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
