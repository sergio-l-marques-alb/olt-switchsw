
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingospfInterfaceStats.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  29 July 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingospfInterfaceStats_obj.h"
#include "usmdb_ospf_api.h"
#include "usmdb_util_api.h"
/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] Interface Number.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objInterfaceValue = 0;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
  }

  do
  {
    owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue, &nextObjInterfaceValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      break;
    }
    if (usmDbOspfIntfExists(L7_UNIT_CURRENT, nextObjInterfaceValue) == L7_TRUE)
    {
      break;
    }
    objInterfaceValue = nextObjInterfaceValue;
  }while (owa.l7rc == L7_SUCCESS);

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
* @function fpObjGet_routingospfInterfaceStats_RxPackets
*
* @purpose Get 'RxPackets'
 *@description  [RxPackets] The number of valid OSPF packets received on the
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_RxPackets (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRxPacketsValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objRxPacketsValue = intfStats.RxPackets;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRxPacketsValue, sizeof (objRxPacketsValue));

  /* return the object value: RxPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxPacketsValue, sizeof (objRxPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_DiscardPackets
*
* @purpose Get 'DiscardPackets'
 *@description  [DiscardPackets] The number of received OSPF packets discarded
* because of an error in the packet or an error in processing the
* packet.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_DiscardPackets (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDiscardPacketsValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objDiscardPacketsValue = intfStats.DiscardPackets;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDiscardPacketsValue, sizeof (objDiscardPacketsValue));

  /* return the object value: DiscardPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDiscardPacketsValue,
                           sizeof (objDiscardPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_BadVersion
*
* @purpose Get 'BadVersion'
 *@description  [BadVersion] The number of received OSPF packets whose version
* field in the OSPF header does not match the version of the OSPF
* process handling the packet.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_BadVersion (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objBadVersionValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objBadVersionValue = intfStats.BadVersion;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadVersionValue, sizeof (objBadVersionValue));

  /* return the object value: BadVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadVersionValue, sizeof (objBadVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_BadNetwork
*
* @purpose Get 'BadNetwork'
 *@description  [BadNetwork] The number of received packets discarded because
* the source IP address is not within a subnet configured on a local
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_BadNetwork (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objBadNetworkValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objBadNetworkValue = intfStats.BadNetwork;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadNetworkValue, sizeof (objBadNetworkValue));

  /* return the object value: BadNetwork */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadNetworkValue, sizeof (objBadNetworkValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_BadVirtualLink
*
* @purpose Get 'BadVirtualLink'
 *@description  [BadVirtualLink] The number of received OSPF packets discarded
* where the ingress interface is in a non-backbone area and the OSPF
* header identifies the packet as belonging to the backbone, but
* OSPF does not have a virtual link to the packet's sender.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_BadVirtualLink (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objBadVirtualLinkValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objBadVirtualLinkValue = intfStats.BadVirtualLink;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadVirtualLinkValue, sizeof (objBadVirtualLinkValue));

  /* return the object value: BadVirtualLink */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadVirtualLinkValue,
                           sizeof (objBadVirtualLinkValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_BadArea
*
* @purpose Get 'BadArea'
 *@description  [BadArea] The number of OSPF packets discarded because the area
* ID in the OSPF header is not the area ID configured on the
* ingress interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_BadArea (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objBadAreaValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objBadAreaValue = intfStats.BadArea;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadAreaValue, sizeof (objBadAreaValue));

  /* return the object value: BadArea */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadAreaValue, sizeof (objBadAreaValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_BadDstAdr
*
* @purpose Get 'BadDstAdr'
 *@description  [BadDstAdr] The number of OSPF packets discarded because the
* packet's destination IP address is not the address of the ingress
* interface and is not the AllDrRouters or AllSpfRouters multicast
* addresses.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_BadDstAdr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objBadDstAdrValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objBadDstAdrValue = intfStats.BadDstAdr;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadDstAdrValue, sizeof (objBadDstAdrValue));

  /* return the object value: BadDstAdr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadDstAdrValue, sizeof (objBadDstAdrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_BadAuType
*
* @purpose Get 'BadAuType'
 *@description  [BadAuType] The number of packets discarded because the
* authentication type specified in the OSPF header does not match the
* authentication type configured on the ingress interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_BadAuType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objBadAuTypeValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objBadAuTypeValue = intfStats.BadAuType;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadAuTypeValue, sizeof (objBadAuTypeValue));

  /* return the object value: BadAuType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadAuTypeValue, sizeof (objBadAuTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_BadAuthentication
*
* @purpose Get 'BadAuthentication'
 *@description  [BadAuthentication] The number of OSPF packets dropped because
* the sender is not an existing neighbor or the sender's IP address
* does not match the previously recorded IP address for that
* neighbor.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_BadAuthentication (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objBadAuthenticationValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objBadAuthenticationValue = intfStats.BadAuthentication;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadAuthenticationValue, sizeof (objBadAuthenticationValue));

  /* return the object value: BadAuthentication */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadAuthenticationValue,
                           sizeof (objBadAuthenticationValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_BadNeighbor
*
* @purpose Get 'BadNeighbor'
 *@description  [BadNeighbor] The number of OSPF packets dropped because the
* sender is not an existing neighbor or the sender's IP address does
* not match the previously recorded IP address for that neighbor.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_BadNeighbor (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objBadNeighborValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objBadNeighborValue = intfStats.BadNeighbor;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadNeighborValue, sizeof (objBadNeighborValue));

  /* return the object value: BadNeighbor */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadNeighborValue, sizeof (objBadNeighborValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_BadPckType
*
* @purpose Get 'BadPckType'
 *@description  [BadPckType] The number of OSPF packets discarded because the
* packet type field in the OSPF header is not a known type.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_BadPckType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objBadPckTypeValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objBadPckTypeValue = intfStats.BadPckType;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadPckTypeValue, sizeof (objBadPckTypeValue));

  /* return the object value: BadPckType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadPckTypeValue, sizeof (objBadPckTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_TxPackets
*
* @purpose Get 'TxPackets'
 *@description  [TxPackets] The number of OSPF packets transmitted on the
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_TxPackets (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTxPacketsValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objTxPacketsValue = intfStats.TxPackets;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTxPacketsValue, sizeof (objTxPacketsValue));

  /* return the object value: TxPackets */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxPacketsValue, sizeof (objTxPacketsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_RxHellos
*
* @purpose Get 'RxHellos'
 *@description  [RxHellos] The number of Hello packets received on this
* interface by this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_RxHellos (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRxHellosValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objRxHellosValue = intfStats.RxHellos;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRxHellosValue, sizeof (objRxHellosValue));

  /* return the object value: RxHellos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxHellosValue, sizeof (objRxHellosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_RxDbDescr
*
* @purpose Get 'RxDbDescr'
 *@description  [RxDbDescr] The number of Database Description packets received
* on this interface by this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_RxDbDescr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRxDbDescrValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objRxDbDescrValue = intfStats.RxDbDescr;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRxDbDescrValue, sizeof (objRxDbDescrValue));

  /* return the object value: RxDbDescr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxDbDescrValue, sizeof (objRxDbDescrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_RxLsReq
*
* @purpose Get 'RxLsReq'
 *@description  [RxLsReq] The number of LS Requests received on this interface
* by this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_RxLsReq (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRxLsReqValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objRxLsReqValue = intfStats.RxLsReq;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRxLsReqValue, sizeof (objRxLsReqValue));

  /* return the object value: RxLsReq */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxLsReqValue, sizeof (objRxLsReqValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_RxLsUpdate
*
* @purpose Get 'RxLsUpdate'
 *@description  [RxLsUpdate] The number of LS updates received on this interface
* by this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_RxLsUpdate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRxLsUpdateValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objRxLsUpdateValue = intfStats.RxLsUpdate;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRxLsUpdateValue, sizeof (objRxLsUpdateValue));

  /* return the object value: RxLsUpdate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxLsUpdateValue, sizeof (objRxLsUpdateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_RxLsAck
*
* @purpose Get 'RxLsAck'
 *@description  [RxLsAck] The number of LS acknowledgements received on this
* interface by this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_RxLsAck (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objRxLsAckValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objRxLsAckValue = intfStats.RxLsAck;
  }

  FPOBJ_TRACE_VALUE (bufp, &objRxLsAckValue, sizeof (objRxLsAckValue));

  /* return the object value: RxLsAck */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxLsAckValue, sizeof (objRxLsAckValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_HellosIgnored
*
* @purpose Get 'HellosIgnored'
 *@description  [HellosIgnored] The number of received Hello packets that were
* ignored by this router from the new neighbors after the limit has
* been reached for the number of neighbors on an interface or on
* the system as a whole.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_HellosIgnored (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHellosIgnoredValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objHellosIgnoredValue = intfStats.HellosIgnored;
  }

  FPOBJ_TRACE_VALUE (bufp, &objHellosIgnoredValue, sizeof (objHellosIgnoredValue));

  /* return the object value: HellosIgnored */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHellosIgnoredValue,
                           sizeof (objHellosIgnoredValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_TxHellos
*
* @purpose Get 'TxHellos'
 *@description  [TxHellos] The number of Hello packets sent on this interface by
* this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_TxHellos (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTxHellosValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objTxHellosValue = intfStats.TxHellos;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTxHellosValue, sizeof (objTxHellosValue));

  /* return the object value: TxHellos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxHellosValue, sizeof (objTxHellosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_TxDbDescr
*
* @purpose Get 'TxDbDescr'
 *@description  [TxDbDescr] The number of Database Description packets sent on
* this interface by this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_TxDbDescr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTxDbDescrValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objTxDbDescrValue = intfStats.TxDbDescr;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTxDbDescrValue, sizeof (objTxDbDescrValue));

  /* return the object value: TxDbDescr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxDbDescrValue, sizeof (objTxDbDescrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_TxLsReq
*
* @purpose Get 'TxLsReq'
 *@description  [TxLsReq] The number of LS Requests sent on this interface by
* this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_TxLsReq (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTxLsReqValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objTxLsReqValue = intfStats.TxLsReq;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTxLsReqValue, sizeof (objTxLsReqValue));

  /* return the object value: TxLsReq */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxLsReqValue, sizeof (objTxLsReqValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_TxLsUpdate
*
* @purpose Get 'TxLsUpdate'
 *@description  [TxLsUpdate] The number of LS updates sent on this interface by
* this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_TxLsUpdate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTxLsUpdateValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objTxLsUpdateValue = intfStats.TxLsUpdate;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTxLsUpdateValue, sizeof (objTxLsUpdateValue));

  /* return the object value: TxLsUpdate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxLsUpdateValue, sizeof (objTxLsUpdateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingospfInterfaceStats_TxLsAck
*
* @purpose Get 'TxLsAck'
 *@description  [TxLsAck] The number of LS acknowledgements sent on this
* interface by this router.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfInterfaceStats_TxLsAck (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTxLsAckValue;

  xLibU32_t keyInterfaceValue;

  L7_OspfIntfStats_t intfStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfInterfaceStats_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfIntfStatsGet ( keyInterfaceValue, &intfStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    objTxLsAckValue = intfStats.TxLsAck;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTxLsAckValue, sizeof (objTxLsAckValue));

  /* return the object value: TxLsAck */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxLsAckValue, sizeof (objTxLsAckValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
