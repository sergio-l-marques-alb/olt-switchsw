
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1dStatic.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  27 May 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingDot1dStatic_obj.h"
#include "usmdb_mib_bridge_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dStatic_dot1dStaticAddress
*
* @purpose Get 'dot1dStaticAddress'
 *@description  [dot1dStaticAddress] static address   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dStatic_dot1dStaticAddress (void *wap, void *bufp)
{

  xLibStr256_t objdot1dStaticAddressValue;
  xLibStr256_t nextObjdot1dStaticAddressValue;
  xLibU32_t objdot1dStaticReceivePortValue;
  xLibU32_t nextObjdot1dStaticReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset (objdot1dStaticAddressValue, 0, sizeof (objdot1dStaticAddressValue));
  memset (nextObjdot1dStaticAddressValue, 0, sizeof (nextObjdot1dStaticAddressValue));
  memset (&objdot1dStaticReceivePortValue, 0, sizeof (objdot1dStaticReceivePortValue));
  memset (&objdot1dStaticReceivePortValue, 0, sizeof (nextObjdot1dStaticReceivePortValue));

  /* retrieve key: dot1dStaticAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticAddress,
                          (xLibU8_t *) objdot1dStaticAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDot1dStaticEntryNextGet(L7_UNIT_CURRENT,
                                     nextObjdot1dStaticAddressValue,
                                     &nextObjdot1dStaticReceivePortValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objdot1dStaticAddressValue, owa.len);
    memcpy (nextObjdot1dStaticAddressValue, objdot1dStaticAddressValue, sizeof (nextObjdot1dStaticAddressValue));
    do
    {
      owa.l7rc = usmDbDot1dStaticEntryNextGet(L7_UNIT_CURRENT,
                                      nextObjdot1dStaticAddressValue,
                                      &nextObjdot1dStaticReceivePortValue);
    }
    while ((memcmp(objdot1dStaticAddressValue,nextObjdot1dStaticAddressValue,sizeof(objdot1dStaticAddressValue)) ==0)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjdot1dStaticAddressValue, owa.len);

  /* return the object value: dot1dStaticAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjdot1dStaticAddressValue,
                           strlen (objdot1dStaticAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dStatic_dot1dStaticReceivePort
*
* @purpose Get 'dot1dStaticReceivePort'
 *@description  [dot1dStaticReceivePort] static port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dStatic_dot1dStaticReceivePort (void *wap, void *bufp)
{

  xLibStr256_t objdot1dStaticAddressValue;
  xLibStr256_t nextObjdot1dStaticAddressValue;
  xLibU32_t objdot1dStaticReceivePortValue;
  xLibU32_t nextObjdot1dStaticReceivePortValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset (objdot1dStaticAddressValue, 0, sizeof (objdot1dStaticAddressValue));
  memset (nextObjdot1dStaticAddressValue, 0, sizeof (nextObjdot1dStaticAddressValue));
  memset (&objdot1dStaticReceivePortValue, 0, sizeof (objdot1dStaticReceivePortValue));
  memset (&objdot1dStaticReceivePortValue, 0, sizeof (nextObjdot1dStaticReceivePortValue));
  /* retrieve key: dot1dStaticAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticAddress,
                          (xLibU8_t *) objdot1dStaticAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objdot1dStaticAddressValue, owa.len);

  /* retrieve key: dot1dStaticReceivePort */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticReceivePort,
                          (xLibU8_t *) & objdot1dStaticReceivePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memcpy (nextObjdot1dStaticAddressValue, objdot1dStaticAddressValue, sizeof (nextObjdot1dStaticAddressValue));
    owa.l7rc = usmDbDot1dStaticEntryNextGet(L7_UNIT_CURRENT,
                                     nextObjdot1dStaticAddressValue,
                                     &nextObjdot1dStaticReceivePortValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1dStaticReceivePortValue, owa.len);

    nextObjdot1dStaticReceivePortValue = objdot1dStaticReceivePortValue;
    memcpy (nextObjdot1dStaticAddressValue, objdot1dStaticAddressValue, sizeof (nextObjdot1dStaticAddressValue));

    owa.l7rc = usmDbDot1dStaticEntryNextGet(L7_UNIT_CURRENT,
                                    nextObjdot1dStaticAddressValue,
                                    &nextObjdot1dStaticReceivePortValue);

  }

  if ((memcmp(objdot1dStaticAddressValue, nextObjdot1dStaticAddressValue, sizeof(objdot1dStaticAddressValue)) != 0) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1dStaticReceivePortValue, owa.len);

  /* return the object value: dot1dStaticReceivePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1dStaticReceivePortValue,
                           sizeof (objdot1dStaticReceivePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dStatic_Dot1dStaticAllowedToGo
*
* @purpose Get 'Dot1dStaticAllowedToGo'
 *@description  [Dot1dStaticAllowedToGo] static allowed to go   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dStatic_Dot1dStaticAllowedToGo (void *wap, void *bufp)
{

  fpObjWa_t kwadot1dStaticAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keydot1dStaticAddressValue;
  fpObjWa_t kwadot1dStaticReceivePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dStaticReceivePortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDot1dStaticAllowedToGoValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1dStaticAddress */
  kwadot1dStaticAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticAddress,
                                            (xLibU8_t *) keydot1dStaticAddressValue,
                                            &kwadot1dStaticAddress.len);
  if (kwadot1dStaticAddress.rc != XLIBRC_SUCCESS)
  {
    kwadot1dStaticAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dStaticAddress);
    return kwadot1dStaticAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keydot1dStaticAddressValue, kwadot1dStaticAddress.len);
  
  kwadot1dStaticReceivePort.len = 4;
  /* retrieve key: dot1dStaticReceivePort */
  kwadot1dStaticReceivePort.rc =
    xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticReceivePort,
                   (xLibU8_t *) & keydot1dStaticReceivePortValue, &kwadot1dStaticReceivePort.len);
  if (kwadot1dStaticReceivePort.rc != XLIBRC_SUCCESS)
  {
    kwadot1dStaticReceivePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dStaticReceivePort);
    return kwadot1dStaticReceivePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dStaticReceivePortValue, kwadot1dStaticReceivePort.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1dStaticAllowedToGoToGet(L7_UNIT_CURRENT, keydot1dStaticAddressValue,
                              keydot1dStaticReceivePortValue, objDot1dStaticAllowedToGoValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dStaticAllowedToGo */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1dStaticAllowedToGoValue,
                           sizeof(objDot1dStaticAllowedToGoValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDot1dStatic_Dot1dStaticAllowedToGo
*
* @purpose Set 'Dot1dStaticAllowedToGo'
 *@description  [Dot1dStaticAllowedToGo] static allowed to go   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1dStatic_Dot1dStaticAllowedToGo (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDot1dStaticAllowedToGoValue;

  fpObjWa_t kwadot1dStaticAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keydot1dStaticAddressValue;
  fpObjWa_t kwadot1dStaticReceivePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dStaticReceivePortValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1dStaticAllowedToGo */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDot1dStaticAllowedToGoValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDot1dStaticAllowedToGoValue, owa.len);

  /* retrieve key: dot1dStaticAddress */
  kwadot1dStaticAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticAddress,
                                            (xLibU8_t *) keydot1dStaticAddressValue,
                                            &kwadot1dStaticAddress.len);
  if (kwadot1dStaticAddress.rc != XLIBRC_SUCCESS)
  {
    kwadot1dStaticAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dStaticAddress);
    return kwadot1dStaticAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keydot1dStaticAddressValue, kwadot1dStaticAddress.len);

  /* retrieve key: dot1dStaticReceivePort */
  kwadot1dStaticReceivePort.rc =
    xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticReceivePort,
                   (xLibU8_t *) & keydot1dStaticReceivePortValue, &kwadot1dStaticReceivePort.len);
  if (kwadot1dStaticReceivePort.rc != XLIBRC_SUCCESS)
  {
    kwadot1dStaticReceivePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dStaticReceivePort);
    return kwadot1dStaticReceivePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dStaticReceivePortValue, kwadot1dStaticReceivePort.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1dStaticAllowedToGoToSet(L7_UNIT_CURRENT, keydot1dStaticAddressValue,
                              keydot1dStaticReceivePortValue, objDot1dStaticAllowedToGoValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dStatic_Dot1dStaticStatus
*
* @purpose Get 'Dot1dStaticStatus'
 *@description  [Dot1dStaticStatus] static status   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dStatic_Dot1dStaticStatus (void *wap, void *bufp)
{

  fpObjWa_t kwadot1dStaticAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keydot1dStaticAddressValue;
  fpObjWa_t kwadot1dStaticReceivePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dStaticReceivePortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dStaticStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1dStaticAddress */
  kwadot1dStaticAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticAddress,
                                            (xLibU8_t *) keydot1dStaticAddressValue,
                                            &kwadot1dStaticAddress.len);
  if (kwadot1dStaticAddress.rc != XLIBRC_SUCCESS)
  {
    kwadot1dStaticAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dStaticAddress);
    return kwadot1dStaticAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keydot1dStaticAddressValue, kwadot1dStaticAddress.len);

  /* retrieve key: dot1dStaticReceivePort */
  kwadot1dStaticReceivePort.rc =
    xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticReceivePort,
                   (xLibU8_t *) & keydot1dStaticReceivePortValue, &kwadot1dStaticReceivePort.len);
  if (kwadot1dStaticReceivePort.rc != XLIBRC_SUCCESS)
  {
    kwadot1dStaticReceivePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dStaticReceivePort);
    return kwadot1dStaticReceivePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dStaticReceivePortValue, kwadot1dStaticReceivePort.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1dStaticStatusGet(L7_UNIT_CURRENT, keydot1dStaticAddressValue,
                              keydot1dStaticReceivePortValue, &objDot1dStaticStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dStaticStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dStaticStatusValue,
                           sizeof (objDot1dStaticStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingDot1dStatic_Dot1dStaticStatus
*
* @purpose Set 'Dot1dStaticStatus'
 *@description  [Dot1dStaticStatus] static status   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingDot1dStatic_Dot1dStaticStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dStaticStatusValue;

  fpObjWa_t kwadot1dStaticAddress = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keydot1dStaticAddressValue;
  fpObjWa_t kwadot1dStaticReceivePort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1dStaticReceivePortValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1dStaticStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1dStaticStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1dStaticStatusValue, owa.len);

  /* retrieve key: dot1dStaticAddress */
  kwadot1dStaticAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticAddress,
                                            (xLibU8_t *) keydot1dStaticAddressValue,
                                            &kwadot1dStaticAddress.len);
  if (kwadot1dStaticAddress.rc != XLIBRC_SUCCESS)
  {
    kwadot1dStaticAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dStaticAddress);
    return kwadot1dStaticAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keydot1dStaticAddressValue, kwadot1dStaticAddress.len);

  /* retrieve key: dot1dStaticReceivePort */
  kwadot1dStaticReceivePort.rc =
    xLibFilterGet (wap, XOBJ_SwitchingDot1dStatic_dot1dStaticReceivePort,
                   (xLibU8_t *) & keydot1dStaticReceivePortValue, &kwadot1dStaticReceivePort.len);
  if (kwadot1dStaticReceivePort.rc != XLIBRC_SUCCESS)
  {
    kwadot1dStaticReceivePort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dStaticReceivePort);
    return kwadot1dStaticReceivePort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1dStaticReceivePortValue, kwadot1dStaticReceivePort.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1dStaticStatusSet(L7_UNIT_CURRENT, keydot1dStaticAddressValue,
                              keydot1dStaticReceivePortValue, objDot1dStaticStatusValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
