
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basecableTestCopperSumm.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  29 October 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Vijayanand.K 
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_basecableTestCopperSumm_obj.h"
#include "usmdb_status.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"

/*******************************************************************************
* @function fpObjGet_basecableTestCopperSumm_copperInterface
*
* @purpose Get 'copperInterface'
 *@description  [copperInterface] <HTML>Interface Number of the Cable Test
* performed   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestCopperSumm_copperInterface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcopperInterfaceValue;
  xLibU32_t nextObjcopperInterfaceValue;

  xLibU32_t connType;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: copperInterface */
  owa.len = sizeof (objcopperInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestCopperSumm_copperInterface,
                          (xLibU8_t *) & objcopperInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcopperInterfaceValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objcopperInterfaceValue, owa.len);
  owa.l7rc = usmDbValidIntIfNumNext (objcopperInterfaceValue,
                                    &nextObjcopperInterfaceValue);

  /* Loop through all the interfaces and skip interfaces other than Copper */
  while(owa.l7rc == L7_SUCCESS) 
  {
    if (( usmDbIntfConnectorTypeGet(nextObjcopperInterfaceValue, &connType) == L7_SUCCESS ) &&
          ( connType == L7_RJ45 ) )
    {  
      break;
    }
    objcopperInterfaceValue = nextObjcopperInterfaceValue;
    owa.l7rc = usmDbValidIntIfNumNext (objcopperInterfaceValue,
                                    &nextObjcopperInterfaceValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcopperInterfaceValue, owa.len);

  /* return the object value: copperInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcopperInterfaceValue,
                           sizeof (nextObjcopperInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basecableTestCopperSumm_TestCopperTrigger
*
* @purpose Set 'TestCopperTrigger'
 *@description  [TestCopperTrigger] <HTML>Cable Test copper    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basecableTestCopperSumm_TestCopperTrigger (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTestCopperTriggerValue;

  xLibU32_t keycopperInterfaceValue;

  /* All these parameters are needed for this routine. Hence pass them as dummy arguments */
  L7_CABLE_STATUS_t dummyCableStatus;
  L7_BOOL dummyLengthKnown;
  xLibU32_t dummyShortestLength;
  xLibU32_t dummyLongestLength;
  xLibU32_t dummyCableFailureLength;
  L7_clocktime dummyTimeStamp;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TestCopperTrigger */
  owa.len = sizeof (objTestCopperTriggerValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTestCopperTriggerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTestCopperTriggerValue, owa.len);

  /* retrieve key: copperInterface */
  owa.len = sizeof (keycopperInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestCopperSumm_copperInterface,
                          (xLibU8_t *) & keycopperInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycopperInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbStatusCableGet (keycopperInterfaceValue, &dummyCableStatus,
                                  &dummyLengthKnown,&dummyShortestLength,
                                  &dummyLongestLength,&dummyCableFailureLength,
                                  &dummyTimeStamp);  


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestCopperSumm_CableFailureLength
*
* @purpose Get 'CableFailureLength'
 *@description  [CableFailureLength] <HTML>Failure length of the given interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestCopperSumm_CableFailureLength (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCableFailureLengthValue;

  xLibU32_t keycopperInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* All these parameters are needed for this routine. Hence pass them as dummy arguments */
  L7_CABLE_STATUS_t dummyCableStatus;
  L7_BOOL dummyLengthKnown;
  xLibU32_t dummyShortestLength;
  xLibU32_t dummyLongestLength;
  L7_clocktime dummyTimeStamp;

  /* retrieve key: copperInterface */
  owa.len = sizeof (keycopperInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestCopperSumm_copperInterface,
                          (xLibU8_t *) & keycopperInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycopperInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusCopperResult (keycopperInterfaceValue,&dummyCableStatus,
                                           &dummyLengthKnown,&dummyShortestLength,
                                           &dummyLongestLength,&objCableFailureLengthValue,
                                           &dummyTimeStamp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objCableFailureLengthValue, sizeof (objCableFailureLengthValue));

  /* return the object value: CableFailureLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCableFailureLengthValue,
                           sizeof (objCableFailureLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestCopperSumm_CableStatus
*
* @purpose Get 'CableStatus'
 *@description  [CableStatus] <HTML>Copper Cable Status of the given Interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestCopperSumm_CableStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCableStatusValue;

  xLibU32_t keycopperInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* All these parameters are needed for this routine. Hence pass them as dummy arguments */
  L7_BOOL dummyLengthKnown;
  xLibU32_t dummyShortestLength;
  xLibU32_t dummyLongestLength;
  xLibU32_t dummyCableFailureLength;
  L7_clocktime dummyTimeStamp;

  /* retrieve key: copperInterface */
  owa.len = sizeof (keycopperInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestCopperSumm_copperInterface,
                          (xLibU8_t *) & keycopperInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycopperInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusCopperResult (keycopperInterfaceValue,&objCableStatusValue,
                                           &dummyLengthKnown,&dummyShortestLength,
                                           &dummyLongestLength,&dummyCableFailureLength,
                                           &dummyTimeStamp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objCableStatusValue, sizeof (objCableStatusValue));

  /* return the object value: CableStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCableStatusValue, sizeof (objCableStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestCopperSumm_LengthKnown
*
* @purpose Get 'LengthKnown'
 *@description  [LengthKnown] <HTML>Boolean value whether the length is known or
* not   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestCopperSumm_LengthKnown (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLengthKnownValue;

  xLibU32_t keycopperInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* All these parameters are needed for this routine. Hence pass them as dummy arguments */
  L7_CABLE_STATUS_t dummyCableStatus;
  xLibU32_t dummyShortestLength;
  xLibU32_t dummyLongestLength;
  xLibU32_t dummyCableFailureLength;
  L7_clocktime dummyTimeStamp;

  /* retrieve key: copperInterface */
  owa.len = sizeof (keycopperInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestCopperSumm_copperInterface,
                          (xLibU8_t *) & keycopperInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycopperInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusCopperResult (keycopperInterfaceValue,&dummyCableStatus,
                                           &objLengthKnownValue,&dummyShortestLength,
                                           &dummyLongestLength,&dummyCableFailureLength,
                                           &dummyTimeStamp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objLengthKnownValue, sizeof (objLengthKnownValue));

  /* return the object value: LengthKnown */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLengthKnownValue, sizeof (objLengthKnownValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestCopperSumm_LongestLength
*
* @purpose Get 'LongestLength'
 *@description  [LongestLength] <HTML>Longest Lengh of the given Copper Interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestCopperSumm_LongestLength (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLongestLengthValue;

  xLibU32_t keycopperInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* All these parameters are needed for this routine. Hence pass them as dummy arguments */
  L7_CABLE_STATUS_t dummyCableStatus;
  L7_BOOL dummyLengthKnown;
  xLibU32_t dummyShortestLength;
  xLibU32_t dummyCableFailureLength;
  L7_clocktime dummyTimeStamp;

  /* retrieve key: copperInterface */
  owa.len = sizeof (keycopperInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestCopperSumm_copperInterface,
                          (xLibU8_t *) & keycopperInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycopperInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusCopperResult (keycopperInterfaceValue,&dummyCableStatus,
                                           &dummyLengthKnown,&dummyShortestLength,
                                           &objLongestLengthValue,&dummyCableFailureLength,
                                           &dummyTimeStamp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objLongestLengthValue, sizeof (objLongestLengthValue));

  /* return the object value: LongestLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLongestLengthValue,
                           sizeof (objLongestLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestCopperSumm_ShortestLength
*
* @purpose Get 'ShortestLength'
 *@description  [ShortestLength] <HTML>Shortest Length of the copper cable for
* given interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestCopperSumm_ShortestLength (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objShortestLengthValue;
  xLibStr256_t pairShortLong;
  xLibU32_t keycopperInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* All these parameters are needed for this routine. Hence pass them as dummy arguments */
  L7_CABLE_STATUS_t dummyCableStatus;
  L7_BOOL dummyLengthKnown;
  xLibU32_t dummyLongestLength;
  xLibU32_t dummyCableFailureLength;
  L7_clocktime dummyTimeStamp;

  /* retrieve key: copperInterface */
  owa.len = sizeof (keycopperInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestCopperSumm_copperInterface,
                          (xLibU8_t *) & keycopperInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycopperInterfaceValue, owa.len);

   /* get the value from application */
  owa.l7rc = usmDbCableStatusCopperResult (keycopperInterfaceValue,&dummyCableStatus,
                                           &dummyLengthKnown,&objShortestLengthValue,
                                           &dummyLongestLength,&dummyCableFailureLength,
                                           &dummyTimeStamp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* AS the UI (both CLI and Web) needs combination of shortest and
   * longest length together we are combining two objects in one. 
   * This need to be chaned once we have support from tool for pairing up 
   * objects. 
   */
  FPOBJ_TRACE_VALUE (bufp, &objShortestLengthValue, sizeof (objShortestLengthValue));

  memset(pairShortLong,0x00,sizeof(pairShortLong));
  if(!dummyLengthKnown)
  {
    sprintf(pairShortLong,"%s", "Unknown"); 
  }
  else
  {
   /* Check whether the cable is present or not */
    if (dummyCableStatus == L7_CABLE_NORMAL)
    {
      sprintf(pairShortLong, "%u - %u",objShortestLengthValue,dummyLongestLength);
    }
  }

  /* return the object value: ShortestLength */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  pairShortLong,strlen (pairShortLong));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestCopperSumm_TimeStamp
*
* @purpose Get 'TimeStamp'
 *@description  [TimeStamp] <HTML>Time stamp of the Cable Test Performed   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestCopperSumm_TimeStamp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objTimeStampValue;

  xLibU32_t keycopperInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* All these parameters are needed for this routine. Hence pass them as dummy arguments */
  L7_CABLE_STATUS_t dummyCableStatus;
  L7_BOOL dummyLengthKnown;
  xLibU32_t dummyShortestLength;
  xLibU32_t dummyLongestLength;
  xLibU32_t dummyCableFailureLength;
  L7_clocktime TimeStamp;
 
 /* retrieve key: copperInterface */
  owa.len = sizeof (keycopperInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestCopperSumm_copperInterface,
                          (xLibU8_t *) & keycopperInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycopperInterfaceValue, owa.len);

  /* get the value from application */
   owa.l7rc = usmDbCableStatusCopperResult (keycopperInterfaceValue,&dummyCableStatus,
                                           &dummyLengthKnown,&dummyShortestLength,
                                           &dummyLongestLength,&dummyCableFailureLength,
                                           &TimeStamp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memset(objTimeStampValue,0x00,sizeof(objTimeStampValue));
  
  osapiStrncpySafe(objTimeStampValue, (const char *)usmDbConvertTimeToDateString(TimeStamp.seconds),sizeof(objTimeStampValue));

  FPOBJ_TRACE_VALUE (bufp, objTimeStampValue, strlen (objTimeStampValue));

  /* return the object value: TimeStamp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTimeStampValue, strlen (objTimeStampValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
