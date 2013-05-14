
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_basecableTestFiberSumm.c
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
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
/* This is for abs function declaration */
#include <stdlib.h>

#include "fpobj_util.h"
#include "_xe_basecableTestFiberSumm_obj.h"
#include "usmdb_status.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"

/*******************************************************************************
* @function fpObjGet_basecableTestFiberSumm_fiberInterface
*
* @purpose Get 'fiberInterface'
 *@description  [fiberInterface] <HTML>Fiber Interface    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestFiberSumm_fiberInterface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objfiberInterfaceValue;
  xLibU32_t nextObjfiberInterfaceValue;
  xLibU32_t connType;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: fiberInterface */
  owa.len = sizeof (objfiberInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestFiberSumm_fiberInterface,
                          (xLibU8_t *) & objfiberInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objfiberInterfaceValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objfiberInterfaceValue, owa.len);
  owa.l7rc = usmDbValidIntIfNumNext (objfiberInterfaceValue,
                                    &nextObjfiberInterfaceValue);
  /* Loop through all the interfaces and skip interfaces other than Copper */
  while(owa.l7rc == L7_SUCCESS) 
  {
    if (( usmDbIntfPhyCapabilityGet(nextObjfiberInterfaceValue, &connType) == L7_SUCCESS ) )
    {
      if (connType & L7_PHY_CAP_PORTSPEED_SFP)
      {
        break;
      }
    } 
    objfiberInterfaceValue =nextObjfiberInterfaceValue;
    owa.l7rc = usmDbValidIntIfNumNext (objfiberInterfaceValue,
                                    &nextObjfiberInterfaceValue);
   
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjfiberInterfaceValue, owa.len);

  /* return the object value: fiberInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjfiberInterfaceValue,
                           sizeof (nextObjfiberInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestFiberSumm_GetFiberResult
*
* @purpose Set 'TestFiberTrigger'
 *@description  [TestFiberTrigger] <HTML>Object to Get the Cable Test for
* Fiber   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestFiberSumm_GetFiberResult (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTestFiberTriggerValue;

  xLibU32_t keyfiberInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* These arguments are required for the given handler,Hence pass them as dummy Arguments */
  xLibS32_t  dummyTemperature;
  xLibU32_t  dummyVoltage;
  xLibU32_t  dummyCurrent;
  xLibDouble64_t  dummyPowerOutput;
  xLibDouble64_t  dummyPowerInput;
  L7_BOOL    dummyTxDefault;
  L7_BOOL    dummyLOS;

  /* retrieve object: TestFiberTrigger */
  owa.len = sizeof (objTestFiberTriggerValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTestFiberTriggerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTestFiberTriggerValue, owa.len);

  /* retrieve key: fiberInterface */
  owa.len = sizeof (keyfiberInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestFiberSumm_fiberInterface,
                          (xLibU8_t *) & keyfiberInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfiberInterfaceValue, owa.len);

  /* set the value in application */
    owa.l7rc = usmDbCableStatusFiberTest (keyfiberInterfaceValue, &dummyTemperature,
                                        &dummyVoltage,&dummyCurrent,&dummyPowerOutput,
                                        &dummyPowerInput,&dummyTxDefault,&dummyLOS); 

  /* Return failure only in case of Error as 
   * there is a chance that this routine will return L7_FAILURE 
   * when either the SFP is not inserted or when the inserted SFP is down 
   */
  if (owa.l7rc == L7_ERROR)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basecableTestFiberSumm_GetFiberResult
*
* @purpose Set 'TestFiberTrigger'
 *@description  [TestFiberTrigger] <HTML>Object to Trigger the Cable Test for
* Fiber   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basecableTestFiberSumm_GetFiberResult (void *wap, void *bufp)
{
  /* Just Return SUCCESS here */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_basecableTestFiberSumm_Voltage
*
* @purpose Get 'Voltage'
 *@description  [Voltage] <HTML>Volate Level of the Fiber Interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestFiberSumm_Voltage (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVoltageValue;

  xLibU32_t keyfiberInterfaceValue;
  xLibStr256_t voltageValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  /* These arguments are required for the given handler,Hence pass them as dummy Arguments */
  xLibS32_t  dummyTemperature;
  xLibU32_t  dummyCurrent;
  xLibDouble64_t  dummyPowerOutput;
  xLibDouble64_t  dummyPowerInput;
  L7_BOOL    dummyTxDefault;
  L7_BOOL    dummyLOS;

  /* Clear the buffer first */
  FPOBJ_CLR_STR256(voltageValue);

  /* retrieve key: fiberInterface */
  owa.len = sizeof (keyfiberInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestFiberSumm_fiberInterface,
                          (xLibU8_t *) & keyfiberInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfiberInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusFiberResult (keyfiberInterfaceValue, &dummyTemperature,
                                        &objVoltageValue,&dummyCurrent,&dummyPowerOutput,
                                        &dummyPowerInput,&dummyTxDefault,&dummyLOS); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiSnprintf(voltageValue, sizeof(voltageValue), "%1d.%3.3d",
                objVoltageValue / 1000, objVoltageValue % 1000);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)voltageValue, strlen(voltageValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestFiberSumm_Temperature
*
* @purpose Get 'Temperature'
 *@description  [Temperature] <HTML>Temperature of the Fiber Cable Interface   
* @notes       
*
* @return
*******************************************************************************/
extern int abs ();
xLibRC_t fpObjGet_basecableTestFiberSumm_Temperature (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibS32_t objTemperatureValue;

  xLibU32_t keyfiberInterfaceValue;
  xLibStr256_t temperatureValue;

  /* These arguments are required for the given handler,Hence pass them as dummy Arguments */
  xLibU32_t  dummyVoltage;
  xLibU32_t  dummyCurrent;
  xLibDouble64_t  dummyPowerOutput;
  xLibDouble64_t  dummyPowerInput;
  L7_BOOL    dummyTxDefault;
  L7_BOOL    dummyLOS;

  FPOBJ_TRACE_ENTER (bufp);

  /* Clear the buffer first */
  FPOBJ_CLR_STR256(temperatureValue);

  /* retrieve key: fiberInterface */
  owa.len = sizeof (keyfiberInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestFiberSumm_fiberInterface,
                          (xLibU8_t *) & keyfiberInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfiberInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusFiberResult (keyfiberInterfaceValue, &objTemperatureValue,
                                        &dummyVoltage,&dummyCurrent,&dummyPowerOutput,
                                        &dummyPowerInput,&dummyTxDefault,&dummyLOS); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiSnprintf(temperatureValue, sizeof(temperatureValue), "%4d.%1.1d",
                objTemperatureValue/1000, (abs(objTemperatureValue)%1000)/100);
  /* return the object value: Voltage */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)temperatureValue, strlen(temperatureValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestFiberSumm_Current
*
* @purpose Get 'Current'
 *@description  [Current] <HTML>Current Level of the given Interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestFiberSumm_Current (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCurrentValue;

  xLibU32_t keyfiberInterfaceValue;
  xLibStr256_t currentValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* These arguments are required for the given handler,Hence pass them as dummy Arguments */
  xLibS32_t  dummyTemperature;
  xLibU32_t  dummyVoltage;
  xLibDouble64_t  dummyPowerOutput;
  xLibDouble64_t  dummyPowerInput;
  L7_BOOL    dummyTxDefault;
  L7_BOOL    dummyLOS;
  
  /* Clear the buffer first */
  FPOBJ_CLR_STR256(currentValue);

  /* retrieve key: fiberInterface */
  owa.len = sizeof (keyfiberInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestFiberSumm_fiberInterface,
                          (xLibU8_t *) & keyfiberInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfiberInterfaceValue, owa.len);

  /* get the value from application */
    owa.l7rc = usmDbCableStatusFiberResult (keyfiberInterfaceValue, &dummyTemperature,
                                        &dummyVoltage,&objCurrentValue,&dummyPowerOutput,
                                        &dummyPowerInput,&dummyTxDefault,&dummyLOS); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiSnprintf(currentValue, sizeof(currentValue), "%5d.%1.1d", 
                objCurrentValue/1000, (objCurrentValue%1000)/100);
  /* return the object value: Voltage */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)currentValue, strlen(currentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestFiberSumm_PowerOutput
*
* @purpose Get 'PowerOutput'
 *@description  [PowerOutput] <HTML>Output Power of the given Interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestFiberSumm_PowerOutput (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibDouble64_t objPowerOutputValue;

  xLibU32_t keyfiberInterfaceValue;
  xLibStr256_t powerOutputValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* These arguments are required for the given handler,Hence pass them as dummy Arguments */
  xLibU32_t  dummyTemperature;
  xLibU32_t  dummyVoltage;
  xLibU32_t  dummyCurrent;
  xLibDouble64_t  dummyPowerInput;
  L7_BOOL    dummyTxDefault;
  L7_BOOL    dummyLOS;
  
  /* Clear the buffer first */
  FPOBJ_CLR_STR256(powerOutputValue);

  /* retrieve key: fiberInterface */
  owa.len = sizeof (keyfiberInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestFiberSumm_fiberInterface,
                          (xLibU8_t *) & keyfiberInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfiberInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusFiberResult (keyfiberInterfaceValue, &dummyTemperature,
                                        &dummyVoltage,&dummyCurrent,&objPowerOutputValue,
                                        &dummyPowerInput,&dummyTxDefault,&dummyLOS); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiSnprintf(powerOutputValue, sizeof(powerOutputValue), "%7.3f", objPowerOutputValue);

  /* return the object value: Voltage */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)powerOutputValue, strlen(powerOutputValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestFiberSumm_PowerInput
*
* @purpose Get 'PowerInput'
 *@description  [PowerInput] <HTML>Input Power of the given Interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestFiberSumm_PowerInput (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibDouble64_t objPowerInputValue;

  xLibU32_t keyfiberInterfaceValue;
  xLibStr256_t powerInputValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* These arguments are required for the given handler,Hence pass them as dummy Arguments */
  xLibS32_t  dummyTemperature;
  xLibU32_t  dummyVoltage;
  xLibU32_t  dummyCurrent;
  xLibDouble64_t  dummyPowerOutput;
  L7_BOOL    dummyTxDefault;
  L7_BOOL    dummyLOS;
  
  /* Clear the buffer first */
  FPOBJ_CLR_STR256(powerInputValue);

  /* retrieve key: fiberInterface */
  owa.len = sizeof (keyfiberInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestFiberSumm_fiberInterface,
                          (xLibU8_t *) & keyfiberInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfiberInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusFiberResult (keyfiberInterfaceValue, &dummyTemperature,
                                        &dummyVoltage,&dummyCurrent,&dummyPowerOutput,
                                        &objPowerInputValue,&dummyTxDefault,&dummyLOS); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiSnprintf(powerInputValue, sizeof(powerInputValue), "%7.3f", objPowerInputValue);

  /* return the object value: Voltage */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)powerInputValue, strlen(powerInputValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestFiberSumm_TxFault
*
* @purpose Get 'TxFault'
 *@description  [TxFault] <HTML>Fiber Tx Fault   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestFiberSumm_TxFault (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTxFaultValue;

  xLibU32_t keyfiberInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* These arguments are required for the given handler,Hence pass them as dummy Arguments */
  xLibS32_t  dummyTemperature;
  xLibU32_t  dummyVoltage;
  xLibU32_t  dummyCurrent;
  xLibDouble64_t  dummyPowerOutput;
  xLibDouble64_t  dummyPowerInput;
  L7_BOOL    dummyLOS;
  
  /* retrieve key: fiberInterface */
  owa.len = sizeof (keyfiberInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestFiberSumm_fiberInterface,
                          (xLibU8_t *) & keyfiberInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfiberInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusFiberResult (keyfiberInterfaceValue, &dummyTemperature,
                                        &dummyVoltage,&dummyCurrent,&dummyPowerOutput,
                                        &dummyPowerInput,&objTxFaultValue,&dummyLOS); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTxFaultValue, sizeof (objTxFaultValue));

  /* return the object value: TxFault */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxFaultValue, sizeof (objTxFaultValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basecableTestFiberSumm_LOS
*
* @purpose Get 'LOS'
 *@description  [LOS] <HTML>Fiber LOS   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basecableTestFiberSumm_LOS (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLOSValue;

  xLibU32_t keyfiberInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* These arguments are required for the given handler,Hence pass them as dummy Arguments */
  xLibS32_t  dummyTemperature;
  xLibU32_t  dummyVoltage;
  xLibU32_t  dummyCurrent;
  xLibDouble64_t  dummyPowerOutput;
  xLibDouble64_t  dummyPowerInput;
  L7_BOOL    dummyTxDefault;
  
  /* retrieve key: fiberInterface */
  owa.len = sizeof (keyfiberInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basecableTestFiberSumm_fiberInterface,
                          (xLibU8_t *) & keyfiberInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyfiberInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCableStatusFiberResult (keyfiberInterfaceValue, &dummyTemperature,
                                        &dummyVoltage,&dummyCurrent,&dummyPowerOutput,
                                        &dummyPowerInput,&dummyTxDefault,&objLOSValue); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objLOSValue, sizeof (objLOSValue));

  /* return the object value: LOS */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLOSValue, sizeof (objLOSValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
