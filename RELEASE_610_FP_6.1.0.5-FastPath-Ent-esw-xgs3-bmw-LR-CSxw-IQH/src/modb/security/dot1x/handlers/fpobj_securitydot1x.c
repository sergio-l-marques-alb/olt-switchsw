/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_securitydot1x.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot1x-object.xml
*
* @create  16 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_securitydot1x_obj.h"
#include "usmdb_dot1x_api.h"
#include "dot1x_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xPaePortNumber
*
* @purpose Get 'dot1xPaePortNumber'
*
* @description The Port number associated with this Port. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xPaePortNumber (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xPaePortNumberValue;
  xLibU32_t nextObjdot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & objdot1xPaePortNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objdot1xPaePortNumberValue = 0;
    nextObjdot1xPaePortNumberValue = 0;
  }
  
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1xPaePortNumberValue, owa.len);
    owa.l7rc = usmDbGetNextPhysicalIntIfNumber (objdot1xPaePortNumberValue,
                                    &nextObjdot1xPaePortNumberValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1xPaePortNumberValue, owa.len);

  /* return the object value: dot1xPaePortNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1xPaePortNumberValue,
                           sizeof (objdot1xPaePortNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
	
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppEapolFramesRx
*
* @purpose Get 'dot1xSuppEapolFramesRx'
*
* @description The number of EAPOL frames of any type that have been received 
*              by this Supplicant. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppEapolFramesRx (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppEapolFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortEapolFramesRxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                    &objdot1xSuppEapolFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppEapolFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppEapolFramesRxValue,
                           sizeof (objdot1xSuppEapolFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppEapolFramesTx
*
* @purpose Get 'dot1xSuppEapolFramesTx'
*
* @description The number of EAPOL frames of any type that have been transmitted 
*              by this Supplicant. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppEapolFramesTx (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppEapolFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortEapolFramesTxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                    &objdot1xSuppEapolFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppEapolFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppEapolFramesTxValue,
                           sizeof (objdot1xSuppEapolFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppEapolStartFramesTx
*
* @purpose Get 'dot1xSuppEapolStartFramesTx'
*
* @description The number of EAPOL Start frames that have been transmitted by 
*              this Supplicant. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppEapolStartFramesTx (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppEapolStartFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortEapolStartFramesRxGet (L7_UNIT_CURRENT,  
                                         keydot1xPaePortNumberValue,
                                         &objdot1xSuppEapolStartFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppEapolStartFramesTx */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppEapolStartFramesTxValue,
                    sizeof (objdot1xSuppEapolStartFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppEapolLogoffFramesTx
*
* @purpose Get 'dot1xSuppEapolLogoffFramesTx'
*
* @description The number of EAPOL Logoff frames that have been transmitted by 
*              this Supplicant. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppEapolLogoffFramesTx (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppEapolLogoffFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortEapolLogoffFramesRxGet (L7_UNIT_CURRENT,
                                          keydot1xPaePortNumberValue,
                                          &objdot1xSuppEapolLogoffFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppEapolLogoffFramesTx */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppEapolLogoffFramesTxValue,
                    sizeof (objdot1xSuppEapolLogoffFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppEapolRespIdFramesTx
*
* @purpose Get 'dot1xSuppEapolRespIdFramesTx'
*
* @description The number of EAP Resp/Id frames that have been transmitted by 
*              this Supplicant. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppEapolRespIdFramesTx (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppEapolRespIdFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortEapRespIdFramesRxGet (L7_UNIT_CURRENT,
                                        keydot1xPaePortNumberValue,
                                        &objdot1xSuppEapolRespIdFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppEapolRespIdFramesTx */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppEapolRespIdFramesTxValue,
                    sizeof (objdot1xSuppEapolRespIdFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppEapolRespFramesTx
*
* @purpose Get 'dot1xSuppEapolRespFramesTx'
*
* @description The number of valid EAP Response frames (other than Resp/Id frames) 
*              that have been transmitted by this Supplicant. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppEapolRespFramesTx (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppEapolRespFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortEapResponseFramesRxGet (L7_UNIT_CURRENT,
                                          keydot1xPaePortNumberValue,
                                          &objdot1xSuppEapolRespFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppEapolRespFramesTx */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppEapolRespFramesTxValue,
                    sizeof (objdot1xSuppEapolRespFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppEapolReqIdFramesRx
*
* @purpose Get 'dot1xSuppEapolReqIdFramesRx'
*
* @description The number of EAP Req/Id frames that have been received by this 
*              Supplicant. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppEapolReqIdFramesRx (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppEapolReqIdFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortEapReqIdFramesTxGet (L7_UNIT_CURRENT,
                                       keydot1xPaePortNumberValue,
                                       &objdot1xSuppEapolReqIdFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppEapolReqIdFramesRx */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppEapolReqIdFramesRxValue,
                    sizeof (objdot1xSuppEapolReqIdFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppEapolReqFramesRx
*
* @purpose Get 'dot1xSuppEapolReqFramesRx'
*
* @description The number of EAP Request frames (other than Rq/Id frames) that 
*              have been received by this Supplicant. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppEapolReqFramesRx (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppEapolReqFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortEapReqFramesTxGet (L7_UNIT_CURRENT,
                                     keydot1xPaePortNumberValue,
                                     &objdot1xSuppEapolReqFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppEapolReqFramesRx */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppEapolReqFramesRxValue,
                    sizeof (objdot1xSuppEapolReqFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppInvalidEapolFramesRx
*
* @purpose Get 'dot1xSuppInvalidEapolFramesRx'
*
* @description The number of EAPOL frames that have been received by this Supplicant 
*              in which the frame type is not recognized. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppInvalidEapolFramesRx (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppInvalidEapolFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortInvalidEapolFramesRxGet (L7_UNIT_CURRENT,
                                           keydot1xPaePortNumberValue,
                                           &objdot1xSuppInvalidEapolFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppInvalidEapolFramesRx */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppInvalidEapolFramesRxValue,
                    sizeof (objdot1xSuppInvalidEapolFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppEapLengthErrorFramesRx
*
* @purpose Get 'dot1xSuppEapLengthErrorFramesRx'
*
* @description The number of EAPOL frames that have been received by this Supplicant 
*              in which the Packet Body Length field (7.5.5) is invalid. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppEapLengthErrorFramesRx (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppEapLengthErrorFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortEapLengthErrorFramesRxGet (L7_UNIT_CURRENT,
                                             keydot1xPaePortNumberValue,
                                             &objdot1xSuppEapLengthErrorFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppEapLengthErrorFramesRx */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objdot1xSuppEapLengthErrorFramesRxValue,
                    sizeof (objdot1xSuppEapLengthErrorFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppLastEapolFrameVersion
*
* @purpose Get 'dot1xSuppLastEapolFrameVersion'
*
* @description The protocol version number carried in the most recently received 
*              EAPOL frame. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppLastEapolFrameVersion (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppLastEapolFrameVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortLastEapolFrameVersionGet (L7_UNIT_CURRENT,
                                            keydot1xPaePortNumberValue,
                                            &objdot1xSuppLastEapolFrameVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppLastEapolFrameVersion */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppLastEapolFrameVersionValue,
                    sizeof (objdot1xSuppLastEapolFrameVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppLastEapolFrameSource
*
* @purpose Get 'dot1xSuppLastEapolFrameSource'
*
* @description The source MAC address carried in the most recently received EAPOL 
*              frame. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppLastEapolFrameSource (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1xSuppLastEapolFrameSourceValue;
  L7_enetMacAddr_t macAddr;
  FPOBJ_TRACE_ENTER (bufp);
  memset(objdot1xSuppLastEapolFrameSourceValue, 0x00, sizeof(objdot1xSuppLastEapolFrameSourceValue));
  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortLastEapolFrameSourceGet (L7_UNIT_CURRENT,
                                           keydot1xPaePortNumberValue,
                                           (L7_uchar8 *)&macAddr);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
   memcpy(objdot1xSuppLastEapolFrameSourceValue,macAddr.addr, L7_ENET_MAC_ADDR_LEN);

  /* return the object value: dot1xSuppLastEapolFrameSource */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objdot1xSuppLastEapolFrameSourceValue,
                    sizeof(objdot1xSuppLastEapolFrameSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppPaeState
*
* @purpose Get 'dot1xSuppPaeState'
*
* @description The current state of the Supplicant PAE state machine (8.5.8). 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppPaeState (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppPaeStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xSupplicantPortPaeStateGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                   &objdot1xSuppPaeStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppPaeState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppPaeStateValue,
                           sizeof (objdot1xSuppPaeStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xSuppHeldPeriod
*
* @purpose Get 'dot1xSuppHeldPeriod'
*
* @description The value, in seconds, of the heldPeriod constant currently in 
*              use by the Supplicant PAE state machine (8.5.8.1.2). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppHeldPeriod (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppHeldPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xSupplicantPortHeldPeriodGet ( keydot1xPaePortNumberValue,
                                  &objdot1xSuppHeldPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppHeldPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppHeldPeriodValue,
                           sizeof (objdot1xSuppHeldPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xSuppHeldPeriod
*
* @purpose Set 'dot1xSuppHeldPeriod'
*
* @description The value, in seconds, of the heldPeriod constant currently in 
*              use by the Supplicant PAE state machine (8.5.8.1.2). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xSuppHeldPeriod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppHeldPeriodValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xSuppHeldPeriod */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objdot1xSuppHeldPeriodValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xSuppHeldPeriodValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xSupplicantPortHeldPeriodSet ( keydot1xPaePortNumberValue,
                                  objdot1xSuppHeldPeriodValue);
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
* @function fpObjGet_securitydot1x_dot1xSuppAuthPeriod
*
* @purpose Get 'dot1xSuppAuthPeriod'
*
* @description The value, in seconds, of the authPeriod constant currently in 
*              use by the Supplicant PAE state machine (8.5.8.1.2). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppAuthPeriod (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppAuthPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xSupplicantPortAuthPeriodGet ( keydot1xPaePortNumberValue,
                                   &objdot1xSuppAuthPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppAuthPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppAuthPeriodValue,
                           sizeof (objdot1xSuppAuthPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xSuppAuthPeriod
*
* @purpose Set 'dot1xSuppAuthPeriod'
*
* @description The value, in seconds, of the authPeriod constant currently in 
*              use by the Supplicant PAE state machine (8.5.8.1.2). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xSuppAuthPeriod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppAuthPeriodValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xSuppAuthPeriod */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objdot1xSuppAuthPeriodValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xSuppAuthPeriodValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xSupplicantPortAuthPeriodSet ( keydot1xPaePortNumberValue,
                                   objdot1xSuppAuthPeriodValue);
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
* @function fpObjGet_securitydot1x_dot1xSuppStartPeriod
*
* @purpose Get 'dot1xSuppStartPeriod'
*
* @description The value, in seconds, of the startPeriod constant currently in 
*              use by the Supplicant PAE state machine (8.5.8.1.2). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xSuppStartPeriod (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppStartPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xSupplicantPortStartPeriodGet ( keydot1xPaePortNumberValue,
                               &objdot1xSuppStartPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xSuppStartPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xSuppStartPeriodValue,
                           sizeof (objdot1xSuppStartPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xSuppStartPeriod
*
* @purpose Set 'dot1xSuppStartPeriod'
*
* @description The value, in seconds, of the startPeriod constant currently in 
*              use by the Supplicant PAE state machine (8.5.8.1.2). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xSuppStartPeriod (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xSuppStartPeriodValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xSuppStartPeriod */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objdot1xSuppStartPeriodValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xSuppStartPeriodValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xSupplicantPortStartPeriodSet ( keydot1xPaePortNumberValue,
                               objdot1xSuppStartPeriodValue);
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
* @function fpObjGet_securitydot1x_dot1xAuthEapolFramesRx
*
* @purpose Get 'dot1xAuthEapolFramesRx'
 *@description  [dot1xAuthEapolFramesRx] The number of EAPOL frames of any type
* that have been received by this Authenticator.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthEapolFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthEapolFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortEapolFramesRxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthEapolFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthEapolFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthEapolFramesRxValue,
                           sizeof (objdot1xAuthEapolFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthEapolFramesTx
*
* @purpose Get 'dot1xAuthEapolFramesTx'
 *@description  [dot1xAuthEapolFramesTx] The number of EAPOL frames of any type
* that have been transmitted by this Authenticator.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthEapolFramesTx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthEapolFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortEapolFramesTxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthEapolFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthEapolFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthEapolFramesTxValue,
                           sizeof (objdot1xAuthEapolFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthEapolStartFramesRx
*
* @purpose Get 'dot1xAuthEapolStartFramesRx'
 *@description  [dot1xAuthEapolStartFramesRx] The number of EAPOL Start frames
* that have been received by this Authenticator.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthEapolStartFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthEapolStartFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortEapolStartFramesRxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthEapolStartFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthEapolStartFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthEapolStartFramesRxValue,
                           sizeof (objdot1xAuthEapolStartFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthEapolLogoffFramesRx
*
* @purpose Get 'dot1xAuthEapolLogoffFramesRx'
 *@description  [dot1xAuthEapolLogoffFramesRx] The number of EAPOL Logoff frames
* that have been received by this Authenticator.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthEapolLogoffFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthEapolLogoffFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortEapolLogoffFramesRxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthEapolLogoffFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthEapolLogoffFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthEapolLogoffFramesRxValue,
                           sizeof (objdot1xAuthEapolLogoffFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthEapolRespIdFramesRx
*
* @purpose Get 'dot1xAuthEapolRespIdFramesRx'
 *@description  [dot1xAuthEapolRespIdFramesRx] The number of EAP Resp/Id frames
* that have been received by this Authenticator.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthEapolRespIdFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthEapolRespIdFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortEapRespIdFramesRxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthEapolRespIdFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthEapolRespIdFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthEapolRespIdFramesRxValue,
                           sizeof (objdot1xAuthEapolRespIdFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthEapolRespFramesRx
*
* @purpose Get 'dot1xAuthEapolRespFramesRx'
 *@description  [dot1xAuthEapolRespFramesRx] The number of valid EAP Response
* frames (other than Resp/Id frames) that have been received  by this
* Authenticator.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthEapolRespFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthEapolRespFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortEapResponseFramesRxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthEapolRespFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthEapolRespFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthEapolRespFramesRxValue,
                           sizeof (objdot1xAuthEapolRespFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthEapolReqIdFramesTx
*
* @purpose Get 'dot1xAuthEapolReqIdFramesTx'
 *@description  [dot1xAuthEapolReqIdFramesTx] The number of EAP Req/Id frames
* that have been transmitted by this Authenticator.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthEapolReqIdFramesTx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthEapolReqIdFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortEapReqIdFramesTxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthEapolReqIdFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthEapolReqIdFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthEapolReqIdFramesTxValue,
                           sizeof (objdot1xAuthEapolReqIdFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthEapolReqFramesTx
*
* @purpose Get 'dot1xAuthEapolReqFramesTx'
 *@description  [dot1xAuthEapolReqFramesTx] The number of EAP Request frames
* (other than Rq/Id frames) that have been transmitted  by this
* Authenticator.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthEapolReqFramesTx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthEapolReqFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortEapReqFramesTxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthEapolReqFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthEapolReqFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthEapolReqFramesTxValue,
                           sizeof (objdot1xAuthEapolReqFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthInvalidEapolFramesRx
*
* @purpose Get 'dot1xAuthInvalidEapolFramesRx'
 *@description  [dot1xAuthInvalidEapolFramesRx] The number of EAPOL frames that
* have been received by this Authenticator in which the frame type
* is not recognized.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthInvalidEapolFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthInvalidEapolFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortInvalidEapolFramesRxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthInvalidEapolFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthInvalidEapolFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthInvalidEapolFramesRxValue,
                           sizeof (objdot1xAuthInvalidEapolFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthEapLengthErrorFramesRx
*
* @purpose Get 'dot1xAuthEapLengthErrorFramesRx'
 *@description  [dot1xAuthEapLengthErrorFramesRx] The number of EAPOL frames
* that have been received by this Authenticator in which the Packet
* Body Length field (7.5.5) is invalid.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthEapLengthErrorFramesRx (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthEapLengthErrorFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortEapLengthErrorFramesRxGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthEapLengthErrorFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthEapLengthErrorFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthEapLengthErrorFramesRxValue,
                           sizeof (objdot1xAuthEapLengthErrorFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthLastEapolFrameVersion
*
* @purpose Get 'dot1xAuthLastEapolFrameVersion'
 *@description  [dot1xAuthLastEapolFrameVersion] The protocol version number
* carried in the most recently received EAPOL frame.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthLastEapolFrameVersion (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthLastEapolFrameVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortLastEapolFrameVersionGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthLastEapolFrameVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthLastEapolFrameVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthLastEapolFrameVersionValue,
                           sizeof (objdot1xAuthLastEapolFrameVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthLastEapolFrameSource
*
* @purpose Get 'dot1xAuthLastEapolFrameSource'
 *@description  [dot1xAuthLastEapolFrameSource] The source MAC address carried
* in the most recently received EAPOL frame.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthLastEapolFrameSource (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1xAuthLastEapolFrameSourceValue;


  L7_enetMacAddr_t emacAddr;
		
  FPOBJ_TRACE_ENTER (bufp);
 memset(objdot1xAuthLastEapolFrameSourceValue, 0x00, sizeof(objdot1xAuthLastEapolFrameSourceValue));

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortLastEapolFrameSourceGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                                                                         (L7_uchar8 *)&emacAddr);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memcpy(objdot1xAuthLastEapolFrameSourceValue,emacAddr.addr, L7_ENET_MAC_ADDR_LEN);

  /* return the object value: dot1xAuthLastEapolFrameSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1xAuthLastEapolFrameSourceValue,
                           strlen (objdot1xAuthLastEapolFrameSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xPaePortProtocolVersion
*
* @purpose Get 'dot1xPaePortProtocolVersion'
*
* @description The protocol version associated with this Port. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xPaePortProtocolVersion (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xPaePortProtocolVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortProtocolVersionGet (L7_UNIT_CURRENT,
                                      keydot1xPaePortNumberValue,
                                      &objdot1xPaePortProtocolVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xPaePortProtocolVersion */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xPaePortProtocolVersionValue,
                    sizeof (objdot1xPaePortProtocolVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xPaePortCapabilities
*
* @purpose Set 'dot1xPaePortCapabilities'
 *@description  [dot1xPaePortCapabilities] Indicates the PAE functionality that
* this Port supports and that may be managed through this MIB.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xPaePortCapabilities (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdot1xPaePortCapabilitiesValue;

  xLibU32_t keydot1xPaePortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xPaePortCapabilities */
  owa.len = sizeof (objdot1xPaePortCapabilitiesValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objdot1xPaePortCapabilitiesValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xPaePortCapabilitiesValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  owa.len = sizeof (keydot1xPaePortNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortPaeCapabilitiesSet(keydot1xPaePortNumberValue,
                              objdot1xPaePortCapabilitiesValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xPaePortCapabilities
*
* @purpose Get 'dot1xPaePortCapabilities'
 *@description  [dot1xPaePortCapabilities] Indicates the PAE functionality that
* this Port supports and that may be managed through this MIB.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xPaePortCapabilities (void *wap, void *bufp)
{

  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdot1xPaePortCapabilitiesValue;
  L7_uchar8 pVal;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  owa.len = sizeof (keydot1xPaePortNumberValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortPaeCapabilitiesGet(L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &pVal);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  objdot1xPaePortCapabilitiesValue = pVal;
  /* return the object value: dot1xPaePortCapabilities */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xPaePortCapabilitiesValue,
                           sizeof (objdot1xPaePortCapabilitiesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xPaePortInitialize
*
* @purpose Set 'dot1xPaePortInitialize'
*
* @description The initialization control for this Port. Setting this attribute 
*              TRUE causes the Port to be initialized. The attribute value 
*              reverts to FALSE once initialization has completed. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xPaePortInitialize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xPaePortInitializeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  xLibU32_t mode;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xPaePortInitialize */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objdot1xPaePortInitializeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xPaePortInitializeValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  if(objdot1xPaePortInitializeValue == L7_FALSE)
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xPortInitializeSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                 L7_TRUE);
  /*Find rue cause for failure */
   if(owa.l7rc  != L7_SUCCESS)
   {
        if (usmDbDot1xAdminModeGet(L7_UNIT_CURRENT, &mode) == L7_SUCCESS &&
          mode == L7_DISABLE )
        {
          /* Adminmode must be enabled */
	        owa.rc = XLIBRC_DOT1X_PORT_INITIALISE_FAILED_ADMIN;    /* Add error String */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
	   
        }
        else if ( usmDbDot1xPortOperControlModeGet(L7_UNIT_CURRENT, 
			keydot1xPaePortNumberValue, &mode) == L7_SUCCESS
               && mode != L7_DOT1X_PORT_AUTO)
        {
          /* Port mode must be auto */
          owa.rc = XLIBRC_DOT1X_PORT_INITIALISE_FAILED_CTRL_MODE;    /* Add error String */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
  }
  else
  {
    owa.l7rc = L7_SUCCESS;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_DOT1X_PORT_INITIALISE_FAILED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  } 
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xPaePortReauthenticate
*
* @purpose Get 'dot1xPaePortReauthenticate'
*
* @description The reauthentication control for this port. Setting this attribute 
*              TRUE causes the Authenticator PAE state machine for the 
*              Port to reauthenticate the Supplicant. Setting this attribute 
*              FALSE has no effect. This attribute always returns FALSE when 
*              it is read. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xPaePortReauthenticate (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xPaePortReauthenticateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);
  /*Always returns true */
  objdot1xPaePortReauthenticateValue = L7_FALSE;

  /* return the object value: dot1xPaePortReauthenticate */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xPaePortReauthenticateValue,
                    sizeof (objdot1xPaePortReauthenticateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xPaePortReauthenticate
*
* @purpose Set 'dot1xPaePortReauthenticate'
*
* @description The reauthentication control for this port. Setting this attribute 
*              TRUE causes the Authenticator PAE state machine for the 
*              Port to reauthenticate the Supplicant. Setting this attribute 
*              FALSE has no effect. This attribute always returns FALSE when 
*              it is read. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xPaePortReauthenticate (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xPaePortReauthenticateValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  xLibU32_t mode;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xPaePortReauthenticate */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objdot1xPaePortReauthenticateValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xPaePortReauthenticateValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

if(objdot1xPaePortReauthenticateValue  == L7_FALSE)
{
  owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
  /* set the value in application */
  if(objdot1xPaePortReauthenticateValue == L7_TRUE)
  {
     owa.l7rc =
       usmDbDot1xPortReauthenticateSet (L7_UNIT_CURRENT,
                                     keydot1xPaePortNumberValue,
                                     L7_TRUE);

      /*Find rue cause for failure */
      if(owa.l7rc  != L7_SUCCESS)
     {
        if (usmDbDot1xAdminModeGet(L7_UNIT_CURRENT, &mode) == L7_SUCCESS &&
          mode == L7_DISABLE )
        {
          /* Adminmode must be enabled */
	        owa.rc = XLIBRC_DOT1X_PORT_REAUTHENTICATE_FAILED_ADMIN;    /* TODO: Change if required */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
	   
        }
        else if ( usmDbDot1xPortOperControlModeGet(L7_UNIT_CURRENT, 
			keydot1xPaePortNumberValue, &mode) == L7_SUCCESS
               && mode != L7_DOT1X_PORT_AUTO)
        {
          /* Port mode must be auto */
          owa.rc = XLIBRC_DOT1X_PORT_REAUTHENTICATE_FAILED_CTRL_MODE;    /* TODO: Change if required */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
     }
	 
  }
  else
  {
    owa.l7rc = L7_SUCCESS;
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_DOT1X_PORT_REAUTHENTICATE_FAILED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xPaeSystemAuthControl
*
* @purpose Get 'dot1xPaeSystemAuthControl'
*
* @description The administrative enable/disable state for Port Access Control 
*              in a System. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xPaeSystemAuthControl (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xPaeSystemAuthControlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xAdminModeGet (L7_UNIT_CURRENT,
                            &objdot1xPaeSystemAuthControlValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xPaeSystemAuthControlValue,
                     sizeof (objdot1xPaeSystemAuthControlValue));

  /* return the object value: dot1xPaeSystemAuthControl */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xPaeSystemAuthControlValue,
                    sizeof (objdot1xPaeSystemAuthControlValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xPaeSystemAuthControl
*
* @purpose Set 'dot1xPaeSystemAuthControl'
*
* @description The administrative enable/disable state for Port Access Control 
*              in a System. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xPaeSystemAuthControl (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xPaeSystemAuthControlValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xPaeSystemAuthControl */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objdot1xPaeSystemAuthControlValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xPaeSystemAuthControlValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xAdminModeSet (L7_UNIT_CURRENT, objdot1xPaeSystemAuthControlValue);
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
* @function fpObjGet_securitydot1x_Dot1xGuestVlanId
*
* @purpose Get 'Dot1xGuestVlanId'
*
* @description Specifies the Guest Vlan of the interface. An interface will be 
*              moved to its Guest Vlan if its access is unsucessfully authenticated. 
*              A value of zero indicates no Guest Vlan configured 
*              for the interface. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xGuestVlanId (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xGuestVlanIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xAdvancedGuestPortsCfgGet (L7_UNIT_CURRENT,
                                        keydot1xPaePortNumberValue,
                                        &objDot1xGuestVlanIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xGuestVlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xGuestVlanIdValue,
                           sizeof (objDot1xGuestVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xGuestVlanId
*
* @purpose Set 'Dot1xGuestVlanId'
*
* @description Specifies the Guest Vlan of the interface. An interface will be 
*              moved to its Guest Vlan if its access is unsucessfully authenticated. 
*              A value of zero indicates no Guest Vlan configured 
*              for the interface. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xGuestVlanId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xGuestVlanIdValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xGuestVlanId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xGuestVlanIdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xGuestVlanIdValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xAdvancedGuestPortsCfgSet (L7_UNIT_CURRENT,
                                        keydot1xPaePortNumberValue,
                                        objDot1xGuestVlanIdValue);
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
* @function fpObjGet_securitydot1x_Dot1xGuestVlanIndexList
*
* @purpose Get 'Dot1xGuestVlanIndexList'
 *@description  [Dot1xGuestVlanIndexList] The VLAN-ID in integer format   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xGuestVlanIndexList (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xGuestVlanIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xAdvancedGuestPortsCfgGet (L7_UNIT_CURRENT,
                                        keydot1xPaePortNumberValue,
                                        &objDot1xGuestVlanIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xGuestVlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xGuestVlanIdValue,
                           sizeof (objDot1xGuestVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_securitydot1x_Dot1xGuestVlanIndexList
*
* @purpose List 'Dot1xGuestVlanIndexList'
 *@description  [Dot1xGuestVlanIndexList] The VLAN-ID in integer format   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_securitydot1x_Dot1xGuestVlanIndexList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexListValue;
  xLibU32_t nextObjVlanIndexListValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticMacFilteringVlanId */
  owa.rc = xLibFilterGet (wap,XOBJ_securitydot1x_Dot1xGuestVlanIndexList,
                   (xLibU8_t *) & objVlanIndexListValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    nextObjVlanIndexListValue = 0;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexListValue, owa.len);
    owa.l7rc = usmDbNextVlanGet(L7_UNIT_CURRENT, objVlanIndexListValue,
                        &nextObjVlanIndexListValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIndexListValue, owa.len);

  /* return the object value: StaticMacFilteringVlanId */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIndexListValue,
                    sizeof (objVlanIndexListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xGuestVlanIndexList
*
* @purpose Set 'Dot1xGuestVlanIndexList'
 *@description  [Dot1xGuestVlanIndexList] The VLAN-ID in integer format   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xGuestVlanIndexList (void *wap, void *bufp)
{
  /* Just return SUCCESS as set is handled in Dot1xGuestVlan Set Handler */
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xGuestVlanIdValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xGuestVlanId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xGuestVlanIdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xGuestVlanIdValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xAdvancedGuestPortsCfgSet (L7_UNIT_CURRENT,
                                        keydot1xPaePortNumberValue,
                                        objDot1xGuestVlanIdValue);
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
* @function fpObjGet_securitydot1x_Dot1xGuestVlanPeriod
*
* @purpose Get 'Dot1xGuestVlanPeriod'
*
* @description The value, in seconds, of the guestVlanPeriod constant currently 
*              in use for Guest Vlan Assignment for the interface . 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xGuestVlanPeriod (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xGuestVlanPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xAdvancedPortGuestVlanPeriodGet (L7_UNIT_CURRENT,
                                              keydot1xPaePortNumberValue,
                                              &objDot1xGuestVlanPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xGuestVlanPeriod */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xGuestVlanPeriodValue,
                    sizeof (objDot1xGuestVlanPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xGuestVlanPeriod
*
* @purpose Set 'Dot1xGuestVlanPeriod'
*
* @description The value, in seconds, of the guestVlanPeriod constant currently 
*              in use for Guest Vlan Assignment for the interface . 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xGuestVlanPeriod (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xGuestVlanPeriodValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xGuestVlanPeriod */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xGuestVlanPeriodValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xGuestVlanPeriodValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xAdvancedPortGuestVlanPeriodSet (L7_UNIT_CURRENT,
                                              keydot1xPaePortNumberValue,
                                              objDot1xGuestVlanPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*ObjectRemoved*/
/*******************************************************************************
* @function fpObjGet_securitydot1x_agentGuestVlanSupplMode
*
* @purpose Get 'agentGuestVlanSupplMode'
*
* @description Mode showing whether at the global level,Guest VLAN Supplicant 
*              Mode is enabled or not. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_agentGuestVlanSupplMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentGuestVlanSupplModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xAdvancedGuestVlanSupplicantModeGet (L7_UNIT_CURRENT,
                                                  &objagentGuestVlanSupplModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentGuestVlanSupplModeValue,
                     sizeof (objagentGuestVlanSupplModeValue));

  /* return the object value: agentGuestVlanSupplMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objagentGuestVlanSupplModeValue,
                           sizeof (objagentGuestVlanSupplModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_agentGuestVlanSupplMode
*
* @purpose Set 'agentGuestVlanSupplMode'
*
* @description Mode showing whether at the global level,Guest VLAN Supplicant 
*              Mode is enabled or not. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_agentGuestVlanSupplMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objagentGuestVlanSupplModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: agentGuestVlanSupplMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objagentGuestVlanSupplModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objagentGuestVlanSupplModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xAdvancedGuestVlanSupplicantModeSet (L7_UNIT_CURRENT,
                                                  objagentGuestVlanSupplModeValue);
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
* @function fpObjGet_securitydot1x_Dot1xRadiusVlanAssignment
*
* @purpose Get 'Dot1xRadiusVlanAssignment'
*
* @description Enable/Disable dot1x Vlan Assignment Support on the switch. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xRadiusVlanAssignment (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xRadiusVlanAssignmentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xVlanAssignmentModeGet (L7_UNIT_CURRENT,
                                     &objDot1xRadiusVlanAssignmentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xRadiusVlanAssignmentValue,
                     sizeof (objDot1xRadiusVlanAssignmentValue));

  /* return the object value: Dot1xRadiusVlanAssignment */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xRadiusVlanAssignmentValue,
                    sizeof (objDot1xRadiusVlanAssignmentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xRadiusVlanAssignment
*
* @purpose Set 'Dot1xRadiusVlanAssignment'
*
* @description Enable/Disable dot1x Vlan Assignment Support on the switch. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xRadiusVlanAssignment (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xRadiusVlanAssignmentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xRadiusVlanAssignment */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objDot1xRadiusVlanAssignmentValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xRadiusVlanAssignmentValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xVlanAssignmentModeSet (L7_UNIT_CURRENT,
                                     objDot1xRadiusVlanAssignmentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthPaeState
*
* @purpose Get 'dot1xAuthPaeState'
 *@description  [dot1xAuthPaeState] The current value of the Authenticator PAE
* state   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthPaeState (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthPaeStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortAuthPaeStateGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthPaeStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthPaeState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthPaeStateValue,
                           sizeof (objdot1xAuthPaeStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthQuietPeriod
*
* @purpose Get 'dot1xAuthQuietPeriod'
 *@description  [dot1xAuthQuietPeriod] The value, in seconds, of the heldPeriod
* constant currently in use by the  PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthQuietPeriod (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthQuietPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortQuietPeriodGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthQuietPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthQuietPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthQuietPeriodValue,
                           sizeof (objdot1xAuthQuietPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xAuthQuietPeriod
*
* @purpose Set 'dot1xAuthQuietPeriod'
 *@description  [dot1xAuthQuietPeriod] The value, in seconds, of the heldPeriod
* constant currently in use by the  PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xAuthQuietPeriod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthQuietPeriodValue;

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xAuthQuietPeriod */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objdot1xAuthQuietPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xAuthQuietPeriodValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortQuietPeriodSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              objdot1xAuthQuietPeriodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthTxPeriod
*
* @purpose Get 'dot1xAuthTxPeriod'
 *@description  [dot1xAuthTxPeriod] The value, in seconds, of the startPeriod
* constant currently in use by the  PAE state machine .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthTxPeriod (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthTxPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortTxPeriodGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthTxPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthTxPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthTxPeriodValue,
                           sizeof (objdot1xAuthTxPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xAuthTxPeriod
*
* @purpose Set 'dot1xAuthTxPeriod'
 *@description  [dot1xAuthTxPeriod] The value, in seconds, of the startPeriod
* constant currently in use by the  PAE state machine .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xAuthTxPeriod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthTxPeriodValue;

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xAuthTxPeriod */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objdot1xAuthTxPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xAuthTxPeriodValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortTxPeriodSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              objdot1xAuthTxPeriodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_dot1xAuthReAuthPeriod
*
* @purpose Get 'dot1xAuthReAuthPeriod'
 *@description  [dot1xAuthReAuthPeriod] The value, in seconds, of the authPeriod
* constant currently in use by the  PAE state machine .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_dot1xAuthReAuthPeriod (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthReAuthPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortReAuthPeriodGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objdot1xAuthReAuthPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xAuthReAuthPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xAuthReAuthPeriodValue,
                           sizeof (objdot1xAuthReAuthPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydot1x_dot1xAuthReAuthPeriod
*
* @purpose Set 'dot1xAuthReAuthPeriod'
 *@description  [dot1xAuthReAuthPeriod] The value, in seconds, of the authPeriod
* constant currently in use by the  PAE state machine .   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_dot1xAuthReAuthPeriod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xAuthReAuthPeriodValue;

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dot1xAuthReAuthPeriod */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objdot1xAuthReAuthPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdot1xAuthReAuthPeriodValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortReAuthPeriodSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              objdot1xAuthReAuthPeriodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAdminControlledDirections
*
* @purpose Get 'Dot1xPortAdminControlledDirections'
*
* @description the dot1x admin control direction on the specified port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAdminControlledDirections (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAdminControlledDirectionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAdminControlledDirectionsGet (L7_UNIT_CURRENT,
                                                keydot1xPaePortNumberValue,
                                                &objDot1xPortAdminControlledDirectionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAdminControlledDirections */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDot1xPortAdminControlledDirectionsValue,
                    sizeof (objDot1xPortAdminControlledDirectionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortAdminControlledDirections
*
* @purpose Set 'Dot1xPortAdminControlledDirections'
*
* @description the dot1x admin control direction on the specified port 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortAdminControlledDirections (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAdminControlledDirectionsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xPortAdminControlledDirections */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objDot1xPortAdminControlledDirectionsValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortAdminControlledDirectionsValue,
                     owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xPortAdminControlledDirectionsSet (L7_UNIT_CURRENT,
                                                keydot1xPaePortNumberValue,
                                                objDot1xPortAdminControlledDirectionsValue);
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
* @function fpObjGet_securitydot1x_Dot1xPortOperControlledDirections
*
* @purpose Get 'Dot1xPortOperControlledDirections'
*
* @description the dot1x Operational control direction on the specified port 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortOperControlledDirections (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortOperControlledDirectionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortOperControlledDirectionsGet (L7_UNIT_CURRENT,
                                               keydot1xPaePortNumberValue,
                                               &objDot1xPortOperControlledDirectionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortOperControlledDirections */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDot1xPortOperControlledDirectionsValue,
                    sizeof (objDot1xPortOperControlledDirectionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortControlMode
*
* @purpose Get 'Dot1xPortControlMode'
*
* @description the dot1x port control mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortControlMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortControlModeValue;
  L7_uchar8 cap = 0x0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  if((owa.l7rc = usmDbDot1xPortPaeCapabilitiesGet(L7_UNIT_CURRENT, keydot1xPaePortNumberValue, &cap)) == L7_SUCCESS)
  {
    if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
    {
  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortControlModeGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                  &objDot1xPortControlModeValue);
    }
    else if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
    {
      /* get the value from application */
      owa.l7rc =
      usmDbDot1xSupplicantPortControlModeGet( keydot1xPaePortNumberValue,
                                  &objDot1xPortControlModeValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortControlModeValue,
                           sizeof (objDot1xPortControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortControlMode
*
* @purpose Set 'Dot1xPortControlMode'
*
* @description the dot1x port control mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortControlMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortControlModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xPortControlMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xPortControlModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortControlModeValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xPortControlModeSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                  objDot1xPortControlModeValue);
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
* @function fpObjGet_securitydot1x_Dot1xPortOperControlMode
*
* @purpose Get 'Dot1xPortOperControlMode'
*
* @description Dot1x port operational control mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortOperControlMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortOperControlModeValue;
  L7_uchar8 cap = 0x0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  if((owa.l7rc = usmDbDot1xPortPaeCapabilitiesGet(L7_UNIT_CURRENT, keydot1xPaePortNumberValue, &cap)) == L7_SUCCESS)
  {
    if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
    {
  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortOperControlModeGet (L7_UNIT_CURRENT,
                                      keydot1xPaePortNumberValue,
                                      &objDot1xPortOperControlModeValue);
    }
    else if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
    {
      /* get the value from application */
      owa.l7rc =
      usmDbDot1xSupplicantPortControlModeGet( keydot1xPaePortNumberValue,
                                  &objDot1xPortOperControlModeValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortOperControlMode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortOperControlModeValue,
                    sizeof (objDot1xPortOperControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortSuppTimeout
*
* @purpose Get 'Dot1xPortSuppTimeout'
*
* @description Port support time out 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortSuppTimeout (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortSuppTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortSuppTimeoutGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                  &objDot1xPortSuppTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortSuppTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortSuppTimeoutValue,
                           sizeof (objDot1xPortSuppTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortSuppTimeout
*
* @purpose Set 'Dot1xPortSuppTimeout'
*
* @description Port support time out 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortSuppTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortSuppTimeoutValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xPortSuppTimeout */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xPortSuppTimeoutValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortSuppTimeoutValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xPortSuppTimeoutSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                  objDot1xPortSuppTimeoutValue);
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
* @function fpObjGet_securitydot1x_Dot1xPortServerTimeout
*
* @purpose Get 'Dot1xPortServerTimeout'
*
* @description Port server time out 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortServerTimeout (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortServerTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortServerTimeoutGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                    &objDot1xPortServerTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortServerTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortServerTimeoutValue,
                           sizeof (objDot1xPortServerTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortServerTimeout
*
* @purpose Set 'Dot1xPortServerTimeout'
*
* @description Port server time out 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortServerTimeout (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortServerTimeoutValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xPortServerTimeout */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xPortServerTimeoutValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortServerTimeoutValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xPortServerTimeoutSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                    objDot1xPortServerTimeoutValue);
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
* @function fpObjGet_securitydot1x_Dot1xPortMaxReq
*
* @purpose Get 'Dot1xPortMaxReq'
*
* @description Maximum requests 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortMaxReq (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortMaxReqValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortMaxReqGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                             &objDot1xPortMaxReqValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortMaxReq */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortMaxReqValue,
                           sizeof (objDot1xPortMaxReqValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortMaxReq
*
* @purpose Set 'Dot1xPortMaxReq'
*
* @description Maximum requests 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortMaxReq (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortMaxReqValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xPortMaxReq */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xPortMaxReqValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortMaxReqValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xPortMaxReqSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                             objDot1xPortMaxReqValue);
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
* @function fpObjGet_securitydot1x_Dot1xPortMaxUsers
*
* @purpose Get 'Dot1xPortMaxUsers'
*
* @description [Dot1xPortMaxUsers] Maximum Users
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortMaxUsers (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortMaxUsersValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortMaxUsersGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objDot1xPortMaxUsersValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortMaxUsers */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortMaxUsersValue,
                           sizeof (objDot1xPortMaxUsersValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortMaxUsers
*
* @purpose Set 'Dot1xPortMaxUsers'
*
* @description [Dot1xPortMaxUsers] Maximum Users
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortMaxUsers (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortMaxUsersValue;

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xPortMaxUsers */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xPortMaxUsersValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortMaxUsersValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortMaxUsersSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              objDot1xPortMaxUsersValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortReAuthEnabled
*
* @purpose Get 'Dot1xPortReAuthEnabled'
*
* @description port re-authentication 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortReAuthEnabled (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortReAuthEnabledValue;
  L7_uchar8 cap = 0x0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  if((owa.l7rc = usmDbDot1xPortPaeCapabilitiesGet(L7_UNIT_CURRENT, keydot1xPaePortNumberValue, &cap)) == L7_SUCCESS)
  {
    if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
    {
  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortReAuthEnabledGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                    &objDot1xPortReAuthEnabledValue);
    }
    else if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
    {
      /* get the value from application */
      owa.l7rc = L7_FAILURE;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortReAuthEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortReAuthEnabledValue,
                           sizeof (objDot1xPortReAuthEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortReAuthEnabled
*
* @purpose Set 'Dot1xPortReAuthEnabled'
*
* @description port re-authentication 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortReAuthEnabled (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortReAuthEnabledValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xPortReAuthEnabled */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xPortReAuthEnabledValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortReAuthEnabledValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xPortReAuthEnabledSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                    objDot1xPortReAuthEnabledValue);
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
* @function fpObjGet_securitydot1x_Dot1xPortKeyTransmissionEnabled
*
* @purpose Get 'Dot1xPortKeyTransmissionEnabled'
*
* @description key transmission 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortKeyTransmissionEnabled (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortKeyTransmissionEnabledValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortKeyTransmissionEnabledGet (L7_UNIT_CURRENT,
                                             keydot1xPaePortNumberValue,
                                             &objDot1xPortKeyTransmissionEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortKeyTransmissionEnabled */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDot1xPortKeyTransmissionEnabledValue,
                    sizeof (objDot1xPortKeyTransmissionEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortKeyTransmissionEnabled
*
* @purpose Set 'Dot1xPortKeyTransmissionEnabled'
*
* @description key transmission 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortKeyTransmissionEnabled (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortKeyTransmissionEnabledValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xPortKeyTransmissionEnabled */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objDot1xPortKeyTransmissionEnabledValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortKeyTransmissionEnabledValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xPortKeyTransmissionEnabledSet (L7_UNIT_CURRENT,
                                             keydot1xPaePortNumberValue,
                                             objDot1xPortKeyTransmissionEnabledValue);
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
* @function fpObjGet_securitydot1x_Dot1xPortBackendAuthState
*
* @purpose Get 'Dot1xPortBackendAuthState'
*
* @description backend authenticatation statte 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortBackendAuthState (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortBackendAuthStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortBackendAuthStateGet (L7_UNIT_CURRENT,
                                       keydot1xPaePortNumberValue,
                                       &objDot1xPortBackendAuthStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortBackendAuthState */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortBackendAuthStateValue,
                    sizeof (objDot1xPortBackendAuthStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortStatsClear
*
* @purpose Set 'Dot1xPortStatsClear'
*
* @description clear the statistics 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortStatsClear (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortStatsClearValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xPortStatsClear */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xPortStatsClearValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortStatsClearValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1xPortStatsClear (L7_UNIT_CURRENT, keydot1xPaePortNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    objDot1xPortStatsClearValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objDot1xPortStatsClearValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xPortStatsClearAll
*
* @purpose Set 'Dot1xPortStatsClearAll'
*
* @description clear the statistics on all the ports
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xPortStatsClearAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortStatsClearValue;
  L7_uint32 u, s, p;
  L7_uint32 interface, nextInterface;
  L7_int32  retVal;
  FPOBJ_TRACE_ENTER (bufp);


  /* retrieve object: Dot1xPortStatsClear */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1xPortStatsClearValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xPortStatsClearValue, owa.len);


  /* set the value in application */
  if (L7_SUCCESS != usmDbValidIntIfNumFirstGet(&interface))
  {
    objDot1xPortStatsClearValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;      
  }    
  owa.l7rc = L7_SUCCESS;
  while (interface != 0)
  {
    if ((usmDbVisibleInterfaceCheck(L7_UNIT_CURRENT, interface, &retVal) == L7_SUCCESS) &&
        (usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS) &&
        (usmDbDot1xInterfaceValidate(L7_UNIT_CURRENT, interface) == L7_SUCCESS))
    {
      owa.l7rc = usmDbDot1xPortStatsClear(L7_UNIT_CURRENT, interface);
    }

    if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
    {
      interface = nextInterface;
    }
    else
    {
      interface = 0;
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    objDot1xPortStatsClearValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objDot1xPortStatsClearValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthControlledPortStatus
*
* @purpose Get 'Dot1xPortAuthControlledPortStatus'
*
* @description authentication controlled port status 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthControlledPortStatus (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthControlledPortStatusValue;
  L7_uchar8 cap = 0x0, Status;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  if((owa.l7rc = usmDbDot1xPortPaeCapabilitiesGet(L7_UNIT_CURRENT, keydot1xPaePortNumberValue, &cap)) == L7_SUCCESS)
  {
    if (cap & L7_DOT1X_PAE_PORT_AUTH_CAPABLE)
    {
  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthControlledPortStatusGet (L7_UNIT_CURRENT,
                                               keydot1xPaePortNumberValue,
                                               &objDot1xPortAuthControlledPortStatusValue);
    }
    else if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
    {
       owa.l7rc = usmDbDot1xSupplicantPortStatusGet(keydot1xPaePortNumberValue, &Status); 
       objDot1xPortAuthControlledPortStatusValue = Status;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthControlledPortStatus */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDot1xPortAuthControlledPortStatusValue,
                    sizeof (objDot1xPortAuthControlledPortStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xInterfaceValidate
*
* @purpose Get 'Dot1xInterfaceValidate'
*
* @description validate to check if interfac eis dot1x enabled 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xInterfaceValidate (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xInterfaceValidateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xInterfaceValidate (L7_UNIT_CURRENT, keydot1xPaePortNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    objDot1xInterfaceValidateValue = L7_FALSE;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objDot1xInterfaceValidateValue = L7_TRUE;

  /* return the object value: Dot1xInterfaceValidate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xInterfaceValidateValue,
                           sizeof (objDot1xInterfaceValidateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthEntersConnecting
*
* @purpose Get 'Dot1xPortAuthEntersConnecting'
*
* @description auth enters connecting 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthEntersConnecting (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthEntersConnectingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthEntersConnectingGet (L7_UNIT_CURRENT,
                                           keydot1xPaePortNumberValue,
                                           &objDot1xPortAuthEntersConnectingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthEntersConnecting */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortAuthEntersConnectingValue,
                    sizeof (objDot1xPortAuthEntersConnectingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthEapLogoffsWhileConnecting
*
* @purpose Get 'Dot1xPortAuthEapLogoffsWhileConnecting'
*
* @description Auth Eap Logoffs While Connecting 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthEapLogoffsWhileConnecting (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthEapLogoffsWhileConnectingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthEapLogoffsWhileConnectingGet (L7_UNIT_CURRENT,
                                                    keydot1xPaePortNumberValue,
                                                    &objDot1xPortAuthEapLogoffsWhileConnectingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthEapLogoffsWhileConnecting */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthEapLogoffsWhileConnectingValue,
                    sizeof (objDot1xPortAuthEapLogoffsWhileConnectingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthEntersAuthenticating
*
* @purpose Get 'Dot1xPortAuthEntersAuthenticating'
*
* @description Auth Enters Authenticating 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthEntersAuthenticating (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthEntersAuthenticatingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthEntersAuthenticatingGet (L7_UNIT_CURRENT,
                                               keydot1xPaePortNumberValue,
                                               &objDot1xPortAuthEntersAuthenticatingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthEntersAuthenticating */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDot1xPortAuthEntersAuthenticatingValue,
                    sizeof (objDot1xPortAuthEntersAuthenticatingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthAuthSuccessWhileAuthenticating
*
* @purpose Get 'Dot1xPortAuthAuthSuccessWhileAuthenticating'
*
* @description Success While Authenticating 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_securitydot1x_Dot1xPortAuthAuthSuccessWhileAuthenticating (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthAuthSuccessWhileAuthenticatingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthAuthSuccessWhileAuthenticatingGet (L7_UNIT_CURRENT,
                                                         keydot1xPaePortNumberValue,
                                                         &objDot1xPortAuthAuthSuccessWhileAuthenticatingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthAuthSuccessWhileAuthenticating */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthAuthSuccessWhileAuthenticatingValue,
                    sizeof
                    (objDot1xPortAuthAuthSuccessWhileAuthenticatingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthAuthTimeoutsWhileAuthenticating
*
* @purpose Get 'Dot1xPortAuthAuthTimeoutsWhileAuthenticating'
*
* @description Timeouts While Authenticating 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_securitydot1x_Dot1xPortAuthAuthTimeoutsWhileAuthenticating (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthAuthTimeoutsWhileAuthenticatingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthAuthTimeoutsWhileAuthenticatingGet (L7_UNIT_CURRENT,
                                                          keydot1xPaePortNumberValue,
                                                          &objDot1xPortAuthAuthTimeoutsWhileAuthenticatingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthAuthTimeoutsWhileAuthenticating */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthAuthTimeoutsWhileAuthenticatingValue,
                    sizeof
                    (objDot1xPortAuthAuthTimeoutsWhileAuthenticatingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthAuthFailWhileAuthenticating
*
* @purpose Get 'Dot1xPortAuthAuthFailWhileAuthenticating'
*
* @description Failed While Authenticating 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthAuthFailWhileAuthenticating (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthAuthFailWhileAuthenticatingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthAuthFailWhileAuthenticatingGet (L7_UNIT_CURRENT,
                                                      keydot1xPaePortNumberValue,
                                                      &objDot1xPortAuthAuthFailWhileAuthenticatingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthAuthFailWhileAuthenticating */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthAuthFailWhileAuthenticatingValue,
                    sizeof (objDot1xPortAuthAuthFailWhileAuthenticatingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthAuthReauthsWhileAuthenticating
*
* @purpose Get 'Dot1xPortAuthAuthReauthsWhileAuthenticating'
*
* @description Reauths While Authenticating 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_securitydot1x_Dot1xPortAuthAuthReauthsWhileAuthenticating (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthAuthReauthsWhileAuthenticatingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthAuthReauthsWhileAuthenticatingGet (L7_UNIT_CURRENT,
                                                         keydot1xPaePortNumberValue,
                                                         &objDot1xPortAuthAuthReauthsWhileAuthenticatingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthAuthReauthsWhileAuthenticating */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthAuthReauthsWhileAuthenticatingValue,
                    sizeof
                    (objDot1xPortAuthAuthReauthsWhileAuthenticatingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthAuthEapStartsWhileAuthenticating
*
* @purpose Get 'Dot1xPortAuthAuthEapStartsWhileAuthenticating'
*
* @description Eap Starts While Authenticating 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_securitydot1x_Dot1xPortAuthAuthEapStartsWhileAuthenticating (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthAuthEapStartsWhileAuthenticatingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthAuthEapStartsWhileAuthenticatingGet (L7_UNIT_CURRENT,
                                                           keydot1xPaePortNumberValue,
                                                           &objDot1xPortAuthAuthEapStartsWhileAuthenticatingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthAuthEapStartsWhileAuthenticating */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthAuthEapStartsWhileAuthenticatingValue,
                    sizeof
                    (objDot1xPortAuthAuthEapStartsWhileAuthenticatingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthAuthEapLogoffWhileAuthenticating
*
* @purpose Get 'Dot1xPortAuthAuthEapLogoffWhileAuthenticating'
*
* @description Eap Logoff While Authenticating 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_securitydot1x_Dot1xPortAuthAuthEapLogoffWhileAuthenticating (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthAuthEapLogoffWhileAuthenticatingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthAuthEapLogoffWhileAuthenticatingGet (L7_UNIT_CURRENT,
                                                           keydot1xPaePortNumberValue,
                                                           &objDot1xPortAuthAuthEapLogoffWhileAuthenticatingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthAuthEapLogoffWhileAuthenticating */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthAuthEapLogoffWhileAuthenticatingValue,
                    sizeof
                    (objDot1xPortAuthAuthEapLogoffWhileAuthenticatingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthAuthReauthsWhileAuthenticated
*
* @purpose Get 'Dot1xPortAuthAuthReauthsWhileAuthenticated'
*
* @description Reauths While Authenticated 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthAuthReauthsWhileAuthenticated (void
                                                                            *wap,
                                                                            void
                                                                            *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthAuthReauthsWhileAuthenticatedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthAuthReauthsWhileAuthenticatedGet (L7_UNIT_CURRENT,
                                                        keydot1xPaePortNumberValue,
                                                        &objDot1xPortAuthAuthReauthsWhileAuthenticatedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthAuthReauthsWhileAuthenticated */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthAuthReauthsWhileAuthenticatedValue,
                    sizeof
                    (objDot1xPortAuthAuthReauthsWhileAuthenticatedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthAuthEapStartsWhileAuthenticated
*
* @purpose Get 'Dot1xPortAuthAuthEapStartsWhileAuthenticated'
*
* @description Eap Starts While Authenticated 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_securitydot1x_Dot1xPortAuthAuthEapStartsWhileAuthenticated (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthAuthEapStartsWhileAuthenticatedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthAuthEapStartsWhileAuthenticatedGet (L7_UNIT_CURRENT,
                                                          keydot1xPaePortNumberValue,
                                                          &objDot1xPortAuthAuthEapStartsWhileAuthenticatedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthAuthEapStartsWhileAuthenticated */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthAuthEapStartsWhileAuthenticatedValue,
                    sizeof
                    (objDot1xPortAuthAuthEapStartsWhileAuthenticatedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthAuthEapLogoffWhileAuthenticated
*
* @purpose Get 'Dot1xPortAuthAuthEapLogoffWhileAuthenticated'
*
* @description Eap Logof fWhile Authenticated 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_securitydot1x_Dot1xPortAuthAuthEapLogoffWhileAuthenticated (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthAuthEapLogoffWhileAuthenticatedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthAuthEapLogoffWhileAuthenticatedGet (L7_UNIT_CURRENT,
                                                          keydot1xPaePortNumberValue,
                                                          &objDot1xPortAuthAuthEapLogoffWhileAuthenticatedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthAuthEapLogoffWhileAuthenticated */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthAuthEapLogoffWhileAuthenticatedValue,
                    sizeof
                    (objDot1xPortAuthAuthEapLogoffWhileAuthenticatedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthBackendResponses
*
* @purpose Get 'Dot1xPortAuthBackendResponses'
*
* @description Backend Responses 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthBackendResponses (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthBackendResponsesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthBackendResponsesGet (L7_UNIT_CURRENT,
                                           keydot1xPaePortNumberValue,
                                           &objDot1xPortAuthBackendResponsesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthBackendResponses */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortAuthBackendResponsesValue,
                    sizeof (objDot1xPortAuthBackendResponsesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthBackendAccessChallenges
*
* @purpose Get 'Dot1xPortAuthBackendAccessChallenges'
*
* @description Backend Access Challenges 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthBackendAccessChallenges (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthBackendAccessChallengesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthBackendAccessChallengesGet (L7_UNIT_CURRENT,
                                                  keydot1xPaePortNumberValue,
                                                  &objDot1xPortAuthBackendAccessChallengesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthBackendAccessChallenges */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDot1xPortAuthBackendAccessChallengesValue,
                    sizeof (objDot1xPortAuthBackendAccessChallengesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthBackendOtherRequestsToSupplicant
*
* @purpose Get 'Dot1xPortAuthBackendOtherRequestsToSupplicant'
*
* @description Backend Other Requests To Supplicant 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_securitydot1x_Dot1xPortAuthBackendOtherRequestsToSupplicant (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthBackendOtherRequestsToSupplicantValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthBackendOtherRequestsToSupplicantGet (L7_UNIT_CURRENT,
                                                           keydot1xPaePortNumberValue,
                                                           &objDot1xPortAuthBackendOtherRequestsToSupplicantValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthBackendOtherRequestsToSupplicant */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthBackendOtherRequestsToSupplicantValue,
                    sizeof
                    (objDot1xPortAuthBackendOtherRequestsToSupplicantValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthBackendNonNakResponsesFromSupplicant
*
* @purpose Get 'Dot1xPortAuthBackendNonNakResponsesFromSupplicant'
*
* @description Backend NonNak Responses From Supplicant 
*              
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_securitydot1x_Dot1xPortAuthBackendNonNakResponsesFromSupplicant (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthBackendNonNakResponsesFromSupplicantValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthBackendNonNakResponsesFromSupplicantGet (L7_UNIT_CURRENT,
                                                               keydot1xPaePortNumberValue,
                                                               &objDot1xPortAuthBackendNonNakResponsesFromSupplicantValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthBackendNonNakResponsesFromSupplicant */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDot1xPortAuthBackendNonNakResponsesFromSupplicantValue,
                    sizeof
                    (objDot1xPortAuthBackendNonNakResponsesFromSupplicantValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthBackendAuthSuccesses
*
* @purpose Get 'Dot1xPortAuthBackendAuthSuccesses'
*
* @description Backend Auth Successes 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthBackendAuthSuccesses (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthBackendAuthSuccessesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthBackendAuthSuccessesGet (L7_UNIT_CURRENT,
                                               keydot1xPaePortNumberValue,
                                               &objDot1xPortAuthBackendAuthSuccessesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthBackendAuthSuccesses */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDot1xPortAuthBackendAuthSuccessesValue,
                    sizeof (objDot1xPortAuthBackendAuthSuccessesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortAuthBackendAuthFails
*
* @purpose Get 'Dot1xPortAuthBackendAuthFails'
*
* @description Backend Auth Fails 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortAuthBackendAuthFails (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortAuthBackendAuthFailsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortAuthBackendAuthFailsGet (L7_UNIT_CURRENT,
                                           keydot1xPaePortNumberValue,
                                           &objDot1xPortAuthBackendAuthFailsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortAuthBackendAuthFails */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortAuthBackendAuthFailsValue,
                    sizeof (objDot1xPortAuthBackendAuthFailsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortVlanAssigned
*
* @purpose Get 'Dot1xPortVlanAssigned'
*
* @description Check if the given interface is assigned to vlan 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortVlanAssigned (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortVlanAssignedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                          (xLibU8_t *) & keydot1xPaePortNumberValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1xPortVlanAssignedGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                                   &objDot1xPortVlanAssignedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortVlanAssigned */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortVlanAssignedValue,
                           sizeof (objDot1xPortVlanAssignedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#endif
/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xRadiusVlanAssignment
*
* @purpose Get 'Dot1xRadiusVlanAssignment'
 *@description  [Dot1xRadiusVlanAssignment] Enable/Disable dot1x Vlan Assignment
* Support on the switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xRadiusVlanAssignment (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xRadiusVlanAssignmentValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDot1xVlanAssignmentModeGet (L7_UNIT_CURRENT, &objDot1xRadiusVlanAssignmentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDot1xRadiusVlanAssignmentValue,
                     sizeof (objDot1xRadiusVlanAssignmentValue));

  /* return the object value: Dot1xRadiusVlanAssignment */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xRadiusVlanAssignmentValue,
                           sizeof (objDot1xRadiusVlanAssignmentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xRadiusVlanAssignment
*
* @purpose Set 'Dot1xRadiusVlanAssignment'
 *@description  [Dot1xRadiusVlanAssignment] Enable/Disable dot1x Vlan Assignment
* Support on the switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xRadiusVlanAssignment (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xRadiusVlanAssignmentValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xRadiusVlanAssignment */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xRadiusVlanAssignmentValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xRadiusVlanAssignmentValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDot1xVlanAssignmentModeSet (L7_UNIT_CURRENT, objDot1xRadiusVlanAssignmentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xUnauthenticatedVlan
*
* @purpose Get 'Dot1xUnauthenticatedVlan'
 *@description  [Dot1xUnauthenticatedVlan] Specifies the Unauthenticated Vlan of
* the port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xUnauthenticatedVlan (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xUnauthenticatedVlanValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortUnauthenticatedVlanGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objDot1xUnauthenticatedVlanValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xUnauthenticatedVlan */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xUnauthenticatedVlanValue,
                           sizeof (objDot1xUnauthenticatedVlanValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xUnauthenticatedVlan
*
* @purpose Set 'Dot1xUnauthenticatedVlan'
 *@description  [Dot1xUnauthenticatedVlan] Specifies the Unauthenticated Vlan of
* the port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xUnauthenticatedVlan (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xUnauthenticatedVlanValue;

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xUnauthenticatedVlan */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xUnauthenticatedVlanValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xUnauthenticatedVlanValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortUnauthenticatedVlanSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              objDot1xUnauthenticatedVlanValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xUnauthenticatedVlanList
*
* @purpose Get 'Dot1xUnauthenticatedVlanList'
 *@description  [Dot1xUnauthenticatedVlanList] The VLAN-ID in integer format   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xUnauthenticatedVlanList (void *wap, void *bufp)
{
   fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xUnauthenticatedVlanValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortUnauthenticatedVlanGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              &objDot1xUnauthenticatedVlanValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xUnauthenticatedVlan */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xUnauthenticatedVlanValue,
                           sizeof (objDot1xUnauthenticatedVlanValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_securitydot1x_Dot1xUnauthenticatedVlanList
*
* @purpose List 'Dot1xUnauthenticatedVlanList'
 *@description  [Dot1xUnauthenticatedVlanList] The VLAN-ID in integer format   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_securitydot1x_Dot1xUnauthenticatedVlanList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIndexListValue;
  xLibU32_t nextObjVlanIndexListValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: StaticMacFilteringVlanId */
  owa.rc = xLibFilterGet (wap,XOBJ_securitydot1x_Dot1xUnauthenticatedVlanList,
                   (xLibU8_t *) & objVlanIndexListValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    nextObjVlanIndexListValue = 0;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objVlanIndexListValue, owa.len);
    owa.l7rc = usmDbNextVlanGet(L7_UNIT_CURRENT, objVlanIndexListValue,
                        &nextObjVlanIndexListValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjVlanIndexListValue, owa.len);

  /* return the object value: StaticMacFilteringVlanId */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjVlanIndexListValue,
                    sizeof (objVlanIndexListValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xUnauthenticatedVlanList
*
* @purpose Set 'Dot1xUnauthenticatedVlanList'
 *@description  [Dot1xUnauthenticatedVlanList] The VLAN-ID in integer format   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xUnauthenticatedVlanList (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xUnauthenticatedVlanValue;

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xUnauthenticatedVlan */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xUnauthenticatedVlanValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xUnauthenticatedVlanValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortUnauthenticatedVlanSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue,
                              objDot1xUnauthenticatedVlanValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xMaxUsers
*
* @purpose Get 'Dot1xMaxUsers'
 *@description  [Dot1xMaxUsers] Specifies the maximum users or clients that can
* authenticate on this port when the port control mode is macBased.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xMaxUsers (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xMaxUsersValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortMaxUsersGet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue, &objDot1xMaxUsersValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xMaxUsers */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xMaxUsersValue,
                           sizeof (objDot1xMaxUsersValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydot1x_Dot1xMaxUsers
*
* @purpose Set 'Dot1xMaxUsers'
 *@description  [Dot1xMaxUsers] Specifies the maximum users or clients that can
* authenticate on this port when the port control mode is macBased.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydot1x_Dot1xMaxUsers (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xMaxUsersValue;

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot1xMaxUsers */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDot1xMaxUsersValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1xMaxUsersValue, owa.len);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortMaxUsersSet (L7_UNIT_CURRENT, keydot1xPaePortNumberValue, objDot1xMaxUsersValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortVlanAssigned
*
* @purpose Get 'Dot1xPortVlanAssigned'
 *@description  [Dot1xPortVlanAssigned] Specifies the vlan the port is assigned
* to by Dot1x.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortVlanAssigned (void *wap, void *bufp)
{
  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortVlanAssignedValue;

  L7_DOT1X_PORT_CONTROL_t temp_val;
  xLibU32_t temp_val2;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  if ((usmDbDot1xPortControlModeGet(L7_UNIT_CURRENT,keydot1xPaePortNumberValue,&temp_val)== L7_SUCCESS )&&
      (temp_val != L7_DOT1X_PORT_AUTO)) 
  {
      objDot1xPortVlanAssignedValue = 0;
  }
  else
  {
    owa.l7rc = usmDbDot1xPortVlanAssignedGet(L7_UNIT_CURRENT, keydot1xPaePortNumberValue, &temp_val2);

    if (owa.l7rc == L7_SUCCESS)
    {
      objDot1xPortVlanAssignedValue = temp_val2;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortVlanAssigned */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortVlanAssignedValue,
                           sizeof (objDot1xPortVlanAssignedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortVlanAssignedReason
*
* @purpose Get 'Dot1xPortVlanAssignedReason'
 *@description  [Dot1xPortVlanAssignedReason] Reason the port is assigned to the
* vlan specified by agentDot1xPortVlanAssigned.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortVlanAssignedReason (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortVlanAssignedReasonValue;

  L7_DOT1X_PORT_CONTROL_t port_control_val;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  if ((usmDbDot1xPortControlModeGet(L7_UNIT_CURRENT,keydot1xPaePortNumberValue,&port_control_val)==L7_SUCCESS) &&
      (port_control_val != L7_DOT1X_PORT_AUTO)) 
  {
      objDot1xPortVlanAssignedReasonValue = L7_DOT1X_NOT_ASSIGNED;
  }
  else
  {
    owa.l7rc = usmDbDot1xPortVlanAssignedReasonGet(L7_UNIT_CURRENT, keydot1xPaePortNumberValue, 
                                                                                &objDot1xPortVlanAssignedReasonValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortVlanAssignedReason */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortVlanAssignedReasonValue,
                           sizeof (objDot1xPortVlanAssignedReasonValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortSessionTimeout
*
* @purpose Get 'Dot1xPortSessionTimeout'
 *@description  [Dot1xPortSessionTimeout] Specifies the session timeout value
* assigned by the Radius server for this port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortSessionTimeout (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortSessionTimeoutValue;

  L7_DOT1X_PORT_CONTROL_t temp_val;
  xLibU32_t temp_val2;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  if ((usmDbDot1xPortControlModeGet(L7_UNIT_CURRENT,keydot1xPaePortNumberValue,&temp_val)== L7_SUCCESS )&&
      (temp_val != L7_DOT1X_PORT_AUTO)) 
  {
      objDot1xPortSessionTimeoutValue = 0;
  }
  else
  {
    owa.l7rc = usmDbDot1xPortSessionTimeoutGet(L7_UNIT_CURRENT, keydot1xPaePortNumberValue, &temp_val2);

    if (owa.l7rc == L7_SUCCESS)
    {
      objDot1xPortSessionTimeoutValue = temp_val2;
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortSessionTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortSessionTimeoutValue,
                           sizeof (objDot1xPortSessionTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xPortTerminationAction
*
* @purpose Get 'Dot1xPortTerminationAction'
 *@description  [Dot1xPortTerminationAction] Specifies the session termination
* action assigned by the Radius Server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xPortTerminationAction (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xPaePortNumber = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xPaePortNumberValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xPortTerminationActionValue;

  L7_DOT1X_PORT_CONTROL_t port_control_val;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPaePortNumber */
  kwadot1xPaePortNumber.rc = xLibFilterGet (wap, XOBJ_securitydot1x_dot1xPaePortNumber,
                                            (xLibU8_t *) & keydot1xPaePortNumberValue,
                                            &kwadot1xPaePortNumber.len);
  if (kwadot1xPaePortNumber.rc != XLIBRC_SUCCESS)
  {
    kwadot1xPaePortNumber.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xPaePortNumber);
    return kwadot1xPaePortNumber.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xPaePortNumberValue, kwadot1xPaePortNumber.len);

  /* get the value from application */
  if ((usmDbDot1xPortControlModeGet(L7_UNIT_CURRENT,keydot1xPaePortNumberValue,&port_control_val)==L7_SUCCESS) &&
      (port_control_val != L7_DOT1X_PORT_AUTO)) 
  {
      objDot1xPortTerminationActionValue = L7_DOT1X_TERMINATION_ACTION_DEFAULT;
  }
  else
  {
    owa.l7rc = usmDbDot1xPortTerminationActionGet(L7_UNIT_CURRENT, keydot1xPaePortNumberValue, 
                                                                                &objDot1xPortTerminationActionValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xPortTerminationAction */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xPortTerminationActionValue,
                           sizeof (objDot1xPortTerminationActionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientMacAddress
*
* @purpose Get 'Dot1xClientMacAddress'
 *@description  [Dot1xClientMacAddress] Specifies the client MAC address of the
* client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientMacAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objDot1xClientMacAddressValue;
  xLibStr6_t nextObjDot1xClientMacAddressValue;
  xLibU32_t lIntfNum;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                          (xLibU8_t *) objDot1xClientMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memset(nextObjDot1xClientMacAddressValue, 0, sizeof(nextObjDot1xClientMacAddressValue));
    lIntfNum = 0;
    
    owa.l7rc = usmDbDot1xClientMacAddressNextGet (L7_UNIT_CURRENT, nextObjDot1xClientMacAddressValue,&lIntfNum);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDot1xClientMacAddressValue, owa.len);

    memcpy(nextObjDot1xClientMacAddressValue, objDot1xClientMacAddressValue, sizeof(nextObjDot1xClientMacAddressValue));
    
    owa.l7rc = usmDbDot1xClientMacAddressNextGet (L7_UNIT_CURRENT, nextObjDot1xClientMacAddressValue,&lIntfNum);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDot1xClientMacAddressValue, owa.len);

  /* return the object value: Dot1xClientMacAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDot1xClientMacAddressValue,
                           sizeof (objDot1xClientMacAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xLogicalPort
*
* @purpose Get 'Dot1xLogicalPort'
 *@description  [Dot1xLogicalPort] Specifies the client MAC address of the
* client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xLogicalPort (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xLogicalPortValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &objDot1xLogicalPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xLogicalPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xLogicalPortValue,
                           sizeof (objDot1xLogicalPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xInterface
*
* @purpose Get 'Dot1xInterface'
 *@description  [Dot1xInterface] Specifies the physical interface to which the
* client is attached.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xInterface (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xInterfaceValue;

  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbDot1xPhysicalPortGet (L7_UNIT_CURRENT, lIntfNum,
                                &objDot1xInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xInterfaceValue,
                           sizeof (objDot1xInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientAuthPAEstate
*
* @purpose Get 'Dot1xClientAuthPAEstate'
 *@description  [Dot1xClientAuthPAEstate] The current value of the Authenticator
* PAE state machine for the client.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientAuthPAEstate (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xClientAuthPAEstateValue;

  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbDot1xLogicalPortPaeStateGet (lIntfNum,&objDot1xClientAuthPAEstateValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xClientAuthPAEstate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xClientAuthPAEstateValue,
                           sizeof (objDot1xClientAuthPAEstateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientBackendState
*
* @purpose Get 'Dot1xClientBackendState'
 *@description  [Dot1xClientBackendState] The current state of the Backend
* Authentication state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientBackendState (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xClientBackendStateValue;

  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    /* get the value from application */
    owa.l7rc = usmDbDot1xLogicalPortBackendAuthStateGet (lIntfNum,&objDot1xClientBackendStateValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xClientBackendState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xClientBackendStateValue,
                           sizeof (objDot1xClientBackendStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientUserName
*
* @purpose Get 'Dot1xClientUserName'
 *@description  [Dot1xClientUserName] Specifies the username with which the
* client is authenticated to the Radius server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientUserName (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDot1xClientUserNameValue;

  L7_DOT1X_APM_STATES_t paestate;
  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    memset(objDot1xClientUserNameValue, 0, sizeof(objDot1xClientUserNameValue));

    if ((usmDbDot1xLogicalPortPaeStateGet(lIntfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      /* get the value from application */
      owa.l7rc = usmdbDot1xLogicalPortUserNameGet(USMDB_UNIT_CURRENT,lIntfNum,objDot1xClientUserNameValue);
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xClientUserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1xClientUserNameValue,
                           strlen (objDot1xClientUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientSessionTime
*
* @purpose Get 'Dot1xClientSessionTime'
 *@description  [Dot1xClientSessionTime] Specifies the time elapsed in seconds
* since the client was authenticated in this session.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientSessionTime (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xClientSessionTimeValue;

  L7_DOT1X_APM_STATES_t paestate;
  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    if ((usmDbDot1xLogicalPortPaeStateGet(lIntfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
      objDot1xClientSessionTimeValue = 0;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      /* get the value from application */
      owa.l7rc = usmdbDot1xPortSessionTimeGet(USMDB_UNIT_CURRENT,lIntfNum,&objDot1xClientSessionTimeValue);
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xClientSessionTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xClientSessionTimeValue,
                           sizeof (objDot1xClientSessionTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientFilterID
*
* @purpose Get 'Dot1xClientFilterID'
 *@description  [Dot1xClientFilterID] Specifies the Filter ID or Diffserv Policy
* name to be applied to the session.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientFilterID (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDot1xClientFilterIDValue;

  L7_DOT1X_APM_STATES_t paestate;
  L7_DOT1X_PORT_CONTROL_t port_control_val;
  L7_uint32 intIfNum;
  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    memset(objDot1xClientFilterIDValue, 0, sizeof(objDot1xClientFilterIDValue));
    
    if ((usmDbDot1xLogicalPortPaeStateGet(lIntfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
      owa.l7rc = L7_SUCCESS;
    }

    /* Filter ID assignment only supported for Mac based*/
    else if ((usmDbDot1xPhysicalPortGet(L7_UNIT_CURRENT,lIntfNum,&intIfNum)==L7_SUCCESS) &&
        (usmDbDot1xPortControlModeGet(L7_UNIT_CURRENT,intIfNum,&port_control_val)==L7_SUCCESS) &&
        (port_control_val != L7_DOT1X_PORT_AUTO_MAC_BASED)) 
    {
      owa.l7rc = L7_SUCCESS;
    }
    else 
    {
      /* get the value from application */
      owa.l7rc = usmdbDot1xLogicalPortFilterIdGet(USMDB_UNIT_CURRENT,lIntfNum,objDot1xClientFilterIDValue);
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xClientFilterID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1xClientFilterIDValue,
                           strlen (objDot1xClientFilterIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientVlanAssigned
*
* @purpose Get 'Dot1xClientVlanAssigned'
 *@description  [Dot1xClientVlanAssigned] Specifies the vlan the client is
* associated with by Dot1x.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientVlanAssigned (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xClientVlanAssignedValue;
  xLibU32_t objDot1xClientVlanAssignedReason;

  L7_DOT1X_APM_STATES_t paestate;
  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    if ((usmDbDot1xLogicalPortPaeStateGet(lIntfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
      objDot1xClientVlanAssignedValue = 0;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      /* get the value from application */
      owa.l7rc = usmDbDot1xLogicalPortVlanAssignmentGet(lIntfNum,&objDot1xClientVlanAssignedValue,&objDot1xClientVlanAssignedReason);
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xClientVlanAssigned */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xClientVlanAssignedValue,
                           sizeof (objDot1xClientVlanAssignedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientVlanAssignedReason
*
* @purpose Get 'Dot1xClientVlanAssignedReason'
 *@description  [Dot1xClientVlanAssignedReason] Reason the client is associated
* to the vlan specified by agentDot1xClientVlanAssigned.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientVlanAssignedReason (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xClientVlanAssignedReasonValue;
  xLibU32_t objDot1xClientVlanAssignedValue;

  L7_DOT1X_APM_STATES_t paestate;
  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    if ((usmDbDot1xLogicalPortPaeStateGet(lIntfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
      objDot1xClientVlanAssignedReasonValue = 0;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      /* get the value from application */
      owa.l7rc = usmDbDot1xLogicalPortVlanAssignmentGet(lIntfNum,&objDot1xClientVlanAssignedValue,&objDot1xClientVlanAssignedReasonValue);
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xClientVlanAssignedReason */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xClientVlanAssignedReasonValue,
                           sizeof (objDot1xClientVlanAssignedReasonValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientSessionTimeout
*
* @purpose Get 'Dot1xClientSessionTimeout'
 *@description  [Dot1xClientSessionTimeout] Specifies the session time remaining
* for the client if assigned by the Radius server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientSessionTimeout (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xClientSessionTimeoutValue;

  L7_DOT1X_APM_STATES_t paestate;
  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);

  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    if ((usmDbDot1xLogicalPortPaeStateGet(lIntfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
      objDot1xClientSessionTimeoutValue = 0;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      /* get the value from application */
      owa.l7rc = usmDbDot1xLogicalPortSessionTimeoutGet(USMDB_UNIT_CURRENT,lIntfNum,&objDot1xClientSessionTimeoutValue);
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xClientSessionTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xClientSessionTimeoutValue,
                           sizeof (objDot1xClientSessionTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1x_Dot1xClientTerminationAction
*
* @purpose Get 'Dot1xClientTerminationAction'
 *@description  [Dot1xClientTerminationAction] Specifies the session termination
* action assigned by the Radius Server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1x_Dot1xClientTerminationAction (void *wap, void *bufp)
{

  fpObjWa_t kwaDot1xClientMacAddress = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t keyDot1xClientMacAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1xClientTerminationActionValue;

  L7_DOT1X_APM_STATES_t paestate;
  xLibU32_t lIntfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Dot1xClientMacAddress */
  kwaDot1xClientMacAddress.rc = xLibFilterGet (wap, XOBJ_securitydot1x_Dot1xClientMacAddress,
                                               (xLibU8_t *) keyDot1xClientMacAddressValue,
                                               &kwaDot1xClientMacAddress.len);
  if (kwaDot1xClientMacAddress.rc != XLIBRC_SUCCESS)
  {
    kwaDot1xClientMacAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaDot1xClientMacAddress);
    return kwaDot1xClientMacAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDot1xClientMacAddressValue, kwaDot1xClientMacAddress.len);


  owa.l7rc = usmDbDot1xClientMacAddressGet (L7_UNIT_CURRENT, keyDot1xClientMacAddressValue,
                              &lIntfNum);

  if (owa.l7rc == L7_SUCCESS)
  {
    if ((usmDbDot1xLogicalPortPaeStateGet(lIntfNum,&paestate)== L7_SUCCESS)&&
         (paestate != L7_DOT1X_APM_AUTHENTICATED))
    {
      objDot1xClientTerminationActionValue = L7_DOT1X_TERMINATION_ACTION_DEFAULT;
      owa.l7rc = L7_SUCCESS;
    }
    else
    {
      /* get the value from application */
      owa.l7rc = usmDbDot1xLogicalPortTerminationActionGet(USMDB_UNIT_CURRENT,lIntfNum,&objDot1xClientTerminationActionValue);
    }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1xClientTerminationAction */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1xClientTerminationActionValue,
                           sizeof (objDot1xClientTerminationActionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

