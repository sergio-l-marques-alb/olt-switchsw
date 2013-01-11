/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basePortTest.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  16 September 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Sengottuvelan Srirangan 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basePortTest_obj.h"
#ifdef L7_DOT3AH_PACKAGE
#include "usmdb_dot3ah.h"
#endif
#include "usmdb_util_api.h"
#include "usmdb_common.h"
#include "usmdb_nim_api.h"
#include "usmdb_counters_api.h"

xLibU32_t portLoopbackPassValue=L7_FALSE;

/*******************************************************************************
* @function fpObjGet_basePortTest_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortTest_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);

  /* get the value in application */
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet (L7_UNIT_CURRENT, USM_PHYSICAL_INTF, 0,
                                          &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbIntIfNumTypeNextGet (L7_UNIT_CURRENT,USM_PHYSICAL_INTF, 0,
                                         objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (nextObjInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#ifdef L7_DOT3AH_PACKAGE
/*******************************************************************************
* @function fpObjGet_basePortTest_PortTestType
*
* @purpose Get 'PortTestType'
 *@description  [PortTestType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortTest_PortTestType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPortTestTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  kwa.len = sizeof(xLibU32_t);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                          (xLibU8_t *) & keyinterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

 FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinterfaceValue, kwa.len);

  /* retrieve key: PortTestType */
  owa.l7rc = usmDbLanPortTestGet(L7_UNIT_CURRENT, keyinterfaceValue, &objPortTestTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* return the object value: PortTestType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortTestTypeValue,
                           sizeof (objPortTestTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * * @function fpObjGet_basePortTest_PortTestType
 * *
 * * @purpose Set 'ethernetOam'
 *  *@description  [ethernetOam] Enable/Disable dot3ah on a interface.
 *  * @notes
 *  *
 *  * @return
 *  *******************************************************************************/
xLibRC_t fpObjSet_basePortTest_PortTestType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortTestType;

  fpObjWa_t kwainterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinterfaceValue;
  xLibU32_t portLbValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof(xLibU32_t);
  kwainterface.len = sizeof(xLibU32_t);

  /* retrieve object: objPortTestType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPortTestType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortTestType, owa.len);

  /* retrieve key: interface */
  kwainterface.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                                   (xLibU8_t *) & keyinterfaceValue, &kwainterface.len);
  if (kwainterface.rc != XLIBRC_SUCCESS)
  {
    kwainterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwainterface);
    return kwainterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinterfaceValue, kwainterface.len);

  /* set the value in application */
  if(objPortTestType == L7_PORT_LOOPBACK_PHY)
  {
    owa.l7rc = usmDbPortTestEnDisSet(keyinterfaceValue, L7_TRUE);  
    if(owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbLanPortTest(L7_UNIT_CURRENT, keyinterfaceValue,
                                  L7_PORT_LOOPBACK_PHY);

      owa.l7rc = usmDbDot3ahInterfaceEnableGet(L7_UNIT_CURRENT, keyinterfaceValue, &portLbValue);
      if(portLbValue == L7_FALSE)
      {
        owa.l7rc = usmDbPortTestIntfStart(keyinterfaceValue);
      }
      else
      { 
        owa.l7rc = usmDbLanPortTestEvent(L7_UNIT_CURRENT,keyinterfaceValue,10);
      }
      if(owa.l7rc == L7_SUCCESS)
      {
        do
        {
         owa.l7rc = L7_SUCCESS;  
        }while((usmDbLanOamIntfIsEnabled(keyinterfaceValue, &portLbValue) == L7_SUCCESS) &&
              (portLbValue == L7_FALSE));
      }
     portLoopbackPassValue = L7_TRUE;
    }
  }
  else if(objPortTestType == L7_PORT_LOOPBACK_MAC)
  {
#ifndef L7_ROBO_SUPPORT
    owa.l7rc = usmDbPortTestEnDisSet (keyinterfaceValue,L7_TRUE);
    if(owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbLanPortTest(L7_UNIT_CURRENT, keyinterfaceValue,
                                  L7_PORT_LOOPBACK_MAC);
      owa.l7rc = usmDbDot3ahInterfaceEnableGet(L7_UNIT_CURRENT, keyinterfaceValue, &portLbValue);
      if(portLbValue == L7_FALSE)
      { 
        owa.l7rc = usmDbPortTestIntfStart(keyinterfaceValue);
      }
      else
      {
        owa.l7rc = usmDbLanPortTestEvent(L7_UNIT_CURRENT,keyinterfaceValue,10);
      }

      if(owa.l7rc == L7_SUCCESS)
      {
        do
        {
         owa.l7rc = L7_SUCCESS;
        }while((usmDbLanOamIntfIsEnabled(keyinterfaceValue, &portLbValue) == L7_SUCCESS) &&
              (portLbValue == L7_FALSE));
      }
    }
   
    if (owa.l7rc != L7_SUCCESS)
    {
      portLoopbackPassValue = L7_FALSE;
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else
    {
      portLoopbackPassValue = L7_TRUE;
      owa.rc = XLIBRC_SUCCESS;  
    }
#else
      owa.l7rc = L7_NOT_SUPPORTED;
      owa.rc = XLIBRC_COMMON_PORTCFG_MAC_LOOP_ERROR;
      portLoopbackPassValue = L7_FALSE;
#endif
  }
  else if(objPortTestType == L7_PORT_LOOPBACK_NONE)
  {
    owa.l7rc = usmDbLanPortTest(L7_UNIT_CURRENT, keyinterfaceValue,
                                L7_PORT_LOOPBACK_NONE);
    if(owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbPortTestEnDisSet (keyinterfaceValue,L7_FALSE);
    }
    portLoopbackPassValue = L7_FALSE;
  }

#ifndef L7_ROBO_SUPPORT
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
#endif

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#else
/*******************************************************************************
* @function fpObjGet_basePortTest_PortTestType
*
* @purpose Get 'PortTestType'
 *@description  [PortTestType] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortTest_PortTestType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPortTestTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  kwa.len = sizeof(xLibU32_t);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                          (xLibU8_t *) & keyinterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

 FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinterfaceValue, kwa.len);

  /* retrieve key: PortTestType */
  /* owa.l7rc = usmDbLanPortTestGet(L7_UNIT_CURRENT, keyinterfaceValue, &objPortTestTypeValue); */

  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* return the object value: PortTestType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortTestTypeValue,
                           sizeof (objPortTestTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
 * * @function fpObjGet_basePortTest_PortTestType
 * *
 * * @purpose Set 'ethernetOam'
 *  *@description  [ethernetOam] Enable/Disable dot3ah on a interface.
 *  * @notes
 *  *
 *  * @return
 *  *******************************************************************************/
xLibRC_t fpObjSet_basePortTest_PortTestType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortTestType;

  fpObjWa_t kwainterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyinterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof(xLibU32_t);
  kwainterface.len = sizeof(xLibU32_t);

  /* retrieve object: objPortTestType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPortTestType, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortTestType, owa.len);

  /* retrieve key: interface */
  kwainterface.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                                   (xLibU8_t *) & keyinterfaceValue, &kwainterface.len);
  if (kwainterface.rc != XLIBRC_SUCCESS)
  {
    kwainterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwainterface);
    return kwainterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyinterfaceValue, kwainterface.len);


  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#endif
/*******************************************************************************
* @function fpObjGet_basePortTest_IsLoopbackPass
*
* @purpose Get 'IsLoopbackPass'
 *@description  [IsLoopbackPass] the number of FrameError OAMPDUs transmitted on
* this interface.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortTest_IsLoopbackPass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t IsLoopbackPassValue;
  FPOBJ_TRACE_ENTER (bufp);

  IsLoopbackPassValue = portLoopbackPassValue;

  /* return the object value: frameErrorTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &IsLoopbackPassValue,
                           sizeof (IsLoopbackPassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basePortTest_Status
*
* @purpose Get 'Status'
 *@description  [Status] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortTest_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objStatusValue;

  xLibU32_t value;
  xLibU32_t totalErr = 0;
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet(L7_UNIT_CURRENT, L7_CTR_RX_TOTAL_ERROR_FRAMES, keyInterfaceValue, &value);
  totalErr+=value;
  owa.l7rc = usmDbStatGet(L7_UNIT_CURRENT, L7_CTR_TX_TOTAL_ERROR_FRAMES, keyInterfaceValue, &value);
  totalErr+=value;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
   strcpy(objStatusValue," ");
  if(totalErr == 0)
   strcpy(objStatusValue, "Good");
  else
   strcpy(objStatusValue, "Bad")
  
  FPOBJ_TRACE_VALUE (bufp, objStatusValue, strlen (objStatusValue));

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStatusValue, strlen (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortTest_dataTx
*
* @purpose Get 'dataTx'
 *@description  [dataTx] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortTest_dataFrameTx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdataTxValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet (L7_UNIT_CURRENT, L7_CTR_TX_TOTAL_FRAMES, keyInterfaceValue, &objdataTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdataTxValue, sizeof (objdataTxValue));

  /* return the object value: dataTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdataTxValue, sizeof (objdataTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortTest_dataRx
*
* @purpose Get 'dataRx'
 *@description  [dataRx] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortTest_dataFrameRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdataRxValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet (L7_UNIT_CURRENT, L7_CTR_RX_TOTAL_FRAMES, 
                           keyInterfaceValue, &objdataRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdataRxValue, sizeof (objdataRxValue));

  /* return the object value: dataRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdataRxValue, sizeof (objdataRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortTest_dataTxErr
*
* @purpose Get 'dataTxErr'
 *@description  [dataTxErr] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortTest_errorFrameTx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdataTxErrValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet (L7_UNIT_CURRENT, L7_CTR_TX_TOTAL_ERROR_FRAMES,
                           keyInterfaceValue, &objdataTxErrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdataTxErrValue, sizeof (objdataTxErrValue));

  /* return the object value: dataTxErr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdataTxErrValue, sizeof (objdataTxErrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basePortTest_dataRxErr
*
* @purpose Get 'dataRxErr'
 *@description  [dataRxErr] ToDO: Add Help   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basePortTest_errorFrameRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdataRxErrValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basePortTest_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbStatGet (L7_UNIT_CURRENT, L7_CTR_RX_TOTAL_ERROR_FRAMES,
                           keyInterfaceValue, &objdataRxErrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdataRxErrValue, sizeof (objdataRxErrValue));

  /* return the object value: dataRxErr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdataRxErrValue, sizeof (objdataRxErrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


