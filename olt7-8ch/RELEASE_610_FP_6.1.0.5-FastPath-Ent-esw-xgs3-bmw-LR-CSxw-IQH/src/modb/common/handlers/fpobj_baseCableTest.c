
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseCableTest.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  17 April 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Radha K
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseCableTest_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_status.h" 

/*******************************************************************************
* @function fpObjGet_baseCableTest_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface] The Interface to be used.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCableTest_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_baseCableTest_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objInterfaceValue = 0;
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF,
                                 0, &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF,
                                               0, objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  { 
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjInterfaceValue, sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseCableTest_Status
*
* @purpose Get 'Status'
*
* @description [Status] This displays the cable status as Normal, Open or Short.Normal: the cable is working correctly.Open: the cable is disconnected or there is a faulty connector. Short: there is an electrical short in the cable.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCableTest_Status (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  /*L7_uint32 cableStatus;*/
  L7_uint32 shortestLength;
  L7_uint32 longestLength;
  L7_uint32 cableFailureLength;
  L7_BOOL lengthKnown;
  L7_clocktime dummyTimeStamp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseCableTest_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatusCableGet (keyInterfaceValue,&objStatusValue,
                                  &lengthKnown, &shortestLength,
                                  &longestLength, &cableFailureLength ,&dummyTimeStamp);
                                   
  if ((owa.l7rc != L7_SUCCESS) || (objStatusValue == L7_CABLE_TEST_FAIL))
  {
    objStatusValue = L7_CABLE_TEST_FAIL;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objStatusValue, sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return XLIBRC_SUCCESS;

}

/*******************************************************************************
* @function fpObjGet_baseCableTest_Length
*
* @purpose Get 'Length'
*
* @description [Length] The estimated length of the cable in meters. The length is displayed as a range between the shortest estimated length and the longest estimated length. Unknown is displayedif the cable length could not be determined. The Cable Length is only displayed if the cable status is Normal. This field is displayed after the Test Cable button has been clicked and results are available. This field is not visible when the page is initially displayed.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCableTest_Length (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objLengthValue = {0};
  L7_uint32 cableStatus;
  L7_uint32 shortestLength;
  L7_uint32 longestLength;
  L7_uint32 cableFailureLength;
  L7_BOOL lengthKnown;
  L7_clocktime dummyTimeStamp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseCableTest_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatusCableGet (keyInterfaceValue,&cableStatus,
                                  &lengthKnown, &shortestLength,
                                  &longestLength, &cableFailureLength,&dummyTimeStamp); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ((cableStatus == L7_CABLE_NORMAL))
  {  
    if (lengthKnown == L7_TRUE)
    {
      osapiSnprintf(objLengthValue, sizeof(objLengthValue), "%um - %um", shortestLength, longestLength);
    }
    else
    {
      osapiSnprintf(objLengthValue, sizeof(objLengthValue), "Unknown");
    }
  } 
     
  /* return the object value: Length */
  owa.rc = xLibBufDataSet (bufp, objLengthValue, strlen(objLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseCableTest_FailureLoc
*
* @purpose Get 'FailureLoc'
*
* @description [FailureLoc] The estimated distance in meters from the end of the cable to the failure location. The failure location is only displayed if the cable status is Open or Short. This field is displayed after the Test Cable button has been clicked and results are available. This field is not visible when the page is initially displayed.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCableTest_FailureLoc (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objFailureLocValue = { 0 };
  L7_uint32 cableStatus;
  L7_uint32 shortestLength;
  L7_uint32 longestLength;
  L7_uint32 cableFailureLength;
  L7_BOOL lengthKnown;
  L7_clocktime dummyTimeStamp;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseCableTest_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbStatusCableGet (keyInterfaceValue,&cableStatus,
                                  &lengthKnown, &shortestLength,
                                  &longestLength, &cableFailureLength,&dummyTimeStamp);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ((cableStatus == L7_CABLE_OPEN ) || (cableStatus == L7_CABLE_SHORT))
  {
    if (lengthKnown == L7_TRUE)
    {
      osapiSnprintf(objFailureLocValue, sizeof(objFailureLocValue), "%um", cableFailureLength);
    }
    else
    {
      osapiSnprintf(objFailureLocValue, sizeof(objFailureLocValue), "Unknown");
    }
  }
  /* return the object value: FailureLoc */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFailureLocValue, strlen(objFailureLocValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
