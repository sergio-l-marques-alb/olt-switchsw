
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingswitchHelperStatistics.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  07 November 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingswitchHelperStatistics_obj.h"

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfDhcpClientPktsRx
*
* @purpose Get 'NumOfDhcpClientPktsRx'
 *@description  [NumOfDhcpClientPktsRx] Number of DHCP client packets received
* on the switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfDhcpClientPktsRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfDhcpClientPktsRxValue;
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
  objNumOfDhcpClientPktsRxValue = ihStats.dhcpClientMsgsReceived;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfDhcpClientPktsRxValue, sizeof (objNumOfDhcpClientPktsRxValue));

  /* return the object value: NumOfDhcpClientPktsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfDhcpClientPktsRxValue,
                           sizeof (objNumOfDhcpClientPktsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfDhcpClientPktsRelayed
*
* @purpose Get 'NumOfDhcpClientPktsRelayed'
 *@description  [NumOfDhcpClientPktsRelayed] Number of DHCP client packets
* forwared to the server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfDhcpClientPktsRelayed (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfDhcpClientPktsRelayedValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objNumOfDhcpClientPktsRelayedValue = ihStats.dhcpClientMsgsRelayed;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfDhcpClientPktsRelayedValue,
                     sizeof (objNumOfDhcpClientPktsRelayedValue));

  /* return the object value: NumOfDhcpClientPktsRelayed */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfDhcpClientPktsRelayedValue,
                           sizeof (objNumOfDhcpClientPktsRelayedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfDhcpServerPktsRx
*
* @purpose Get 'NumOfDhcpServerPktsRx'
 *@description  [NumOfDhcpServerPktsRx] Number of DHCP client received on the
* switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfDhcpServerPktsRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfDhcpServerPktsRxValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objNumOfDhcpServerPktsRxValue = ihStats.dhcpServerMsgsReceived;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfDhcpServerPktsRxValue, sizeof (objNumOfDhcpServerPktsRxValue));

  /* return the object value: NumOfDhcpServerPktsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfDhcpServerPktsRxValue,
                           sizeof (objNumOfDhcpServerPktsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfDhcpServerPktsRelayed
*
* @purpose Get 'NumOfDhcpServerPktsRelayed'
 *@description  [NumOfDhcpServerPktsRelayed] Number of DHCP server packets
* forwared to the server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfDhcpServerPktsRelayed (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfDhcpServerPktsRelayedValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objNumOfDhcpServerPktsRelayedValue = ihStats.dhcpServerMsgsRelayed;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfDhcpServerPktsRelayedValue,
                     sizeof (objNumOfDhcpServerPktsRelayedValue));

  /* return the object value: NumOfDhcpServerPktsRelayed */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfDhcpServerPktsRelayedValue,
                           sizeof (objNumOfDhcpServerPktsRelayedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfUdpClientPktsRx
*
* @purpose Get 'NumOfUdpClientPktsRx'
 *@description  [NumOfUdpClientPktsRx] Number of UDP client packets received on
* the switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfUdpClientPktsRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfUdpClientPktsRxValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objNumOfUdpClientPktsRxValue = ihStats.udpClientMsgsReceived;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfUdpClientPktsRxValue, sizeof (objNumOfUdpClientPktsRxValue));

  /* return the object value: NumOfUdpClientPktsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfUdpClientPktsRxValue,
                           sizeof (objNumOfUdpClientPktsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfUdpClientPktsRelayed
*
* @purpose Get 'NumOfUdpClientPktsRelayed'
 *@description  [NumOfUdpClientPktsRelayed] Number of UDP client packets
* forwarded to the server.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfUdpClientPktsRelayed (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfUdpClientPktsRelayedValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objNumOfUdpClientPktsRelayedValue = ihStats.udpClientMsgsRelayed;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfUdpClientPktsRelayedValue,
                     sizeof (objNumOfUdpClientPktsRelayedValue));

  /* return the object value: NumOfUdpClientPktsRelayed */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfUdpClientPktsRelayedValue,
                           sizeof (objNumOfUdpClientPktsRelayedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfDhcpMaxHopCountPktsRx
*
* @purpose Get 'NumOfDhcpMaxHopCountPktsRx'
 *@description  [NumOfDhcpMaxHopCountPktsRx] Number of DHCP packets received
* with maximum hop count on the switch.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfDhcpMaxHopCountPktsRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfDhcpMaxHopCountPktsRxValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objNumOfDhcpMaxHopCountPktsRxValue = ihStats.tooManyHops;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfDhcpMaxHopCountPktsRxValue,
                     sizeof (objNumOfDhcpMaxHopCountPktsRxValue));

  /* return the object value: NumOfDhcpMaxHopCountPktsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfDhcpMaxHopCountPktsRxValue,
                           sizeof (objNumOfDhcpMaxHopCountPktsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfDhcpTooEarlyPktsRx
*
* @purpose Get 'NumOfDhcpTooEarlyPktsRx'
 *@description  [NumOfDhcpTooEarlyPktsRx] Number of DHCP packets received with
* secs field set to value less than minimum allowed value.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfDhcpTooEarlyPktsRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfDhcpTooEarlyPktsRxValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  objNumOfDhcpTooEarlyPktsRxValue = ihStats.tooEarly;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfDhcpTooEarlyPktsRxValue,
                     sizeof (objNumOfDhcpTooEarlyPktsRxValue));

  /* return the object value: NumOfDhcpTooEarlyPktsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfDhcpTooEarlyPktsRxValue,
                           sizeof (objNumOfDhcpTooEarlyPktsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfDhcpSpoofedGiaddrPktsRx
*
* @purpose Get 'NumOfDhcpSpoofedGiaddrPktsRx'
 *@description  [NumOfDhcpSpoofedGiaddrPktsRx] Number of DHCP client messages
* received with the giaddr set to local address.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfDhcpSpoofedGiaddrPktsRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfDhcpSpoofedGiaddrPktsRxValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objNumOfDhcpSpoofedGiaddrPktsRxValue = ihStats.spoofedGiaddr;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfDhcpSpoofedGiaddrPktsRxValue,
                     sizeof (objNumOfDhcpSpoofedGiaddrPktsRxValue));

  /* return the object value: NumOfDhcpSpoofedGiaddrPktsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfDhcpSpoofedGiaddrPktsRxValue,
                           sizeof (objNumOfDhcpSpoofedGiaddrPktsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfUdpTtlExpiredPktsRx
*
* @purpose Get 'NumOfUdpTtlExpiredPktsRx'
 *@description  [NumOfUdpTtlExpiredPktsRx] Number of UDP packets received with
* TTL value less than or equal to one.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfUdpTtlExpiredPktsRx (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfUdpTtlExpiredPktsRxValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objNumOfUdpTtlExpiredPktsRxValue = ihStats.ttlExpired;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfUdpTtlExpiredPktsRxValue,
                     sizeof (objNumOfUdpTtlExpiredPktsRxValue));

  /* return the object value: NumOfUdpTtlExpiredPktsRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfUdpTtlExpiredPktsRxValue,
                           sizeof (objNumOfUdpTtlExpiredPktsRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperStatistics_NumOfUdpDisardPkts
*
* @purpose Get 'NumOfUdpDisardPkts'
 *@description  [NumOfUdpDisardPkts] Number of UDP packets dropped due to match
* against a discard entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperStatistics_NumOfUdpDisardPkts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNumOfUdpDisardPktsValue;
  ipHelperStats_t ihStats;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIpHelperStatisticsGet (&ihStats);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objNumOfUdpDisardPktsValue = ihStats.matchDiscardEntry;
  FPOBJ_TRACE_VALUE (bufp, &objNumOfUdpDisardPktsValue, sizeof (objNumOfUdpDisardPktsValue));

  /* return the object value: NumOfUdpDisardPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNumOfUdpDisardPktsValue,
                           sizeof (objNumOfUdpDisardPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchHelperStatistics_ResetHelperStats
*
* @purpose Set 'ResetHelperStats'
 *@description  [ResetHelperStats] Clears the IP Helper Statistics.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchHelperStatistics_ResetHelperStats (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objResetHelperStatsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ResetHelperStats */
  owa.len = sizeof (objResetHelperStatsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objResetHelperStatsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objResetHelperStatsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIpHelperStatisticsClear();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
