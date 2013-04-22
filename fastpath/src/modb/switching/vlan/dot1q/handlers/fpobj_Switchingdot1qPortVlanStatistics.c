/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot1qPortVlanStatistics.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dot1q-object.xml
*
* @create  9 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_Switchingdot1qPortVlanStatistics_obj.h"
#include "usmdb_mib_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort
*
* @purpose Get 'PortVlanStatsBasePort'
*
* @description [PortVlanStatsBasePort] The port number of the port for which this entry contains bridge management information
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort (void *wap, void *bufp)
{

  xLibU32_t objPortVlanStatsBasePortValue;
  xLibU32_t nextObjPortVlanStatsBasePortValue;
  xLibU32_t objPortVlanStatsVlanIndexValue;
  xLibU32_t nextObjPortVlanStatsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanStatsBasePort */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort,
                          (xLibU8_t *) & objPortVlanStatsBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPortVlanStatsBasePortValue = objPortVlanStatsVlanIndexValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortVlanStatsBasePortValue, owa.len);
    objPortVlanStatsVlanIndexValue = 0;
    do
    {
      owa.l7rc = usmDbDot1qPortVlanStatisticsEntryNextGet(L7_UNIT_CURRENT,
                                                          &objPortVlanStatsBasePortValue,
                                                          &objPortVlanStatsVlanIndexValue);
     nextObjPortVlanStatsBasePortValue = objPortVlanStatsBasePortValue;
     nextObjPortVlanStatsVlanIndexValue = objPortVlanStatsVlanIndexValue;
    }
    while ((objPortVlanStatsBasePortValue == nextObjPortVlanStatsBasePortValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortVlanStatsBasePortValue, owa.len);

  /* return the object value: PortVlanStatsBasePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortVlanStatsBasePortValue,
                           sizeof (objPortVlanStatsBasePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanStatistics_PortVlanStatsVlanIndex
*
* @purpose Get 'PortVlanStatsVlanIndex'
*
* @description [PortVlanStatsVlanIndex] The VLAN-ID or other identifier refering to this VLAN
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qPortVlanStatistics_PortVlanStatsVlanIndex (void *wap, void *bufp)
{

  xLibU32_t objPortVlanStatsBasePortValue;
  xLibU32_t nextObjPortVlanStatsBasePortValue;
  xLibU32_t objPortVlanStatsVlanIndexValue;
  xLibU32_t nextObjPortVlanStatsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanStatsBasePort */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort,
                          (xLibU8_t *) & objPortVlanStatsBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortVlanStatsBasePortValue, owa.len);

  /* retrieve key: PortVlanStatsVlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsVlanIndex,
                          (xLibU8_t *) & objPortVlanStatsVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPortVlanStatsVlanIndexValue = 0;
    nextObjPortVlanStatsBasePortValue = 0;
    nextObjPortVlanStatsVlanIndexValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortVlanStatsVlanIndexValue, owa.len);

    nextObjPortVlanStatsBasePortValue = objPortVlanStatsBasePortValue;
    nextObjPortVlanStatsVlanIndexValue = objPortVlanStatsVlanIndexValue;
    owa.l7rc = usmDbDot1qPortVlanStatisticsEntryNextGet(L7_UNIT_CURRENT,
                                                        &nextObjPortVlanStatsBasePortValue,
                                                        &nextObjPortVlanStatsVlanIndexValue);
  }

  if ((objPortVlanStatsBasePortValue != nextObjPortVlanStatsBasePortValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortVlanStatsVlanIndexValue, owa.len);

  /* return the object value: PortVlanStatsVlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortVlanStatsVlanIndexValue,
                           sizeof (objPortVlanStatsVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortInDiscards
*
* @purpose Get 'TpVlanPortInDiscards'
*
* @description [TpVlanPortInDiscards]: The number of valid frames received
*              by this port from its segment which were classified as belonging
*              to this VLAN which were discarded due to VLAN related
*              reasons. Specifically, the IEEE 802.1Q counters for Discard
*              Inbound and Discard on Ingress Filtering. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortInDiscards (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsBasePortValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTpVlanPortInDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanStatsBasePort */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort,
                   (xLibU8_t *) & keyPortVlanStatsBasePortValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsBasePortValue, kwa1.len);

  /* retrieve key: PortVlanStatsVlanIndex */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsVlanIndex,
                   (xLibU8_t *) & keyPortVlanStatsVlanIndexValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpVlanPortInDiscardsGet (L7_UNIT_CURRENT,
                                       keyPortVlanStatsBasePortValue,
                                       keyPortVlanStatsVlanIndexValue,
                                       &objTpVlanPortInDiscardsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TpVlanPortInDiscards */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTpVlanPortInDiscardsValue,
                           sizeof (objTpVlanPortInDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortInFrames
*
* @purpose Get 'TpVlanPortInFrames'
*
* @description [TpVlanPortInFrames]: The number of valid frames received by
*              this port from its segment which were classified as belonging
*              to this VLAN. Note that a frame received on this port
*              is counted by this object if and only if it is for a protocol
*              being processed by the local forwarding process for this
*              VLAN. This object includes received bridge management frames
*              classified as belonging to this VLAN(e.g. GMRP, but not
*              GVRP or STP). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortInFrames (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsBasePortValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTpVlanPortInFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanStatsBasePort */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort,
                   (xLibU8_t *) & keyPortVlanStatsBasePortValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsBasePortValue, kwa1.len);

  /* retrieve key: PortVlanStatsVlanIndex */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsVlanIndex,
                   (xLibU8_t *) & keyPortVlanStatsVlanIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpVlanPortInFramesGet (L7_UNIT_CURRENT,
                                     keyPortVlanStatsBasePortValue,
                                     keyPortVlanStatsVlanIndexValue,
                                     &objTpVlanPortInFramesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TpVlanPortInFrames */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTpVlanPortInFramesValue,
                           sizeof (objTpVlanPortInFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortInOverflowDiscards
*
* @purpose Get 'TpVlanPortInOverflowDiscards'
*
* @description [TpVlanPortInOverflowDiscards]: The number of times the associated
*              dot1qTpVlanPortInDiscards counter has overflowed. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortInOverflowDiscards (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsBasePortValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTpVlanPortInOverflowDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanStatsBasePort */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort,
                   (xLibU8_t *) & keyPortVlanStatsBasePortValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsBasePortValue, kwa1.len);

  /* retrieve key: PortVlanStatsVlanIndex */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsVlanIndex,
                   (xLibU8_t *) & keyPortVlanStatsVlanIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpVlanPortInOverflowDiscardsGet (L7_UNIT_CURRENT,
                                               keyPortVlanStatsBasePortValue,
                                               keyPortVlanStatsVlanIndexValue,
                                               &objTpVlanPortInOverflowDiscardsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TpVlanPortInOverflowDiscards */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objTpVlanPortInOverflowDiscardsValue,
                    sizeof (objTpVlanPortInOverflowDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortInOverflowFrames
*
* @purpose Get 'TpVlanPortInOverflowFrames'
*
* @description [TpVlanPortInOverflowFrames]: The number of times the associated
*              dot1qTpVlanPortOutFrames counter has overflowed 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortInOverflowFrames (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsBasePortValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTpVlanPortInOverflowFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanStatsBasePort */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort,
                   (xLibU8_t *) & keyPortVlanStatsBasePortValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsBasePortValue, kwa1.len);

  /* retrieve key: PortVlanStatsVlanIndex */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsVlanIndex,
                   (xLibU8_t *) & keyPortVlanStatsVlanIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpVlanPortOutOverflowFramesGet (L7_UNIT_CURRENT,
                                              keyPortVlanStatsBasePortValue,
                                              keyPortVlanStatsVlanIndexValue,
                                              &objTpVlanPortInOverflowFramesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TpVlanPortInOverflowFrames */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objTpVlanPortInOverflowFramesValue,
                    sizeof (objTpVlanPortInOverflowFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortOutFrames
*
* @purpose Get 'TpVlanPortOutFrames'
*
* @description [TpVlanPortOutFrames]: The number of valid frames transmitted
*              by this port to its segment from the local forwarding process
*              for this VLAN. This includes bridge management frames
*              originated by this device which are classified as belonging
*              to this VLAN (e.g. GMRP, but not GVRP or STP). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortOutFrames (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsBasePortValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTpVlanPortOutFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanStatsBasePort */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort,
                   (xLibU8_t *) & keyPortVlanStatsBasePortValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsBasePortValue, kwa1.len);

  /* retrieve key: PortVlanStatsVlanIndex */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsVlanIndex,
                   (xLibU8_t *) & keyPortVlanStatsVlanIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpVlanPortOutFramesGet (L7_UNIT_CURRENT,
                                      keyPortVlanStatsBasePortValue,
                                      keyPortVlanStatsVlanIndexValue,
                                      &objTpVlanPortOutFramesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TpVlanPortOutFrames */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTpVlanPortOutFramesValue,
                           sizeof (objTpVlanPortOutFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortOutOverflowFrames
*
* @purpose Get 'TpVlanPortOutOverflowFrames'
*
* @description [TpVlanPortOutOverflowFrames]: The number of times the associated
*              dot1qTpVlanPortOutFrames counter has overflowed. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_Switchingdot1qPortVlanStatistics_TpVlanPortOutOverflowFrames (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsBasePortValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanStatsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTpVlanPortOutOverflowFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanStatsBasePort */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsBasePort,
                   (xLibU8_t *) & keyPortVlanStatsBasePortValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsBasePortValue, kwa1.len);

  /* retrieve key: PortVlanStatsVlanIndex */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanStatistics_PortVlanStatsVlanIndex,
                   (xLibU8_t *) & keyPortVlanStatsVlanIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanStatsVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpVlanPortOutOverflowFramesGet (L7_UNIT_CURRENT,
                                              keyPortVlanStatsBasePortValue,
                                              keyPortVlanStatsVlanIndexValue,
                                              &objTpVlanPortOutOverflowFramesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TpVlanPortOutOverflowFrames */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objTpVlanPortOutOverflowFramesValue,
                    sizeof (objTpVlanPortOutOverflowFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
