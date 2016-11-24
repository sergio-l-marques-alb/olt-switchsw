/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfVirtNbr.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfVirtNbr-object.xml
*
* @create  2 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfVirtNbr_obj.h"
#include "usmdb_mib_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfVirtNbr_Area
*
* @purpose Get 'Area'
 *@description  [Area] The Transit Area Identifier.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtNbr_Area (void *wap, void *bufp)
{

  xLibIpV4_t objAreaValue;
  xLibIpV4_t nextObjAreaValue;
  xLibIpV4_t objRtrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Area */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_Area, (xLibU8_t *) & objAreaValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objAreaValue, 0, sizeof (objAreaValue));
    memset (&objRtrIdValue, 0, sizeof (objRtrIdValue));
    owa.l7rc = usmDbOspfVirtNbrEntryNext(L7_UNIT_CURRENT, &objAreaValue, &objRtrIdValue); 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaValue, owa.len);
    nextObjAreaValue = objAreaValue;
    memset (&objRtrIdValue, 0, sizeof (objRtrIdValue));
    do
    {
      owa.l7rc = usmDbOspfVirtNbrEntryNext(L7_UNIT_CURRENT, &objAreaValue, &objRtrIdValue); 
    }
    while ((objAreaValue == nextObjAreaValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjAreaValue = objAreaValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAreaValue, owa.len);

  /* return the object value: Area */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAreaValue, sizeof (objAreaValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfVirtNbr_RtrId
*
* @purpose Get 'RtrId'
 *@description  [RtrId] A 32-bit integer uniquely identifying the neighboring
* router in the Autonomous System.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtNbr_RtrId (void *wap, void *bufp)
{

  xLibIpV4_t objAreaValue;
  xLibIpV4_t nextObjAreaValue;
  xLibIpV4_t objRtrIdValue;
  xLibIpV4_t nextObjRtrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Area */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_Area, (xLibU8_t *) & objAreaValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaValue, owa.len);

  /* retrieve key: RtrId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_RtrId,
                          (xLibU8_t *) & objRtrIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objRtrIdValue, 0, sizeof (objRtrIdValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRtrIdValue, owa.len);

  }

  nextObjAreaValue = objAreaValue;
  owa.l7rc = usmDbOspfVirtNbrEntryNext(L7_UNIT_CURRENT, &objAreaValue, &objRtrIdValue); 

  if ((objAreaValue != nextObjAreaValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjRtrIdValue = objRtrIdValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRtrIdValue, sizeof (objRtrIdValue));

  /* return the object value: RtrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRtrIdValue, sizeof (objRtrIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#if 0
/*******************************************************************************
* @function fpObjGet_routingospfVirtNbr_Area_RtrId
*
* @purpose Get 'Area + RtrId +'
*
* @description [Area]: The Transit Area Identifier. 
*              [RtrId]: A 32-bit integer uniquely identifying the neighboring
*              router in the Autonomous System. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtNbr_Area_RtrId (void *wap, void *bufp[],
                                                 xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaArea = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaValue, nextObjAreaValue;
  fpObjWa_t owaRtrId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRtrIdValue, nextObjRtrIdValue;
  void *outArea = (void *) bufp[--keyCount];
  void *outRtrId = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outArea);
  FPOBJ_TRACE_ENTER (outRtrId);

  /* retrieve key: Area */
  owaArea.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_Area,
                              (xLibU8_t *) & objAreaValue, &owaArea.len);
  if (owaArea.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: RtrId */
    owaRtrId.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_RtrId,
                                 (xLibU8_t *) & objRtrIdValue, &owaRtrId.len);
  }
  else
  {
    objAreaValue = 0;
    nextObjAreaValue = 0;
    objRtrIdValue = 0;
    nextObjRtrIdValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (outArea, &objAreaValue, owaArea.len);
  FPOBJ_TRACE_CURRENT_KEY (outRtrId, &objRtrIdValue, owaRtrId.len);

  owa.rc = usmDbOspfVirtNbrEntryNext(L7_UNIT_CURRENT, &objAreaValue, &objRtrIdValue);

  nextObjAreaValue =  objAreaValue;
  nextObjRtrIdValue = objRtrIdValue;

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outArea, owaArea);
    FPOBJ_TRACE_EXIT (outRtrId, owaRtrId);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (outArea, &nextObjAreaValue, owaArea.len);
  FPOBJ_TRACE_CURRENT_KEY (outRtrId, &nextObjRtrIdValue, owaRtrId.len);

  /* return the object value: Area */
  xLibBufDataSet (outArea,
                  (xLibU8_t *) & nextObjAreaValue, sizeof (nextObjAreaValue));

  /* return the object value: RtrId */
  xLibBufDataSet (outRtrId,
                  (xLibU8_t *) & nextObjRtrIdValue, sizeof (nextObjRtrIdValue));
  FPOBJ_TRACE_EXIT (outArea, owaArea);
  FPOBJ_TRACE_EXIT (outRtrId, owaRtrId);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_routingospfVirtNbr_IpAddr
*
* @purpose Get 'IpAddr'
*
* @description [IpAddr]: The IP address this Virtual Neighbor is us- ing.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtNbr_IpAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRtrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Area */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_Area,
                           (xLibU8_t *) & keyAreaValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaValue, kwa1.len);

  /* retrieve key: RtrId */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_RtrId,
                           (xLibU8_t *) & keyRtrIdValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRtrIdValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtNbrIpAddrGet (L7_UNIT_CURRENT, keyAreaValue,
                                        keyRtrIdValue, &objIpAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpAddrValue,
                           sizeof (objIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfVirtNbr_Options
*
* @purpose Get 'Options'
*
* @description [Options]: A Bit Mask corresponding to the neighbor's op- tions
*              field. Bit 1, if set, indicates that the system will operate
*              on Type of Service metrics other than TOS 0. If zero,
*              the neighbor will ignore all metrics except the TOS 0 metric.
*              Bit 2, if set, indicates that the system is Network Multicast
*              capable; ie, that it imple- ments OSPF Multicast Routing.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtNbr_Options (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRtrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOptionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Area */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_Area,
                           (xLibU8_t *) & keyAreaValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaValue, kwa1.len);

  /* retrieve key: RtrId */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_RtrId,
                           (xLibU8_t *) & keyRtrIdValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRtrIdValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtNbrOptionsGet (L7_UNIT_CURRENT, keyAreaValue,
                                         keyRtrIdValue, &objOptionsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Options */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOptionsValue,
                           sizeof (objOptionsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfVirtNbr_State
*
* @purpose Get 'State'
*
* @description [State]: The state of the Virtual Neighbor Relation- ship.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtNbr_State (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRtrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Area */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_Area,
                           (xLibU8_t *) & keyAreaValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaValue, kwa1.len);

  /* retrieve key: RtrId */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_RtrId,
                           (xLibU8_t *) & keyRtrIdValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRtrIdValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtNbrStateGet (L7_UNIT_CURRENT, keyAreaValue,
                                       keyRtrIdValue, &objStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    /* Return NbrState as Down in case of failure */
    objStateValue = XUI_OSPF_DOWN; 
    owa.rc = XLIBRC_SUCCESS; /* TODO: Change if required */
  }

  /* return the object value: State */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStateValue,
                           sizeof (objStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfVirtNbr_Events
*
* @purpose Get 'Events'
*
* @description [Events]: The number of times this virtual link has changed
*              its state, or an error has occurred. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtNbr_Events (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRtrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEventsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Area */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_Area,
                           (xLibU8_t *) & keyAreaValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaValue, kwa1.len);

  /* retrieve key: RtrId */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_RtrId,
                           (xLibU8_t *) & keyRtrIdValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRtrIdValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtNbrEventsGet (L7_UNIT_CURRENT, keyAreaValue,
                                        keyRtrIdValue, &objEventsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Events */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEventsValue,
                           sizeof (objEventsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfVirtNbr_LsRetransQLen
*
* @purpose Get 'LsRetransQLen'
*
* @description [LsRetransQLen]: The current length of the retransmission queue.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtNbr_LsRetransQLen (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRtrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLsRetransQLenValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Area */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_Area,
                           (xLibU8_t *) & keyAreaValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaValue, kwa1.len);

  /* retrieve key: RtrId */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_RtrId,
                           (xLibU8_t *) & keyRtrIdValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRtrIdValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtNbrLsRetransQLenGet (L7_UNIT_CURRENT, keyAreaValue,
                                               keyRtrIdValue,
                                               &objLsRetransQLenValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LsRetransQLen */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLsRetransQLenValue,
                           sizeof (objLsRetransQLenValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfVirtNbr_HelloSuppressed
*
* @purpose Get 'HelloSuppressed'
*
* @description [HelloSuppressed]: Indicates whether Hellos are being suppressed
*              to the neighbor 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtNbr_HelloSuppressed (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRtrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHelloSuppressedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Area */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_Area,
                           (xLibU8_t *) & keyAreaValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaValue, kwa1.len);

  /* retrieve key: RtrId */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtNbr_RtrId,
                           (xLibU8_t *) & keyRtrIdValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRtrIdValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtNbrHelloSuppressedGet (L7_UNIT_CURRENT, keyAreaValue,
                                                 keyRtrIdValue,
                                                 &objHelloSuppressedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HelloSuppressed */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHelloSuppressedValue,
                           sizeof (objHelloSuppressedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
