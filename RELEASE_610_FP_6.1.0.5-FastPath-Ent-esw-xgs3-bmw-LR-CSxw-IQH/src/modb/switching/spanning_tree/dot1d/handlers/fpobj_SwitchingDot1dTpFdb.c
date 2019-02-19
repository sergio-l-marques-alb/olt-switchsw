
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingDot1dTpFdb.c
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
#include "_xe_SwitchingDot1dTpFdb_obj.h"
#include "usmdb_mib_bridge_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dTpFdb_dot1dTpFdbAddress
*
* @purpose Get 'dot1dTpFdbAddress'
 *@description  [dot1dTpFdbAddress] Fdb address   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dTpFdb_dot1dTpFdbAddress (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objdot1dTpFdbAddressValue;
  xLibStr6_t nextObjdot1dTpFdbAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(nextObjdot1dTpFdbAddressValue, 0x0,sizeof(nextObjdot1dTpFdbAddressValue));
  memset(objdot1dTpFdbAddressValue, 0x0,sizeof(objdot1dTpFdbAddressValue));

  /* retrieve key: dot1dTpFdbAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTpFdb_dot1dTpFdbAddress,
                          (xLibU8_t *) objdot1dTpFdbAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDot1dTpFdbEntryNextGet(L7_UNIT_CURRENT, nextObjdot1dTpFdbAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objdot1dTpFdbAddressValue, owa.len);
    memcpy(nextObjdot1dTpFdbAddressValue, objdot1dTpFdbAddressValue, sizeof(nextObjdot1dTpFdbAddressValue));
    owa.l7rc = usmDbDot1dTpFdbEntryNextGet(L7_UNIT_CURRENT, 
                                    nextObjdot1dTpFdbAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjdot1dTpFdbAddressValue, owa.len);

  /* return the object value: dot1dTpFdbAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjdot1dTpFdbAddressValue,
                           sizeof (objdot1dTpFdbAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dTpFdb_Dot1dTpFdbPort
*
* @purpose Get 'Dot1dTpFdbPort'
 *@description  [Dot1dTpFdbPort] Fdb port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dTpFdb_Dot1dTpFdbPort (void *wap, void *bufp)
{

  fpObjWa_t kwadot1dTpFdbAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keydot1dTpFdbAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dTpFdbPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1dTpFdbAddress */
  kwadot1dTpFdbAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTpFdb_dot1dTpFdbAddress,
                                           (xLibU8_t *) keydot1dTpFdbAddressValue,
                                           &kwadot1dTpFdbAddress.len);
  if (kwadot1dTpFdbAddress.rc != XLIBRC_SUCCESS)
  {
    kwadot1dTpFdbAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dTpFdbAddress);
    return kwadot1dTpFdbAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keydot1dTpFdbAddressValue, kwadot1dTpFdbAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1dTpFdbPortGet(L7_UNIT_CURRENT, keydot1dTpFdbAddressValue, &objDot1dTpFdbPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dTpFdbPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dTpFdbPortValue,
                           sizeof (objDot1dTpFdbPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingDot1dTpFdb_Dot1dTpFdbStatus
*
* @purpose Get 'Dot1dTpFdbStatus'
 *@description  [Dot1dTpFdbStatus] Fdb status   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingDot1dTpFdb_Dot1dTpFdbStatus (void *wap, void *bufp)
{

  fpObjWa_t kwadot1dTpFdbAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keydot1dTpFdbAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1dTpFdbStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1dTpFdbAddress */
  kwadot1dTpFdbAddress.rc = xLibFilterGet (wap, XOBJ_SwitchingDot1dTpFdb_dot1dTpFdbAddress,
                                           (xLibU8_t *) keydot1dTpFdbAddressValue,
                                           &kwadot1dTpFdbAddress.len);
  if (kwadot1dTpFdbAddress.rc != XLIBRC_SUCCESS)
  {
    kwadot1dTpFdbAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1dTpFdbAddress);
    return kwadot1dTpFdbAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keydot1dTpFdbAddressValue, kwadot1dTpFdbAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1dTpFdbStatusGet(L7_UNIT_CURRENT, keydot1dTpFdbAddressValue,
                              &objDot1dTpFdbStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1dTpFdbStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1dTpFdbStatusValue,
                           sizeof (objDot1dTpFdbStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
