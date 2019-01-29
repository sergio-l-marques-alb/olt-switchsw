/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingRip2Peer.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to Rip2PeerStats-object.xml
*
* @create  22 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingRip2Peer_obj.h"
#include "usmdb_mib_ripv2_api.h"

/*******************************************************************************
* @function fpObjGet_routingRip2Peer_Address
*
* @purpose Get 'Address'
 *@description  [Address] The IP Address that the peer is using as its source
* address. Note that on an unnumbered link, this may not be a member
* of any subnet on the system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2Peer_Address (void *wap, void *bufp)
{

  xLibIpV4_t objAddressValue;
  xLibIpV4_t nextObjAddressValue;
  xLibStr256_t objDomainValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Address,
                          (xLibU8_t *) & objAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objAddressValue, 0, sizeof (objAddressValue));
    memset (objDomainValue, 0, sizeof (objDomainValue));
    owa.l7rc =
      usmDbRip2PeerEntryNext(L7_UNIT_CURRENT, &objAddressValue, objDomainValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objAddressValue, owa.len);
    nextObjAddressValue = objAddressValue;
    memset (objDomainValue, 0, sizeof (objDomainValue));
    do
    {
      owa.l7rc =
        usmDbRip2PeerEntryNext(L7_UNIT_CURRENT, &objAddressValue, objDomainValue);
    }
    while ((objAddressValue == nextObjAddressValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjAddressValue, owa.len);

  /* return the object value: Address */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjAddressValue, sizeof (objAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingRip2Peer_Domain
*
* @purpose Get 'Domain'
 *@description  [Domain] The value in the Routing Domain field in RIP packets
* received from the peer. As domain suuport is deprecated, this must
* be zero.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2Peer_Domain (void *wap, void *bufp)
{

  xLibIpV4_t objAddressValue;
  xLibIpV4_t nextObjAddressValue;
  xLibStr256_t objDomainValue;
  xLibStr256_t nextObjDomainValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  owa.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Address,
                          (xLibU8_t *) & objAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objAddressValue, owa.len);

  /* retrieve key: Domain */
  owa.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Domain, (xLibU8_t *) objDomainValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objDomainValue, 0, sizeof (objDomainValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objDomainValue, owa.len);
  }

  nextObjAddressValue = objAddressValue;
  owa.l7rc =
    usmDbRip2PeerEntryNext(L7_UNIT_CURRENT, &objAddressValue, objDomainValue);
  
  if ((objAddressValue != nextObjAddressValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  strcpy(nextObjDomainValue,objDomainValue);
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjDomainValue, strlen(nextObjDomainValue));

  /* return the object value: Domain */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjDomainValue, strlen (objDomainValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

#if 0
/*******************************************************************************
* @function fpObjGet_routingRip2Peer_Address_Domain
*
* @purpose Get 'Address + Domain +'
*
* @description [Address]: The IP Address that the peer is using as its source
*              address. Note that on an unnumbered link, this may not
*              be a member of any subnet on the system. 
*              [Domain]: The value in the Routing Domain field in RIP packets
*              received from the peer. As domain suuport is deprecated,
*              this must be zero. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2Peer_Address_Domain (void *wap, void *bufp[],
                                                  xLibU16_t keyCount)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaAddress = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAddressValue, nextObjAddressValue;
  fpObjWa_t owaDomain = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDomainValue, nextObjDomainValue;
  void *outAddress = (void *) bufp[--keyCount];
  void *outDomain = (void *) bufp[--keyCount];
  FPOBJ_TRACE_ENTER (outAddress);
  FPOBJ_TRACE_ENTER (outDomain);

  /* retrieve key: Address */
  owaAddress.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Address,
                                 (xLibU8_t *) & objAddressValue,
                                 &owaAddress.len);
  if (owaAddress.rc == XLIBRC_SUCCESS)
  {
    /* retrieve key: Domain */
    owaDomain.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Domain,
                                  (xLibU8_t *) objDomainValue, &owaDomain.len);
  }
  else
  {
    objAddressValue = 0;
    memset(objDomainValue,0, sizeof(objDomainValue));
  }

  FPOBJ_TRACE_CURRENT_KEY (outAddress, &objAddressValue, owaAddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outDomain, &objDomainValue, owaDomain.len);

  owa.rc =
    usmDbRip2PeerEntryNext(L7_UNIT_CURRENT, &objAddressValue, objDomainValue);

  if (owa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (outAddress, owaAddress);
    FPOBJ_TRACE_EXIT (outDomain, owaDomain);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (outAddress, &nextObjAddressValue, owaAddress.len);
  FPOBJ_TRACE_CURRENT_KEY (outDomain, &nextObjDomainValue, owaDomain.len);

  nextObjAddressValue = objAddressValue;
  osapiStrncpy(nextObjDomainValue, objDomainValue, sizeof(objDomainValue));
  
  /* return the object value: Address */
  xLibBufDataSet (outAddress,
                  (xLibU8_t *) & nextObjAddressValue,
                  sizeof (nextObjAddressValue));

  /* return the object value: Domain */
  xLibBufDataSet (outDomain,
                  (xLibU8_t *) nextObjDomainValue, strlen (nextObjDomainValue));
  FPOBJ_TRACE_EXIT (outAddress, owaAddress);
  FPOBJ_TRACE_EXIT (outDomain, owaDomain);
  return XLIBRC_SUCCESS;
}
#endif

/*******************************************************************************
* @function fpObjGet_routingRip2Peer_LastUpdate
*
* @purpose Get 'LastUpdate'
*
* @description [LastUpdate]: The value of sysUpTime when the most recent RIP
*              update was received from this system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2Peer_LastUpdate (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyDomainValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLastUpdateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Address,
                           (xLibU8_t *) & keyAddressValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa1.len);

  /* retrieve key: Domain */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Domain,
                           (xLibU8_t *) keyDomainValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDomainValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbRip2PeerLastUpdateGet (L7_UNIT_CURRENT, keyAddressValue,
                                         keyDomainValue, &objLastUpdateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LastUpdate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLastUpdateValue,
                           sizeof (objLastUpdateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingRip2Peer_Version
*
* @purpose Get 'Version'
*
* @description [Version]: The RIP version number in the header of the last
*              RIP packet received. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2Peer_Version (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyDomainValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Address,
                           (xLibU8_t *) & keyAddressValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa1.len);

  /* retrieve key: Domain */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Domain,
                           (xLibU8_t *) keyDomainValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDomainValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbRip2PeerVersionGet (L7_UNIT_CURRENT, keyAddressValue,
                                      keyDomainValue, &objVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Version */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVersionValue,
                           sizeof (objVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingRip2Peer_RcvBadPackets
*
* @purpose Get 'RcvBadPackets'
*
* @description [RcvBadPackets]: The number of RIP response packets from this
*              peer discarded as invalid. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2Peer_RcvBadPackets (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyDomainValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvBadPacketsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Address,
                           (xLibU8_t *) & keyAddressValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa1.len);

  /* retrieve key: Domain */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Domain,
                           (xLibU8_t *) keyDomainValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDomainValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbRip2PeerRcvBadPacketsGet (L7_UNIT_CURRENT, keyAddressValue,
                                            keyDomainValue,
                                            &objRcvBadPacketsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvBadPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRcvBadPacketsValue,
                           sizeof (objRcvBadPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingRip2Peer_RcvBadRoutes
*
* @purpose Get 'RcvBadRoutes'
*
* @description [RcvBadRoutes]: The number of routes from this peer that were
*              ignored because the entry format was invalid. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingRip2Peer_RcvBadRoutes (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyAddressValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyDomainValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRcvBadRoutesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Address */
  kwa1.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Address,
                           (xLibU8_t *) & keyAddressValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyAddressValue, kwa1.len);

  /* retrieve key: Domain */
  kwa2.rc = xLibFilterGet (wap, XOBJ_routingRip2Peer_Domain,
                           (xLibU8_t *) keyDomainValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyDomainValue, kwa2.len);

  /* get the value from application */
  owa.l7rc = usmDbRip2PeerRcvBadRoutesGet (L7_UNIT_CURRENT, keyAddressValue,
                                           keyDomainValue,
                                           &objRcvBadRoutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RcvBadRoutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRcvBadRoutesValue,
                           sizeof (objRcvBadRoutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
