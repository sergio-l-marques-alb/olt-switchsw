/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basedot3Stats.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  14 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basedot3Stats_obj.h"
#include "usmdb_1643_stats_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsIndex
*
* @purpose Get 'dot3StatsIndex'
*
* @description  An index value that uniquely identifies an interface to an ethernet-like 
*              medium. The interface identified by a particular 
*              value of this index is the same interface as identified 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsIndexValue;
  xLibU32_t nextObjdot3StatsIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & objdot3StatsIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjdot3StatsIndexValue = 1;	
    owa.l7rc = usmDbVisibleIntIfNumberCheck (L7_UNIT_CURRENT,
                                     nextObjdot3StatsIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot3StatsIndexValue, owa.len);
    owa.l7rc =
      usmDbGetNextVisibleIntIfNumber (objdot3StatsIndexValue,
                                                  &nextObjdot3StatsIndexValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot3StatsIndexValue, owa.len);

  /* return the object value: dot3StatsIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot3StatsIndexValue,
                           sizeof (objdot3StatsIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsAlignmentErrors
*
* @purpose Get 'dot3StatsAlignmentErrors'
*
* @description  A count of frames received on a particular interface that are 
*              not an integral number of octets in length and do not pass the 
*              FCS check. The count represented by an instance of this 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsAlignmentErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsAlignmentErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsAlignmentErrorsGet (L7_UNIT_CURRENT, keydot3StatsIndexValue,
                                      &objdot3StatsAlignmentErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsAlignmentErrors */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsAlignmentErrorsValue,
                    sizeof (objdot3StatsAlignmentErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsFCSErrors
*
* @purpose Get 'dot3StatsFCSErrors'
*
* @description  A count of frames received on a particular interface that are 
*              an integral number of octets in length but do not pass the FCS 
*              check. This count does not include frames received with 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsFCSErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsFCSErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsFCSErrorsGet (L7_UNIT_CURRENT, keydot3StatsIndexValue,
                                &objdot3StatsFCSErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsFCSErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsFCSErrorsValue,
                           sizeof (objdot3StatsFCSErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsSingleCollisionFrames
*
* @purpose Get 'dot3StatsSingleCollisionFrames'
*
* @description  A count of frames that are involved in a single collision, and 
*              are subsequently transmitted successfully. A frame that is 
*              counted by an instance of this object is also count 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsSingleCollisionFrames (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsSingleCollisionFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsSingleCollisionFramesGet (L7_UNIT_CURRENT,
                                            keydot3StatsIndexValue,
                                            &objdot3StatsSingleCollisionFramesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsSingleCollisionFrames */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsSingleCollisionFramesValue,
                    sizeof (objdot3StatsSingleCollisionFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsMultipleCollisionFrames
*
* @purpose Get 'dot3StatsMultipleCollisionFrames'
*
* @description  A count of frames that are involved in more than one collision 
*              and are subsequently transmitted successfully. A frame that 
*              is counted by an instance of this object is also 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsMultipleCollisionFrames (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsMultipleCollisionFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsMultipleCollisionFramesGet (L7_UNIT_CURRENT,
                                              keydot3StatsIndexValue,
                                              &objdot3StatsMultipleCollisionFramesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsMultipleCollisionFrames */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objdot3StatsMultipleCollisionFramesValue,
                    sizeof (objdot3StatsMultipleCollisionFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsSQETestErrors
*
* @purpose Get 'dot3StatsSQETestErrors'
*
* @description A count of times that the SQE TEST ERROR is received on a particular 
*              interface. The SQE TEST ERROR is set in accordance with 
*              the rules for verification of the SQE detection mechan 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsSQETestErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsSQETestErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsSQETestErrorsGet (L7_UNIT_CURRENT, keydot3StatsIndexValue,
                                    &objdot3StatsSQETestErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsSQETestErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsSQETestErrorsValue,
                           sizeof (objdot3StatsSQETestErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsDeferredTransmissions
*
* @purpose Get 'dot3StatsDeferredTransmissions'
*
* @description A count of frames for which the first transmission attempt on 
*              a particular interface is delayed because the medium is busy. 
*              The count represented by an instance of this object doe 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsDeferredTransmissions (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsDeferredTransmissionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsDeferredTransmissionsGet (L7_UNIT_CURRENT,
                                            keydot3StatsIndexValue,
                                            &objdot3StatsDeferredTransmissionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsDeferredTransmissions */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsDeferredTransmissionsValue,
                    sizeof (objdot3StatsDeferredTransmissionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsLateCollisions
*
* @purpose Get 'dot3StatsLateCollisions'
*
* @description  The number of times that a collision is detected on a particular 
*              interface later than one slotTime into the transmission of 
*              a packet. A (late) collision included in a count 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsLateCollisions (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsLateCollisionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsLateCollisionsGet (L7_UNIT_CURRENT, keydot3StatsIndexValue,
                                     &objdot3StatsLateCollisionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsLateCollisions */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsLateCollisionsValue,
                           sizeof (objdot3StatsLateCollisionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsExcessiveCollisions
*
* @purpose Get 'dot3StatsExcessiveCollisions'
*
* @description  A count of frames for which transmission on a particular interface 
*              fails due to excessive collisions. This counter does not 
*              increment when the interface is operating in full 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsExcessiveCollisions (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsExcessiveCollisionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsExcessiveCollisionsGet (L7_UNIT_CURRENT,
                                          keydot3StatsIndexValue,
                                          &objdot3StatsExcessiveCollisionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsExcessiveCollisions */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsExcessiveCollisionsValue,
                    sizeof (objdot3StatsExcessiveCollisionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsInternalMacTransmitErrors
*
* @purpose Get 'dot3StatsInternalMacTransmitErrors'
*
* @description A count of frames for which transmission on a particular interface 
*              fails due to an internal MAC sublayer transmit error. A 
*              frame is only counted by an instance of this object if it is 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsInternalMacTransmitErrors (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsInternalMacTransmitErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsInternalMacTransmitErrorsGet (L7_UNIT_CURRENT,
                                                keydot3StatsIndexValue,
                                                &objdot3StatsInternalMacTransmitErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsInternalMacTransmitErrors */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objdot3StatsInternalMacTransmitErrorsValue,
                    sizeof (objdot3StatsInternalMacTransmitErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsCarrierSenseErrors
*
* @purpose Get 'dot3StatsCarrierSenseErrors'
*
* @description The number of times that the carrier sense condition was lost 
*              or never asserted when attempting to transmit a frame on a particular 
*              interface. The count represented by an instance 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsCarrierSenseErrors (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsCarrierSenseErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsCarrierSenseErrorsGet (L7_UNIT_CURRENT,
                                         keydot3StatsIndexValue,
                                         &objdot3StatsCarrierSenseErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsCarrierSenseErrors */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsCarrierSenseErrorsValue,
                    sizeof (objdot3StatsCarrierSenseErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsFrameTooLongs
*
* @purpose Get 'dot3StatsFrameTooLongs'
*
* @description  A count of frames received on a particular interface that exceed 
*              the maximum permitted frame size. The count represented by 
*              an instance of this object is incremented when th 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsFrameTooLongs (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsFrameTooLongsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsFrameTooLongsGet (L7_UNIT_CURRENT, keydot3StatsIndexValue,
                                    &objdot3StatsFrameTooLongsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsFrameTooLongs */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsFrameTooLongsValue,
                           sizeof (objdot3StatsFrameTooLongsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsInternalMacReceiveErrors
*
* @purpose Get 'dot3StatsInternalMacReceiveErrors'
*
* @description A count of frames for which reception on a particular interface 
*              fails due to an internal MAC sublayer receive error. A frame 
*              is only counted by an instance of this object if it is 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsInternalMacReceiveErrors (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsInternalMacReceiveErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsInternalMacReceiveErrorsGet (L7_UNIT_CURRENT,
                                               keydot3StatsIndexValue,
                                               &objdot3StatsInternalMacReceiveErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsInternalMacReceiveErrors */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objdot3StatsInternalMacReceiveErrorsValue,
                    sizeof (objdot3StatsInternalMacReceiveErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basedot3Stats_dot3StatsEtherChipSet
*
* @purpose Get 'dot3StatsEtherChipSet'
*
* @description the chipset on which this interface is built 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basedot3Stats_dot3StatsEtherChipSet (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot3StatsIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot3StatsEtherChipSetValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot3StatsIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_basedot3Stats_dot3StatsIndex,
                          (xLibU8_t *) & keydot3StatsIndexValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot3StatsIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot3StatsEtherChipSetGet (L7_UNIT_CURRENT, keydot3StatsIndexValue,
                                   &objdot3StatsEtherChipSetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot3StatsEtherChipSet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot3StatsEtherChipSetValue,
                           sizeof (objdot3StatsEtherChipSetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
