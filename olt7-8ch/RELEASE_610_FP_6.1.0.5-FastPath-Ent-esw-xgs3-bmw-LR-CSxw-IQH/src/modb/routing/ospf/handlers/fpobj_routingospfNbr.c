/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfNbr.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfNbr-object.xml
*
* @create  1 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfNbr_obj.h"
#include "usmdb_mib_ospf_api.h"
#include "osapi.h"
#include "usmdb_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfNbr_IpAddr
*
* @purpose Get 'IpAddr'
 *@description  [IpAddr] The IP address this neighbor is using in its IP Source
* Address. Note that, on addressless links, this will not be
* 0.0.0.0, but the address of another of the neighbor's interfaces.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_IpAddr (void *wap, void *bufp)
{

  xLibIpV4_t objIpAddrValue;
  xLibIpV4_t nextObjIpAddrValue;
  xLibU32_t objAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr, (xLibU8_t *) & objIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objIpAddrValue, 0, sizeof (objIpAddrValue));
    memset (&objAddressLessIndexValue, 0, sizeof (objAddressLessIndexValue));
    owa.l7rc = usmDbOspfNbrEntryNext(L7_UNIT_CURRENT, &objIpAddrValue,
                         &objAddressLessIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddrValue, owa.len);
    nextObjIpAddrValue = objIpAddrValue;
    memset (&objAddressLessIndexValue, 0, sizeof (objAddressLessIndexValue));
    do
    {
      owa.l7rc = usmDbOspfNbrEntryNext(L7_UNIT_CURRENT, &objIpAddrValue,
                           &objAddressLessIndexValue);
    }
    while ((objIpAddrValue == nextObjIpAddrValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjIpAddrValue = objIpAddrValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIpAddrValue, owa.len);

  /* return the object value: IpAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIpAddrValue, sizeof (objIpAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfNbr_AddressLessIndex
*
* @purpose Get 'AddressLessIndex'
 *@description  [AddressLessIndex] On an interface having an IP Address, zero.
* On addressless interfaces, the corresponding value of ifIndex in
* the Internet Standard MIB. On row creation, this can be derived
* from the instance.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_AddressLessIndex (void *wap, void *bufp)
{

  xLibIpV4_t objIpAddrValue;
  xLibU32_t nextObjIpAddrValue;
  xLibU32_t objAddressLessIndexValue;
  xLibU32_t nextObjAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr, (xLibU8_t *) & objIpAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddrValue, owa.len);

  /* retrieve key: AddressLessIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                          (xLibU8_t *) & objAddressLessIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objAddressLessIndexValue, 0, sizeof (objAddressLessIndexValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAddressLessIndexValue, owa.len);
  }

  nextObjIpAddrValue = objIpAddrValue;
  owa.l7rc = usmDbOspfNbrEntryNext(L7_UNIT_CURRENT, &objIpAddrValue,
                       &objAddressLessIndexValue);

  if ((objIpAddrValue != nextObjIpAddrValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  nextObjAddressLessIndexValue = objAddressLessIndexValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAddressLessIndexValue, owa.len);

  /* return the object value: AddressLessIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAddressLessIndexValue,
                           sizeof (objAddressLessIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_routingospfNbr_IpAddr_AddressLessIndex
*
* @purpose Get 'IpAddr + AddressLessIndex +'
*
* @description [IpAddr]: The IP address this neighbor is using in its IP Source
*              Address. Note that, on addressless links, this will not
*              be 0.0.0.0, but the address of another of the neighbor's
*              interfaces. 
*              [AddressLessIndex]: On an interface having an IP Address, zero.
*              On addressless interfaces, the corresponding value of
*              ifIndex in the Internet Standard MIB. On row creation, this
*              can be derived from the instance. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_IpAddr_AddressLessIndex (void *wap,
                                                          void *bufp[],
                                                          xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaIpAddr = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIpAddrValue, nextObjIpAddrValue;
  fpObjWa_t owaAddressLessIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAddressLessIndexValue, nextObjAddressLessIndexValue;
  void *outIpAddr = (void *) bufp[--keyCount];
  void *outAddressLessIndex = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outIpAddr);
  FPOBJ_TRACE_ENTER (outAddressLessIndex);

  /* retrieve key: IpAddr */
  owaIpAddr.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                                (xLibU8_t *) & objIpAddrValue, &owaIpAddr.len);
  if (owaIpAddr.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: AddressLessIndex */
    owaAddressLessIndex.rc =
      xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                     (xLibU8_t *) & objAddressLessIndexValue,
                     &owaAddressLessIndex.len);
  }
  else
  {
     objIpAddrValue = 0;
     nextObjIpAddrValue = 0;
     objAddressLessIndexValue = 0;
     nextObjAddressLessIndexValue = 0;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (outIpAddr, &objIpAddrValue, owaIpAddr.len);
  FPOBJ_TRACE_CURRENT_KEY (outAddressLessIndex, &objAddressLessIndexValue,
                           owaAddressLessIndex.len);
  owa.rc = usmDbOspfNbrEntryNext(L7_UNIT_CURRENT, &objIpAddrValue,
                         &objAddressLessIndexValue);
 
  nextObjIpAddrValue = objIpAddrValue;
  nextObjAddressLessIndexValue = objAddressLessIndexValue;
                         
  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outIpAddr, owaIpAddr);
    FPOBJ_TRACE_EXIT (outAddressLessIndex, owaAddressLessIndex);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (outIpAddr, &nextObjIpAddrValue, owaIpAddr.len);
  FPOBJ_TRACE_CURRENT_KEY (outAddressLessIndex, &nextObjAddressLessIndexValue,
                           owaAddressLessIndex.len);

  /* return the object value: IpAddr */
  xLibBufDataSet (outIpAddr,
                  (xLibU8_t *) & nextObjIpAddrValue,
                  sizeof (nextObjIpAddrValue));

  /* return the object value: AddressLessIndex */
  xLibBufDataSet (outAddressLessIndex,
                  (xLibU8_t *) & nextObjAddressLessIndexValue,
                  sizeof (nextObjAddressLessIndexValue));
  FPOBJ_TRACE_EXIT (outIpAddr, owaIpAddr);
  FPOBJ_TRACE_EXIT (outAddressLessIndex, owaAddressLessIndex);
  return XLIBRC_SUCCESS;
}

#endif
/*******************************************************************************
* @function fpObjGet_routingospfNbr_RtrId
*
* @purpose Get 'RtrId'
*
* @description [RtrId]: A 32-bit integer (represented as a type IpAd- dress)
*              uniquely identifying the neighboring router in the Autonomous
*              System. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_RtrId (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRtrIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbrRtrIdGet (L7_UNIT_CURRENT, keyIpAddrValue,
                                   keyAddressLessIndexValue, &objRtrIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RtrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRtrIdValue,
                           sizeof (objRtrIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfNbr_Options
*
* @purpose Get 'Options'
*
* @description [Options]: A Bit Mask corresponding to the neighbor's op- tions
*              field. Bit 0, if set, indicates that the system will operate
*              on Type of Service metrics other than TOS 0. If zero,
*              the neighbor will ignore all metrics except the TOS 0 metric.
*              Bit 1, if set, indicates that the associated area accepts
*              and operates on external informa- tion; if zero, it is
*              a stub area. Bit 2, if set, indicates that the system is ca-
*              pable of routing IP Multicast datagrams; i.e., that it implements
*              the Multicast Extensions to OSPF. Bit 3, if set, indicates
*              that the associated area is an NSSA. These areas are
*              capable of carrying type 7 external advertisements, which
*              are translated into type 5 external advertise- ments at NSSA
*              borders. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_Options (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOptionsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbrOptionsGet (L7_UNIT_CURRENT, keyIpAddrValue,
                                     keyAddressLessIndexValue,
                                     &objOptionsValue);
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
* @function fpObjGet_routingospfNbr_Priority
*
* @purpose Get 'Priority'
*
* @description [Priority]: The priority of this neighbor in the designat-
*              ed router election algorithm. The value 0 sig- nifies that
*              the neighbor is not eligible to be- come the designated router
*              on this particular network. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_Priority (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbrPriorityGet (L7_UNIT_CURRENT, keyIpAddrValue,
                                      keyAddressLessIndexValue,
                                      &objPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Priority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPriorityValue,
                           sizeof (objPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfNbr_Priority
*
* @purpose Set 'Priority'
*
* @description [Priority]: The priority of this neighbor in the designat-
*              ed router election algorithm. The value 0 sig- nifies that
*              the neighbor is not eligible to be- come the designated router
*              on this particular network. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfNbr_Priority (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPriorityValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Priority */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPriorityValue, owa.len);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* set the value in application */
   
  #if 0
  WE DONT HAVE SET USMDB AND APPLICATION FUNCTION
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyIpAddrValue,
                              keyAddressLessIndexValue, objPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  #endif
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfNbr_State
*
* @purpose Get 'State'
*
* @description [State]: The State of the relationship with this Neigh- bor.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_State (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbrStateGet (L7_UNIT_CURRENT, keyIpAddrValue,
                                   keyAddressLessIndexValue, &objStateValue);
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
* @function fpObjGet_routingospfNbr_Events
*
* @purpose Get 'Events'
*
* @description [Events]: The number of times this neighbor relationship has
*              changed state, or an error has occurred. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_Events (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEventsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbrEventsGet (L7_UNIT_CURRENT, keyIpAddrValue,
                                    keyAddressLessIndexValue, &objEventsValue);
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
* @function fpObjGet_routingospfNbr_LsRetransQLen
*
* @purpose Get 'LsRetransQLen'
*
* @description [LsRetransQLen]: The current length of the retransmission queue.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_LsRetransQLen (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLsRetransQLenValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbrLsRetransQLenGet (L7_UNIT_CURRENT, keyIpAddrValue,
                                           keyAddressLessIndexValue,
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
* @function fpObjGet_routingospfNbr_ospfNbmaNbrStatus
*
* @purpose Get 'ospfNbmaNbrStatus'
*
* @description [ospfNbmaNbrStatus]: This variable displays the status of the
*              en- try. Setting it to 'invalid' has the effect of rendering
*              it inoperative. The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_ospfNbmaNbrStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objospfNbmaNbrStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbmaNbrStatusGet (L7_UNIT_CURRENT, keyIpAddrValue,
                                        keyAddressLessIndexValue,
                                        &objospfNbmaNbrStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ospfNbmaNbrStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objospfNbmaNbrStatusValue,
                           sizeof (objospfNbmaNbrStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfNbr_ospfNbmaNbrStatus
*
* @purpose Set 'ospfNbmaNbrStatus'
*
* @description [ospfNbmaNbrStatus]: This variable displays the status of the
*              en- try. Setting it to 'invalid' has the effect of rendering
*              it inoperative. The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfNbr_ospfNbmaNbrStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objospfNbmaNbrStatusValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ospfNbmaNbrStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objospfNbmaNbrStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objospfNbmaNbrStatusValue, owa.len);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* call the usmdb only for add and delete */
  if (objospfNbmaNbrStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    #if 0
    we should not set this object
    /* Create a row */
    owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyIpAddrValue,
                                keyAddressLessIndexValue,
                                objospfNbmaNbrStatusValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    #endif
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objospfNbmaNbrStatusValue == L7_ROW_STATUS_DESTROY)
  {
    #if 0
    we should not set this object
    /* Delete the existing row */
    owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyIpAddrValue,
                                keyAddressLessIndexValue,
                                objospfNbmaNbrStatusValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    #endif
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjGet_routingospfNbr_ospfNbmaNbrPermanence
*
* @purpose Get 'ospfNbmaNbrPermanence'
*
* @description [ospfNbmaNbrPermanence]: This variable displays the status
*              of the en- try. 'dynamic' and 'permanent' refer to how the
*              neighbor became known. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_ospfNbmaNbrPermanence (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objospfNbmaNbrPermanenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbmaNbrPermanenceGet (L7_UNIT_CURRENT, keyIpAddrValue,
                                            keyAddressLessIndexValue,
                                            &objospfNbmaNbrPermanenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ospfNbmaNbrPermanence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objospfNbmaNbrPermanenceValue,
                           sizeof (objospfNbmaNbrPermanenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfNbr_HelloSuppressed
*
* @purpose Get 'HelloSuppressed'
*
* @description [HelloSuppressed]: Indicates whether Hellos are being suppressed
*              to the neighbor 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_HelloSuppressed (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objHelloSuppressedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbrHelloSuppressedGet (L7_UNIT_CURRENT, keyIpAddrValue,
                                             keyAddressLessIndexValue,
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

/*******************************************************************************
* @function fpObjGet_routingospfNbr_UpTime
*
* @purpose Get 'UpTime'
*
* @description [UpTime]: Get the number of seconds since adjacency last went
*              to Full state with a given neighbor. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_UpTime (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUpTimeValue;
  L7_char8 upTimeInfo[64];
  L7_timespec tsUptime;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNbrUptimeGet (keyAddressLessIndexValue, keyIpAddrValue, &objUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  osapiConvertRawUpTime(objUpTimeValue, &tsUptime);  
  osapiSnprintf(upTimeInfo, sizeof(upTimeInfo), "%d days %d hrs %d mins %d secs",
                    tsUptime.days, tsUptime.hours, tsUptime.minutes, tsUptime.seconds);

  /* return the object value: UpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & upTimeInfo,
                           strlen (upTimeInfo));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfNbr_Area
*
* @purpose Get 'Area'
*
* @description [Area]: Get the Ospf Area of the specified neighbour router
*              Id of the specified interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_Area (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAreaValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNeighbourAreaGet (L7_UNIT_CURRENT, keyAddressLessIndexValue, keyIpAddrValue,
                                        &objAreaValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Area */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAreaValue,
                           sizeof (objAreaValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfNbr_DeadTimerRemaining
*
* @purpose Get 'DeadTimerRemaining'
*
* @description [DeadTimerRemaining]: Get the Ospf Dead timer remaining of
*              the specified neighbour router Id of the specified interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_DeadTimerRemaining (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDeadTimerRemainingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfNeighbourDeadTimerRemainingGet (L7_UNIT_CURRENT, keyAddressLessIndexValue, keyIpAddrValue,
                                             &objDeadTimerRemainingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objDeadTimerRemainingValue /=1000;
  /* return the object value: DeadTimerRemaining */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDeadTimerRemainingValue,
                           sizeof (objDeadTimerRemainingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfNbr_IfIndex
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex]: Get the Ospf Interface index of the specified neighbour
*              router Id of the specified interface 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfNbr_IfIndex (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIpAddrValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IpAddr */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                           (xLibU8_t *) & keyIpAddrValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwa1.len);

  /* retrieve key: AddressLessIndex */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                           (xLibU8_t *) & keyAddressLessIndexValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNeighbourIfIndexGet (L7_UNIT_CURRENT, keyAddressLessIndexValue, keyIpAddrValue,
                                           &objIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIfIndexValue,
                           sizeof (objIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfNbr_NeighboursClear
*
* @purpose Set 'NeighboursClear'
 *@description  [NeighboursClear] Clear OSPFv2 neighbors.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfNbr_NeighboursClear (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNeighboursClearValue;

  fpObjWa_t kwaIpAddr = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyIpAddrValue;
  fpObjWa_t kwaAddressLessIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressLessIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NeighboursClear */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNeighboursClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNeighboursClearValue, owa.len);

  /* retrieve key: IpAddr */
  kwaIpAddr.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_IpAddr,
                                (xLibU8_t *) & keyIpAddrValue, &kwaIpAddr.len);
  if (kwaIpAddr.rc != XLIBRC_SUCCESS)
  {
    kwaIpAddr.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIpAddr);
    return kwaIpAddr.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIpAddrValue, kwaIpAddr.len);

  /* retrieve key: AddressLessIndex */
  kwaAddressLessIndex.rc = xLibFilterGet (wap, XOBJ_routingospfNbr_AddressLessIndex,
                                          (xLibU8_t *) & keyAddressLessIndexValue,
                                          &kwaAddressLessIndex.len);
  if (kwaAddressLessIndex.rc != XLIBRC_SUCCESS)
  {
    kwaAddressLessIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAddressLessIndex);
    return kwaAddressLessIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressLessIndexValue, kwaAddressLessIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfMapNeighborClear (keyAddressLessIndexValue,keyIpAddrValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

