/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfVirtualInterface.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfVitualInterface-object.xml
*
* @create  1 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfVirtualInterface_obj.h"
#include "usmdb_mib_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfVirtualInterface_AreaId
*
* @purpose Get 'AreaId'
 *@description  [AreaId] The Transit Area that the Virtual Link traverses. By
* definition, this is not 0.0.0.0   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_AreaId (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIdValue;
  xLibIpV4_t nextObjAreaIdValue;
  xLibIpV4_t objNeighborValue;
  xLibIpV4_t nextObjNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memset (&nextObjAreaIdValue, 0, sizeof (nextObjAreaIdValue));
    memset (&nextObjNeighborValue, 0, sizeof (nextObjNeighborValue));

    owa.l7rc =
      usmDbOspfVirtIfEntryNext(L7_UNIT_CURRENT, &nextObjAreaIdValue, &nextObjNeighborValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);
    nextObjAreaIdValue = objAreaIdValue; 
    memset (&objNeighborValue, 0, sizeof (objNeighborValue));
    do
    {
      owa.l7rc =
        usmDbOspfVirtIfEntryNext(L7_UNIT_CURRENT, &nextObjAreaIdValue, &nextObjNeighborValue);
    }
    while ((objAreaIdValue == nextObjAreaIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAreaIdValue, owa.len);

  /* return the object value: AreaId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAreaIdValue, sizeof (objAreaIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfVirtualInterface_Neighbor
*
* @purpose Get 'Neighbor'
 *@description  [Neighbor] The Router ID of the Virtual Neighbor.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_Neighbor (void *wap, void *bufp)
{

  xLibIpV4_t objAreaIdValue;
  xLibIpV4_t nextObjAreaIdValue;
  xLibIpV4_t objNeighborValue;
  xLibIpV4_t nextObjNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                          (xLibU8_t *) & objAreaIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAreaIdValue, owa.len);

  /* retrieve key: Neighbor */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                          (xLibU8_t *) & objNeighborValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objNeighborValue, 0, sizeof (objNeighborValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objNeighborValue, owa.len);
  }
  nextObjAreaIdValue = objAreaIdValue;
  nextObjNeighborValue = objNeighborValue;

  owa.l7rc =
    usmDbOspfVirtIfEntryNext(L7_UNIT_CURRENT, &nextObjAreaIdValue, &nextObjNeighborValue);
  
  if ((objAreaIdValue != nextObjAreaIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjNeighborValue, owa.len);

  /* return the object value: Neighbor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjNeighborValue, sizeof (objNeighborValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfVirtualInterface_TransitDelay
*
* @purpose Get 'TransitDelay'
*
* @description [TransitDelay]: The estimated number of seconds it takes to
*              transmit a link- state update packet over this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_TransitDelay (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTransitDelayValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfTransitDelayGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                             keyNeighborValue,
                                             &objTransitDelayValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TransitDelay */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTransitDelayValue,
                           sizeof (objTransitDelayValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfVirtualInterface_TransitDelay
*
* @purpose Set 'TransitDelay'
*
* @description [TransitDelay]: The estimated number of seconds it takes to
*              transmit a link- state update packet over this interface.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfVirtualInterface_TransitDelay (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTransitDelayValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TransitDelay */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTransitDelayValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTransitDelayValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfVirtIfTransitDelaySet (L7_UNIT_CURRENT, keyAreaIdValue,
                                             keyNeighborValue,
                                             objTransitDelayValue);
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
* @function fpObjGet_routingospfVirtualInterface_RetransInterval
*
* @purpose Get 'RetransInterval'
*
* @description [RetransInterval]: The number of seconds between link-state
*              ad- vertisement retransmissions, for adjacencies belonging
*              to this interface. This value is also used when retransmitting
*              database descrip- tion and link-state request packets.
*              This value should be well over the expected round- trip time.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_RetransInterval (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRetransIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfRetransIntervalGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                                keyNeighborValue,
                                                &objRetransIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RetransInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRetransIntervalValue,
                           sizeof (objRetransIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfVirtualInterface_RetransInterval
*
* @purpose Set 'RetransInterval'
*
* @description [RetransInterval]: The number of seconds between link-state
*              ad- vertisement retransmissions, for adjacencies belonging
*              to this interface. This value is also used when retransmitting
*              database descrip- tion and link-state request packets.
*              This value should be well over the expected round- trip time.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfVirtualInterface_RetransInterval (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRetransIntervalValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RetransInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRetransIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRetransIntervalValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfVirtIfRetransIntervalSet (L7_UNIT_CURRENT, keyAreaIdValue,
                                                keyNeighborValue,
                                                objRetransIntervalValue);
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
* @function fpObjGet_routingospfVirtualInterface_HelloInterval
*
* @purpose Get 'HelloInterval'
*
* @description [HelloInterval]: The length of time, in seconds, between the
*              Hello packets that the router sends on the in- terface. This
*              value must be the same for the virtual neighbor. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_HelloInterval (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHelloIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfHelloIntervalGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                              keyNeighborValue,
                                              &objHelloIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HelloInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHelloIntervalValue,
                           sizeof (objHelloIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfVirtualInterface_HelloInterval
*
* @purpose Set 'HelloInterval'
*
* @description [HelloInterval]: The length of time, in seconds, between the
*              Hello packets that the router sends on the in- terface. This
*              value must be the same for the virtual neighbor. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfVirtualInterface_HelloInterval (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHelloIntervalValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HelloInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objHelloIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHelloIntervalValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfVirtIfHelloIntervalSet (L7_UNIT_CURRENT, keyAreaIdValue,
                                              keyNeighborValue,
                                              objHelloIntervalValue);
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
* @function fpObjGet_routingospfVirtualInterface_RtrDeadInterval
*
* @purpose Get 'RtrDeadInterval'
*
* @description [RtrDeadInterval]: The number of seconds that a router's Hello
*              packets have not been seen before it's neigh- bors declare
*              the router down. This should be some multiple of the Hello
*              interval. This value must be the same for the virtual neigh-
*              bor. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_RtrDeadInterval (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRtrDeadIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfRtrDeadIntervalGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                                keyNeighborValue,
                                                &objRtrDeadIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RtrDeadInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRtrDeadIntervalValue,
                           sizeof (objRtrDeadIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfVirtualInterface_RtrDeadInterval
*
* @purpose Set 'RtrDeadInterval'
*
* @description [RtrDeadInterval]: The number of seconds that a router's Hello
*              packets have not been seen before it's neigh- bors declare
*              the router down. This should be some multiple of the Hello
*              interval. This value must be the same for the virtual neigh-
*              bor. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfVirtualInterface_RtrDeadInterval (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRtrDeadIntervalValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RtrDeadInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objRtrDeadIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRtrDeadIntervalValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfVirtIfRtrDeadIntervalSet (L7_UNIT_CURRENT, keyAreaIdValue,
                                                keyNeighborValue,
                                                objRtrDeadIntervalValue);
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
* @function fpObjGet_routingospfVirtualInterface_State
*
* @purpose Get 'State'
*
* @description [State]: OSPF virtual interface states. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_State (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfStateGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                      keyNeighborValue, &objStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: State */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStateValue,
                           sizeof (objStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfVirtualInterface_Events
*
* @purpose Get 'Events'
*
* @description [Events]: The number of state changes or error events on this
*              Virtual Link 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_Events (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEventsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfEventsGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                       keyNeighborValue, &objEventsValue);
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
* @function fpObjGet_routingospfVirtualInterface_AuthKey
*
* @purpose Get 'AuthKey'
*
* @description [AuthKey]: If Authentication Type is simplePassword, the device
*              will left adjust and zero fill to 8 oc- tets. Note that
*              unauthenticated interfaces need no authentication key, and
*              simple password authen- tication cannot use a key of more
*              than 8 oc- tets. Larger keys are useful only with authen-
*              tication mechanisms not specified in this docu- ment. When
*              read, ospfVifAuthKey always returns a string of length zero.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_AuthKey (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfAuthKeyGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                        keyNeighborValue, objAuthKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAuthKeyValue,
                           strlen (objAuthKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfVirtualInterface_AuthKey
*
* @purpose Set 'AuthKey'
*
* @description [AuthKey]: If Authentication Type is simplePassword, the device
*              will left adjust and zero fill to 8 oc- tets. Note that
*              unauthenticated interfaces need no authentication key, and
*              simple password authen- tication cannot use a key of more
*              than 8 oc- tets. Larger keys are useful only with authen-
*              tication mechanisms not specified in this docu- ment. When
*              read, ospfVifAuthKey always returns a string of length zero.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfVirtualInterface_AuthKey (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objAuthKeyValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthKey */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAuthKeyValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAuthKeyValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);
  
  /* set the value in application */
  owa.l7rc = usmDbOspfVirtIfAuthKeySet(L7_UNIT_CURRENT, keyAreaIdValue,
                              keyNeighborValue, objAuthKeyValue);
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
* @function fpObjGet_routingospfVirtualInterface_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This variable displays the status of the en- try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfStatusGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                       keyNeighborValue, &objStatusValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objStatusValue == L7_OSPF_ROW_ACTIVE)
  {
    objStatusValue = L7_OSPF_ROW_ACTIVE;
  }
  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfVirtualInterface_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This variable displays the status of the en- try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfVirtualInterface_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  if(keyAreaIdValue == 0)
  {
    owa.rc = XLIBRC_OSPF_VLINK_AREA_BACKBONE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = L7_SUCCESS;
  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc =
      usmDbOspfVirtIfCreate(L7_UNIT_CURRENT, keyAreaIdValue, keyNeighborValue);
    if(owa.l7rc == L7_ERROR)
    {
      owa.rc = XLIBRC_OSPF_VLINK_RTRID_MYRTRID;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else if(owa.l7rc == L7_REQUEST_DENIED)
    {
      owa.rc = XLIBRC_OSPF_VLINK_TRANSITAREA_STUB_NSSA;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc =
      usmDbOspfVirtIfDelete(L7_UNIT_CURRENT, keyAreaIdValue, keyNeighborValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfVirtualInterface_AuthType
*
* @purpose Get 'AuthType'
*
* @description [AuthType]: The authentication type specified for a virtu-
*              al interface. Additional authentication types may be assigned
*              locally. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_AuthType (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfAuthTypeGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                         keyNeighborValue, &objAuthTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthTypeValue,
                           sizeof (objAuthTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfVirtualInterface_AuthType
*
* @purpose Set 'AuthType'
*
* @description [AuthType]: The authentication type specified for a virtu-
*              al interface. Additional authentication types may be assigned
*              locally. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfVirtualInterface_AuthType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthTypeValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthTypeValue, owa.len);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfVirtIfAuthTypeSet (L7_UNIT_CURRENT, keyAreaIdValue,
                                         keyNeighborValue, objAuthTypeValue);
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
* @function fpObjGet_routingospfVirtualInterface_Metric
*
* @purpose Get 'Metric'
*
* @description [Metric]: The metric of using this type of service on this
*              interface. The default value of the TOS 0 Metric is 10^8 /
*              ifSpeed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_Metric (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfMetricGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                       keyNeighborValue, &objMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Metric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMetricValue,
                           sizeof (objMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfVirtualInterface_ActualAuthKey
*
* @purpose Get 'ActualAuthKey'
*
* @description [ActualAuthKey]: This API does provide the contents of the
*              authentication key, thus it should not be used if the key contents
*              are to be kept hidden, such as when supporting the
*              OSPF MIB. Use usmDbOspfVirtIfAuthKeyGet instead. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_ActualAuthKey (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAreaIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyNeighborValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objActualAuthKeyValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: AreaId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_AreaId,
                           (xLibU8_t *) & keyAreaIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAreaIdValue, kwa1.len);

  /* retrieve key: Neighbor */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfVirtualInterface_Neighbor,
                           (xLibU8_t *) & keyNeighborValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyNeighborValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfVirtIfAuthKeyActualGet (L7_UNIT_CURRENT, keyAreaIdValue,
                                              keyNeighborValue,
                                              objActualAuthKeyValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ActualAuthKey */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objActualAuthKeyValue,
                           strlen (objActualAuthKeyValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfVirtualInterface_VirtLinkCreatable
*
* @purpose Get 'VirtLinkCreatable'
*
* @description [VirtLinkCreatable]: TODO 
*
* @return  returns 'true' if there is atleast one area eligible as transit area
*******************************************************************************/
xLibRC_t fpObjGet_routingospfVirtualInterface_VirtLinkCreatable (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t objVirtLinkCreatable;
  xLibU32_t areaId = 0, val = 0;

  do
  {
    owa.l7rc = usmDbOspfAreaIdGetNext (L7_UNIT_CURRENT, areaId, &areaId);
    if(owa.l7rc == L7_SUCCESS)
    {
      if((areaId != 0) &&
         (usmDbOspfAreaExternalRoutingCapabilityGet(L7_UNIT_CURRENT, areaId, &val) == L7_SUCCESS) &&
         (val != L7_OSPF_AREA_IMPORT_NSSA) &&
         (val != L7_OSPF_AREA_IMPORT_NO_EXT))
      {
        break;
      }
    }  
  }
  while(owa.l7rc == L7_SUCCESS);
  if(owa.l7rc == L7_SUCCESS)
  {
    objVirtLinkCreatable = L7_XUI_TRUE;
  }
  else
  {
    objVirtLinkCreatable = L7_XUI_FALSE;
  }

  /* return the object value: objVirtLinkCreatable */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objVirtLinkCreatable,
                                      sizeof(objVirtLinkCreatable));

  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}
