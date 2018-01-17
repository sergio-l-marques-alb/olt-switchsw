
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_securityDot1xSupplicantIntfStats.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to security-object.xml
*
* @create  28 June 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_securityDot1xSupplicantIntfStats_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_dot1x_api.h"

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] A table that contains the statistics objects for the
* Supplicant PAE associated with each port. An entry appears in
* this table for each port that may authenticate itself when
* challenged by a remote system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_Interface (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbNextPhysicalExtIfNumberGet(L7_UNIT_CURRENT, 0, &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbNextPhysicalExtIfNumberGet(L7_UNIT_CURRENT, objInterfaceValue, &nextObjInterfaceValue);
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

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_FramesRx
*
* @purpose Get 'FramesRx'
 *@description  [FramesRx] The number of EAPOL frames of any type that have been
* received by this Supplicant.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_FramesRx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortEapolFramesRxGet(keyInterfaceValue, &objFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFramesRxValue, sizeof (objFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_FramesTx
*
* @purpose Get 'FramesTx'
 *@description  [FramesTx] The number of EAPOL frames of any type that have been
* transmitted by this Supplicant.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_FramesTx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortEapolFramesTxGet(keyInterfaceValue, &objFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFramesTxValue, sizeof (objFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_StartFramesTx
*
* @purpose Get 'StartFramesTx'
 *@description  [StartFramesTx] The number of EAPOL Start frames that have been
* transmitted by this Supplicant.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_StartFramesTx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortEapolStartFramesTxGet(keyInterfaceValue, &objStartFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StartFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartFramesTxValue,
                           sizeof (objStartFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_LogoffFramesTx
*
* @purpose Get 'LogoffFramesTx'
 *@description  [LogoffFramesTx] The number of EAPOL Logoff frames that have
* been transmitted by this Supplicant.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_LogoffFramesTx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLogoffFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortEapolLogoffFramesTxGet(keyInterfaceValue, &objLogoffFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LogoffFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLogoffFramesTxValue,
                           sizeof (objLogoffFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_RespIdFramesTx
*
* @purpose Get 'RespIdFramesTx'
 *@description  [RespIdFramesTx] The number of EAP Resp/Id frames that have been
* transmitted by this Supplicant.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_RespIdFramesTx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRespIdFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortEapolRespIdFramesTxGet(keyInterfaceValue, &objRespIdFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RespIdFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRespIdFramesTxValue,
                           sizeof (objRespIdFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_ResponseFramesTx
*
* @purpose Get 'ResponseFramesTx'
 *@description  [ResponseFramesTx] The number of valid EAP Response frames
* (other than Resp/Id frames) that have been transmitted by this
* Supplicant.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_ResponseFramesTx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objResponseFramesTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortEapolResponseFramesTxGet(keyInterfaceValue, &objResponseFramesTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ResponseFramesTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objResponseFramesTxValue,
                           sizeof (objResponseFramesTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_ReqIdFramesRx
*
* @purpose Get 'ReqIdFramesRx'
 *@description  [ReqIdFramesRx] The number of EAP Req/Id frames that have been
* received by this Supplicant.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_ReqIdFramesRx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objReqIdFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortEapolReqIdFramesRxGet(keyInterfaceValue, &objReqIdFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ReqIdFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objReqIdFramesRxValue,
                           sizeof (objReqIdFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_ReqFramesRx
*
* @purpose Get 'ReqFramesRx'
 *@description  [ReqFramesRx] The number of EAP Request frames (other than Rq/Id
* frames) that have been received by this Supplicant.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_ReqFramesRx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objReqFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortEapolRequestFramesRxGet(keyInterfaceValue, &objReqFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ReqFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objReqFramesRxValue, sizeof (objReqFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_InvalidEapolFramesRx
*
* @purpose Get 'InvalidEapolFramesRx'
 *@description  [InvalidEapolFramesRx] The number of EAPOL frames that have been
* received by this Supplicant in which the frame type is not
* recognized.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_InvalidEapolFramesRx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInvalidEapolFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortInvalidEapolFramesRxGet(keyInterfaceValue, &objInvalidEapolFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: InvalidEapolFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objInvalidEapolFramesRxValue,
                           sizeof (objInvalidEapolFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_EapLengthErrorFramesRx
*
* @purpose Get 'EapLengthErrorFramesRx'
 *@description  [EapLengthErrorFramesRx] The number of EAPOL frames that have
* been received by this Supplicant in which the Packet Body Length
* field is invalid.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_EapLengthErrorFramesRx (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objEapLengthErrorFramesRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortEapLengthErrorFramesRxGet(keyInterfaceValue, &objEapLengthErrorFramesRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: EapLengthErrorFramesRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEapLengthErrorFramesRxValue,
                           sizeof (objEapLengthErrorFramesRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_LastEapolFrameVersion
*
* @purpose Get 'LastEapolFrameVersion'
 *@description  [LastEapolFrameVersion] The protocol version number carried in
* the most recently received EAPOL frame.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_LastEapolFrameVersion (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objLastEapolFrameVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortLastEapolFrameVersionGet(keyInterfaceValue, &objLastEapolFrameVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LastEapolFrameVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLastEapolFrameVersionValue,
                           sizeof (objLastEapolFrameVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfStats_LastEapolFrameSource
*
* @purpose Get 'LastEapolFrameSource'
 *@description  [LastEapolFrameSource] The source MAC address carried in the
* most recently received EAPOL frame.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfStats_LastEapolFrameSource (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr6_t objLastEapolFrameSourceValue;
  L7_enetMacAddr_t lastEapolFrameSource;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortLastEapolFrameSourceGet(keyInterfaceValue, &lastEapolFrameSource);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  memcpy(objLastEapolFrameSourceValue, &lastEapolFrameSource, sizeof(lastEapolFrameSource));
  /* return the object value: LastEapolFrameSource */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLastEapolFrameSourceValue,
                           sizeof (objLastEapolFrameSourceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
