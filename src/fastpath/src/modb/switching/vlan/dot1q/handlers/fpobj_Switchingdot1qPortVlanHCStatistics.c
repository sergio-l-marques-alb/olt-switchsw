/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot1qPortVlanHCStatistics.c
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
#include "_xe_Switchingdot1qPortVlanHCStatistics_obj.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"


L7_char8 *strUtil64toasc (L7_uint32 high, L7_uint32 low, L7_char8 * inbuf, L7_uint32 inbufsize)
{
  L7_ulong64 num;
  static L7_char8 valbuf[32];
  
  char *tmpBuf;
  L7_uint32 size;
  num.high = high;
  num.low = low;
  if(inbuf != L7_NULLPTR)
  {
    tmpBuf = inbuf;
    size = inbufsize;
  }
  else
  {
    tmpBuf = valbuf;
    size = sizeof (valbuf);
  }
  memset (tmpBuf, 0x00, size);
  if (usmDb64BitsToString (num, tmpBuf) != L7_SUCCESS)
  {
    osapiStrncpySafe (tmpBuf, "ERROR", size);
  }
  return tmpBuf;
}
 
/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsBasePort
*
* @purpose Get 'PortVlanHCStatisticsBasePort'
*
* @description [PortVlanHCStatisticsBasePort] The port number of the port for which this entry contains bridge management information
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsBasePort (void *wap,
                                                                                   void *bufp)
{

  xLibU32_t objPortVlanHCStatisticsBasePortValue;
  xLibU32_t nextObjPortVlanHCStatisticsBasePortValue;
  xLibU32_t objPortVlanHCStatisticsVlanIndexValue;
  xLibU32_t nextObjPortVlanHCStatisticsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanHCStatisticsBasePort */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsBasePort,
                          (xLibU8_t *) & objPortVlanHCStatisticsBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPortVlanHCStatisticsBasePortValue = objPortVlanHCStatisticsVlanIndexValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortVlanHCStatisticsBasePortValue, owa.len);
    objPortVlanHCStatisticsVlanIndexValue = 0;
    do
    {
      owa.l7rc = usmDbDot1qPortVlanHCStatisticsEntryNextGet (L7_UNIT_CURRENT,
                                                             &objPortVlanHCStatisticsBasePortValue,
                                                             &objPortVlanHCStatisticsVlanIndexValue);
     nextObjPortVlanHCStatisticsBasePortValue = objPortVlanHCStatisticsBasePortValue;
     nextObjPortVlanHCStatisticsVlanIndexValue = objPortVlanHCStatisticsVlanIndexValue;
    }
    while ((objPortVlanHCStatisticsBasePortValue == nextObjPortVlanHCStatisticsBasePortValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortVlanHCStatisticsBasePortValue, owa.len);

  /* return the object value: PortVlanHCStatisticsBasePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortVlanHCStatisticsBasePortValue,
                           sizeof (objPortVlanHCStatisticsBasePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsVlanIndex
*
* @purpose Get 'PortVlanHCStatisticsVlanIndex'
*
* @description [PortVlanHCStatisticsVlanIndex] The VLAN-ID or other identifier refering to this VLAN
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsVlanIndex (void *wap,
                                                                                    void *bufp)
{
  xLibU32_t objPortVlanHCStatisticsBasePortValue;
  xLibU32_t nextObjPortVlanHCStatisticsBasePortValue;
  xLibU32_t objPortVlanHCStatisticsVlanIndexValue;
  xLibU32_t nextObjPortVlanHCStatisticsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanHCStatisticsBasePort */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsBasePort,
                          (xLibU8_t *) & objPortVlanHCStatisticsBasePortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortVlanHCStatisticsBasePortValue, owa.len);

  /* retrieve key: PortVlanHCStatisticsVlanIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsVlanIndex,
                   (xLibU8_t *) & objPortVlanHCStatisticsVlanIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objPortVlanHCStatisticsVlanIndexValue = 0;
    nextObjPortVlanHCStatisticsBasePortValue = objPortVlanHCStatisticsBasePortValue;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortVlanHCStatisticsVlanIndexValue, owa.len);

    nextObjPortVlanHCStatisticsBasePortValue = objPortVlanHCStatisticsBasePortValue;
    nextObjPortVlanHCStatisticsVlanIndexValue = objPortVlanHCStatisticsVlanIndexValue;
    owa.l7rc = usmDbDot1qPortVlanHCStatisticsEntryNextGet(L7_UNIT_CURRENT,
                                                          &nextObjPortVlanHCStatisticsBasePortValue,
                                                          &nextObjPortVlanHCStatisticsVlanIndexValue);
  }

  if ((objPortVlanHCStatisticsBasePortValue != nextObjPortVlanHCStatisticsBasePortValue)
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortVlanHCStatisticsVlanIndexValue, owa.len);

  /* return the object value: PortVlanHCStatisticsVlanIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortVlanHCStatisticsVlanIndexValue,
                           sizeof (objPortVlanHCStatisticsVlanIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanHCStatistics_TpVlanPortHCInDiscards
*
* @purpose Get 'TpVlanPortHCInDiscards'
*
* @description [TpVlanPortHCInDiscards]: The number of valid frames received
*              by this port from its segment which were classified as belonging
*              to this VLAN which were discarded due to VLAN related
*              reasons. Specifically, the IEEE 802.1Q counters for Discard
*              Inbound and Discard on Ingress Filtering. 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_Switchingdot1qPortVlanHCStatistics_TpVlanPortHCInDiscards (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanHCStatisticsBasePortValue,big_end,little_end;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanHCStatisticsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objTpVlanPortHCInDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanHCStatisticsBasePort */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsBasePort,
                   (xLibU8_t *) & keyPortVlanHCStatisticsBasePortValue,
                   &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortVlanHCStatisticsBasePortValue, kwa1.len);

  /* retrieve key: PortVlanHCStatisticsVlanIndex */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsVlanIndex,
                   (xLibU8_t *) & keyPortVlanHCStatisticsVlanIndexValue,
                   &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp,
                           &keyPortVlanHCStatisticsVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpVlanPortHCInDiscardsGet (L7_UNIT_CURRENT,
                                         keyPortVlanHCStatisticsBasePortValue,
                                         keyPortVlanHCStatisticsVlanIndexValue,
                                         &big_end, &little_end);

  strUtil64toasc(big_end,little_end,objTpVlanPortHCInDiscardsValue, sizeof(objTpVlanPortHCInDiscardsValue)); 

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TpVlanPortHCInDiscards */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTpVlanPortHCInDiscardsValue,
                           sizeof (objTpVlanPortHCInDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanHCStatistics_TpVlanPortHCInFrames
*
* @purpose Get 'TpVlanPortHCInFrames'
*
* @description [TpVlanPortHCInFrames]: The number of valid frames received
*              by this port from its segment which were classified as belonging
*              to this VLAN. Note that a frame received on this port
*              is counted by this object if and only if it is for a protocol
*              being processed by the local forwarding process for this
*              VLAN. This object includes received bridge management frames
*              classified as belonging to this VLAN (e.g. GMRP, but not
*              GVRP or STP) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qPortVlanHCStatistics_TpVlanPortHCInFrames (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanHCStatisticsBasePortValue, big_end, little_end;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanHCStatisticsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objTpVlanPortHCInFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanHCStatisticsBasePort */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsBasePort,
                   (xLibU8_t *) & keyPortVlanHCStatisticsBasePortValue,
                   &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp,
                           &keyPortVlanHCStatisticsBasePortValue, kwa1.len);

  /* retrieve key: PortVlanHCStatisticsVlanIndex */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsVlanIndex,
                   (xLibU8_t *) & keyPortVlanHCStatisticsVlanIndexValue,
                   &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp,
                           &keyPortVlanHCStatisticsVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpVlanPortHCInFramesGet (L7_UNIT_CURRENT,
                                       keyPortVlanHCStatisticsBasePortValue,
                                       keyPortVlanHCStatisticsVlanIndexValue,
                                       &big_end,
                                       &little_end);

  strUtil64toasc(big_end,little_end,objTpVlanPortHCInFramesValue, sizeof(objTpVlanPortHCInFramesValue)); 

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TpVlanPortHCInFrames */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTpVlanPortHCInFramesValue,
                           sizeof (objTpVlanPortHCInFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1qPortVlanHCStatistics_TpVlanPortHCOutFrames
*
* @purpose Get 'TpVlanPortHCOutFrames'
*
* @description [TpVlanPortHCOutFrames]: The number of valid frames transmitted
*              by this port to its segment from the local forwarding
*              process for this VLAN. This includes bridge management frames
*              originated by this device which are classified as belonging
*              to this VLAN (e.g. GMRP, but not GVRP or STP) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1qPortVlanHCStatistics_TpVlanPortHCOutFrames (void
                                                                            *wap,
                                                                            void
                                                                            *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanHCStatisticsBasePortValue, big_end,little_end;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortVlanHCStatisticsVlanIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objTpVlanPortHCOutFramesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortVlanHCStatisticsBasePort */
  kwa1.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsBasePort,
                   (xLibU8_t *) & keyPortVlanHCStatisticsBasePortValue,
                   &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp,
                           &keyPortVlanHCStatisticsBasePortValue, kwa1.len);

  /* retrieve key: PortVlanHCStatisticsVlanIndex */
  kwa2.rc =
    xLibFilterGet (wap,
                   XOBJ_Switchingdot1qPortVlanHCStatistics_PortVlanHCStatisticsVlanIndex,
                   (xLibU8_t *) & keyPortVlanHCStatisticsVlanIndexValue,
                   &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp,
                           &keyPortVlanHCStatisticsVlanIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1qTpVlanPortHCOutFramesGet (L7_UNIT_CURRENT,
                                        keyPortVlanHCStatisticsBasePortValue,
                                        keyPortVlanHCStatisticsVlanIndexValue,
                                        &big_end,
                                        &little_end);
  strUtil64toasc(big_end,little_end,objTpVlanPortHCOutFramesValue, sizeof(objTpVlanPortHCOutFramesValue)); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TpVlanPortHCOutFrames */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTpVlanPortHCOutFramesValue,
                           sizeof (objTpVlanPortHCOutFramesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
