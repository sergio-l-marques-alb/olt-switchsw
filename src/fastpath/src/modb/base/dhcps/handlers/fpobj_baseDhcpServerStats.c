/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDhcpServerStats.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to snmp-object.xml
*
* @create  9 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseDhcpServerStats_obj.h"
#include "usmdb_dhcps_api.h"

/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_DhcpServerOFFERMessagesSent
*
* @purpose Get 'DhcpServerOFFERMessagesSent'
*
* @description  The no. of DHCP messages of type DHCPOFFER that have been sent 
*              by the DHCP server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerOFFERMessagesSent (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerOFFERMessagesSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsOfferSentGet (L7_UNIT_CURRENT,
                            &objDhcpServerOFFERMessagesSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerOFFERMessagesSentValue,
                     sizeof (objDhcpServerOFFERMessagesSentValue));

  /* return the object value: DhcpServerOFFERMessagesSent */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDhcpServerOFFERMessagesSentValue,
                    sizeof (objDhcpServerOFFERMessagesSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerStats_DhcpServerClearStatistics
*
* @purpose Set 'DhcpServerClearStatistics'
*
* @description  Clears the DHCP server statistics. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerStats_DhcpServerClearStatistics (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerClearStatisticsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DhcpServerClearStatistics */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDhcpServerClearStatisticsValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerClearStatisticsValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDhcpsStatisticsClear (L7_UNIT_CURRENT);
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
* @function fpObjGet_baseDhcpServerStats_DhcpServerExpiredBindingsNos
*
* @purpose Get 'DhcpServerExpiredBindingsNos'
*
* @description  The no. of expired leases. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerExpiredBindingsNos (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerExpiredBindingsNosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsNoOfExpiredLeasesGet (L7_UNIT_CURRENT,
                                    &objDhcpServerExpiredBindingsNosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerExpiredBindingsNosValue,
                     sizeof (objDhcpServerExpiredBindingsNosValue));

  /* return the object value: DhcpServerExpiredBindingsNos */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDhcpServerExpiredBindingsNosValue,
                    sizeof (objDhcpServerExpiredBindingsNosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_DhcpServerDECLINEMessagesReceived
*
* @purpose Get 'DhcpServerDECLINEMessagesReceived'
*
* @description  The no. of DHCP messages of type DHCPDECLINE that have been received 
*              by the DHCP server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerDECLINEMessagesReceived (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerDECLINEMessagesReceivedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsDeclineReceivedGet (L7_UNIT_CURRENT,
                                  &objDhcpServerDECLINEMessagesReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerDECLINEMessagesReceivedValue,
                     sizeof (objDhcpServerDECLINEMessagesReceivedValue));

  /* return the object value: DhcpServerDECLINEMessagesReceived */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDhcpServerDECLINEMessagesReceivedValue,
                    sizeof (objDhcpServerDECLINEMessagesReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_DhcpServerRELEASEMessagesReceived
*
* @purpose Get 'DhcpServerRELEASEMessagesReceived'
*
* @description  The no. of DHCP messages of type DHCPRELEASE that have been received 
*              by the DHCP server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerRELEASEMessagesReceived (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerRELEASEMessagesReceivedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsReleaseReceivedGet (L7_UNIT_CURRENT,
                                  &objDhcpServerRELEASEMessagesReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerRELEASEMessagesReceivedValue,
                     sizeof (objDhcpServerRELEASEMessagesReceivedValue));

  /* return the object value: DhcpServerRELEASEMessagesReceived */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDhcpServerRELEASEMessagesReceivedValue,
                    sizeof (objDhcpServerRELEASEMessagesReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_DhcpServerNAKMessagesSent
*
* @purpose Get 'DhcpServerNAKMessagesSent'
*
* @description  The no. of DHCP messages of type DHCPNAK that have been sent 
*              by the DHCP server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerNAKMessagesSent (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerNAKMessagesSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsNackSentGet (L7_UNIT_CURRENT, &objDhcpServerNAKMessagesSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerNAKMessagesSentValue,
                     sizeof (objDhcpServerNAKMessagesSentValue));

  /* return the object value: DhcpServerNAKMessagesSent */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDhcpServerNAKMessagesSentValue,
                    sizeof (objDhcpServerNAKMessagesSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_DhcpServerDISCOVERMessagesReceived
*
* @purpose Get 'DhcpServerDISCOVERMessagesReceived'
*
* @description  The no. of DHCP messages of type DHCPDISCOVER that have been 
*              received by the DHCP server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerDISCOVERMessagesReceived (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerDISCOVERMessagesReceivedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsDiscoverReceivedGet (L7_UNIT_CURRENT,
                                   &objDhcpServerDISCOVERMessagesReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerDISCOVERMessagesReceivedValue,
                     sizeof (objDhcpServerDISCOVERMessagesReceivedValue));

  /* return the object value: DhcpServerDISCOVERMessagesReceived */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDhcpServerDISCOVERMessagesReceivedValue,
                    sizeof (objDhcpServerDISCOVERMessagesReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_DhcpServerACKMessagesSent
*
* @purpose Get 'DhcpServerACKMessagesSent'
*
* @description  The no. of DHCP messages of type DHCPACK that have been sent 
*              by the DHCP server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerACKMessagesSent (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerACKMessagesSentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsAckSentGet (L7_UNIT_CURRENT, &objDhcpServerACKMessagesSentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerACKMessagesSentValue,
                     sizeof (objDhcpServerACKMessagesSentValue));

  /* return the object value: DhcpServerACKMessagesSent */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDhcpServerACKMessagesSentValue,
                    sizeof (objDhcpServerACKMessagesSentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_DhcpServerINFORMMessagesReceived
*
* @purpose Get 'DhcpServerINFORMMessagesReceived'
*
* @description  The no. of DHCP messages of type DHCPINFORM that have been received 
*              by the DHCP server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerINFORMMessagesReceived (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerINFORMMessagesReceivedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsInformReceivedGet (L7_UNIT_CURRENT,
                                 &objDhcpServerINFORMMessagesReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerINFORMMessagesReceivedValue,
                     sizeof (objDhcpServerINFORMMessagesReceivedValue));

  /* return the object value: DhcpServerINFORMMessagesReceived */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDhcpServerINFORMMessagesReceivedValue,
                    sizeof (objDhcpServerINFORMMessagesReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_DhcpServerMalformedMessagesReceived
*
* @purpose Get 'DhcpServerMalformedMessagesReceived'
*
* @description  The no. of malformed(truncated or corrupt) messages that have 
*              been received by the DHCP server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerMalformedMessagesReceived (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerMalformedMessagesReceivedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsMalformedGet (L7_UNIT_CURRENT,
                            &objDhcpServerMalformedMessagesReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerMalformedMessagesReceivedValue,
                     sizeof (objDhcpServerMalformedMessagesReceivedValue));

  /* return the object value: DhcpServerMalformedMessagesReceived */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDhcpServerMalformedMessagesReceivedValue,
                    sizeof (objDhcpServerMalformedMessagesReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_DhcpServerPingPktNos
*
* @purpose Get 'DhcpServerPingPktNos'
*
* @description  The no. of packets a DHCP Server sends to a pool address as part 
*              of a ping operation.Setting the value of ping-packets to 
*              zero turns off DHCP Server ping operation . 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerPingPktNos (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerPingPktNosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsNoOfPingPktGet (L7_UNIT_CURRENT, &objDhcpServerPingPktNosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerPingPktNosValue,
                     sizeof (objDhcpServerPingPktNosValue));

  /* return the object value: DhcpServerPingPktNos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDhcpServerPingPktNosValue,
                           sizeof (objDhcpServerPingPktNosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerStats_DhcpServerPingPktNos
*
* @purpose Set 'DhcpServerPingPktNos'
*
* @description  The no. of packets a DHCP Server sends to a pool address as part 
*              of a ping operation.Setting the value of ping-packets to 
*              zero turns off DHCP Server ping operation . 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerStats_DhcpServerPingPktNos (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerPingPktNosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DhcpServerPingPktNos */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDhcpServerPingPktNosValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerPingPktNosValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDhcpsNoOfPingPktSet (L7_UNIT_CURRENT, objDhcpServerPingPktNosValue);
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
* @function fpObjGet_baseDhcpServerStats_DhcpServerREQUESTMessagesReceived
*
* @purpose Get 'DhcpServerREQUESTMessagesReceived'
*
* @description  The no. of DHCP messages of type DHCPREQUEST that have been received 
*              by the DHCP server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_DhcpServerREQUESTMessagesReceived (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerREQUESTMessagesReceivedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsRequestReceivedGet (L7_UNIT_CURRENT,
                                  &objDhcpServerREQUESTMessagesReceivedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerREQUESTMessagesReceivedValue,
                     sizeof (objDhcpServerREQUESTMessagesReceivedValue));

  /* return the object value: DhcpServerREQUESTMessagesReceived */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDhcpServerREQUESTMessagesReceivedValue,
                    sizeof (objDhcpServerREQUESTMessagesReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerStats_AutomaticBindingsNos
*
* @purpose Get 'AutomaticBindingsNos'
*
* @description  The no. of IP addresses that have been assigned automatically. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerStats_AutomaticBindingsNos (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutomaticBindingsNosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsNoOfActiveLeasesGet (L7_UNIT_CURRENT,
                                   &objAutomaticBindingsNosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutomaticBindingsNosValue,
                     sizeof (objAutomaticBindingsNosValue));

  /* return the object value: AutomaticBindingsNos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutomaticBindingsNosValue,
                           sizeof (objAutomaticBindingsNosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
