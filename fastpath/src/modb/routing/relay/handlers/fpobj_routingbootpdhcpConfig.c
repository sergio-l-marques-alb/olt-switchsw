/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingbootpdhcpConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to bootpdhcprelay-object.xml
*
* @create  13 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingbootpdhcpConfig_obj.h"
#include "usmdb_mib_relay_api.h"

/*******************************************************************************
* @function fpObjGet_routingbootpdhcpConfig_MaxHopCount
*
* @purpose Get 'MaxHopCount'
*
* @description [MaxHopCount]: The BOOTP/DHCP Max Hop Count. The relay agent
*              silently discards BOOTREQUEST messages whose hops field exceeds
*              the value 16 assuming that the request is looped through
*              the intermediate agents. The default value is 4. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingbootpdhcpConfig_MaxHopCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxHopCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbBootpDhcpRelayMaxHopCountGet (L7_UNIT_CURRENT, &objMaxHopCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxHopCountValue, sizeof (objMaxHopCountValue));

  /* return the object value: MaxHopCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxHopCountValue,
                           sizeof (objMaxHopCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingbootpdhcpConfig_MaxHopCount
*
* @purpose Set 'MaxHopCount'
*
* @description [MaxHopCount]: The BOOTP/DHCP Max Hop Count. The relay agent
*              silently discards BOOTREQUEST messages whose hops field exceeds
*              the value 16 assuming that the request is looped through
*              the intermediate agents. The default value is 4. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingbootpdhcpConfig_MaxHopCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxHopCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MaxHopCount */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMaxHopCountValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxHopCountValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbBootpDhcpRelayMaxHopCountSet (L7_UNIT_CURRENT, objMaxHopCountValue);
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
* @function fpObjGet_routingbootpdhcpConfig_ForwardingIp
*
* @purpose Get 'ForwardingIp'
*
* @description [ForwardingIp]: The BOOTP/DHCP Server IP Address. All the requests
*              will be forwarded and the replies expected from this
*              address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingbootpdhcpConfig_ForwardingIp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objForwardingIpValue;
  FPOBJ_TRACE_ENTER (bufp);

  objForwardingIpValue = 0;

  /* get the value from application */
  /* owa.l7rc =
    usmDbIpHelperAddressGet (IH_INTF_ANY, UDP_PORT_BOOTP_SERV, &objForwardingIpValue ,discard, &hitCount);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objForwardingIpValue,
                     sizeof (objForwardingIpValue));

  /* return the object value: ForwardingIp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objForwardingIpValue,
                           sizeof (objForwardingIpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingbootpdhcpConfig_ForwardingIp
*
* @purpose Set 'ForwardingIp'
*
* @description [ForwardingIp]: The BOOTP/DHCP Server IP Address. All the requests
*              will be forwarded and the replies expected from this
*              address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingbootpdhcpConfig_ForwardingIp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objForwardingIpValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ForwardingIp */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objForwardingIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objForwardingIpValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIpHelperAddressAdd(IH_INTF_ANY, UDP_PORT_BOOTP_SERV, objForwardingIpValue);
  
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
* @function fpObjGet_routingbootpdhcpConfig_ForwardMode
*
* @purpose Get 'ForwardMode'
*
* @description [ForwardMode]: The BOOTP/DHCP Admin Mode.The relaying functionality
*              can be administratively enabled or disabled by this
*              object 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingbootpdhcpConfig_ForwardMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objForwardModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbIpHelperAdminModeGet ( &objForwardModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objForwardModeValue, sizeof (objForwardModeValue));

  /* return the object value: ForwardMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objForwardModeValue,
                           sizeof (objForwardModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingbootpdhcpConfig_ForwardMode
*
* @purpose Set 'ForwardMode'
*
* @description [ForwardMode]: The BOOTP/DHCP Admin Mode.The relaying functionality
*              can be administratively enabled or disabled by this
*              object 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingbootpdhcpConfig_ForwardMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objForwardModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ForwardMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objForwardModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objForwardModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbIpHelperAdminModeSet ( objForwardModeValue);
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
* @function fpObjGet_routingbootpdhcpConfig_MinWaitTime
*
* @purpose Get 'MinWaitTime'
*
* @description [MinWaitTime]: The BOOTP/DHCP Minimum Wait Time. When the BOOTP
*              relay agent receives a BOOTREQUEST message, it MAY use
*              the value of the 'secs' (seconds since client began booting)
*              field of the request as a factor in deciding whether to
*              relay the request or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingbootpdhcpConfig_MinWaitTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMinWaitTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbBootpDhcpRelayMinWaitTimeGet (L7_UNIT_CURRENT, &objMinWaitTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMinWaitTimeValue, sizeof (objMinWaitTimeValue));

  /* return the object value: MinWaitTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMinWaitTimeValue,
                           sizeof (objMinWaitTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingbootpdhcpConfig_MinWaitTime
*
* @purpose Set 'MinWaitTime'
*
* @description [MinWaitTime]: The BOOTP/DHCP Minimum Wait Time. When the BOOTP
*              relay agent receives a BOOTREQUEST message, it MAY use
*              the value of the 'secs' (seconds since client began booting)
*              field of the request as a factor in deciding whether to
*              relay the request or not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingbootpdhcpConfig_MinWaitTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMinWaitTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MinWaitTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMinWaitTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMinWaitTimeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbBootpDhcpRelayMinWaitTimeSet (L7_UNIT_CURRENT, objMinWaitTimeValue);
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
* @function fpObjGet_routingbootpdhcpConfig_CircuitIdOptionMode
*
* @purpose Get 'CircuitIdOptionMode'
*
* @description [CircuitIdOptionMode]: The BOOTP/DHCP Circuit ID Option Mode.
*              This flag is set to enable/disable the network element to
*              add/remove the DHCP Relay agent Circuit ID sub-options. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingbootpdhcpConfig_CircuitIdOptionMode (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCircuitIdOptionModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbBootpDhcpRelayCircuitIdOptionModeGet (L7_UNIT_CURRENT,
                                               &objCircuitIdOptionModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCircuitIdOptionModeValue,
                     sizeof (objCircuitIdOptionModeValue));

  /* return the object value: CircuitIdOptionMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCircuitIdOptionModeValue,
                           sizeof (objCircuitIdOptionModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingbootpdhcpConfig_CircuitIdOptionMode
*
* @purpose Set 'CircuitIdOptionMode'
*
* @description [CircuitIdOptionMode]: The BOOTP/DHCP Circuit ID Option Mode.
*              This flag is set to enable/disable the network element to
*              add/remove the DHCP Relay agent Circuit ID sub-options. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingbootpdhcpConfig_CircuitIdOptionMode (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCircuitIdOptionModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: CircuitIdOptionMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objCircuitIdOptionModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCircuitIdOptionModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbBootpDhcpRelayCircuitIdOptionModeSet (L7_UNIT_CURRENT,
                                               objCircuitIdOptionModeValue);
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
* @function fpObjGet_routingbootpdhcpConfig_NumOfRequestsReceived
*
* @purpose Get 'NumOfRequestsReceived'
*
* @description [NumOfRequestsReceived]: The Number of BOOTP/DHCP Requests
*              Received. Total number of BOOTREQUESTs received from all clients.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingbootpdhcpConfig_NumOfRequestsReceived (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNumOfRequestsReceivedValue;
  ipHelperStats_t ihStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet(&ihStats); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objNumOfRequestsReceivedValue = ihStats.udpClientMsgsReceived;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfRequestsReceivedValue,
                     sizeof (objNumOfRequestsReceivedValue));

  /* return the object value: NumOfRequestsReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfRequestsReceivedValue,
                           sizeof (objNumOfRequestsReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingbootpdhcpConfig_NumOfRequestsForwarded
*
* @purpose Get 'NumOfRequestsForwarded'
*
* @description [NumOfRequestsForwarded]: The Number of BOOTP/DHCP Requests
*              Forwarded. Total number of BOOTREQUESTs forwarded to the next
*              agents/servers. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingbootpdhcpConfig_NumOfRequestsForwarded (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNumOfRequestsForwardedValue;
  ipHelperStats_t ihStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet(&ihStats); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objNumOfRequestsForwardedValue = ihStats.udpClientMsgsRelayed;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfRequestsForwardedValue,
                     sizeof (objNumOfRequestsForwardedValue));

  /* return the object value: NumOfRequestsForwarded */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfRequestsForwardedValue,
                           sizeof (objNumOfRequestsForwardedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingbootpdhcpConfig_NumOfDiscards
*
* @purpose Get 'NumOfDiscards'
*
* @description [NumOfDiscards]: The Number of BOOTP/DHCP Discards.Total number
*              of packets discarded by the network element because of
*              error in the packet. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingbootpdhcpConfig_NumOfDiscards (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNumOfDiscardsValue;
  ipHelperStats_t ihStats;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet(&ihStats); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNumOfDiscardsValue,
                     sizeof (objNumOfDiscardsValue));

  objNumOfDiscardsValue = ihStats.matchDiscardEntry;
  /* return the object value: NumOfDiscards */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfDiscardsValue,
                           sizeof (objNumOfDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
