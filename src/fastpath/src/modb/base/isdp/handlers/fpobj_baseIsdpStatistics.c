/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseIsdpStatistics.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  05 June 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseIsdpStatistics_obj.h"
#include "usmdb_isdp_api.h"

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsPduReceived
*
* @purpose Get 'IsdpStatisticsPduReceived'
 *@description  [IsdpStatisticsPduReceived] Display the number of all ISDP pdu
* received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsPduReceived (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsPduReceivedValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsPduReceivedValue = usmdbIsdpTrafficPduReceivedGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsPduReceivedValue,
                     sizeof (objIsdpStatisticsPduReceivedValue));

  /* return the object value: IsdpStatisticsPduReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsPduReceivedValue,
                           sizeof (objIsdpStatisticsPduReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsPduTransmit
*
* @purpose Get 'IsdpStatisticsPduTransmit'
 *@description  [IsdpStatisticsPduTransmit] Display the number of all ISDP pdu
* transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsPduTransmit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsPduTransmitValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsPduTransmitValue = usmdbIsdpTrafficPduTransmitGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsPduTransmitValue,
                     sizeof (objIsdpStatisticsPduTransmitValue));

  /* return the object value: IsdpStatisticsPduTransmit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsPduTransmitValue,
                           sizeof (objIsdpStatisticsPduTransmitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsV1PduReceived
*
* @purpose Get 'IsdpStatisticsV1PduReceived'
 *@description  [IsdpStatisticsV1PduReceived] Display the number of v1 ISDP pdu
* transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsV1PduReceived (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsV1PduReceivedValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsV1PduReceivedValue = usmdbIsdpTrafficV1PduReceivedGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsV1PduReceivedValue,
                     sizeof (objIsdpStatisticsV1PduReceivedValue));

  /* return the object value: IsdpStatisticsV1PduReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsV1PduReceivedValue,
                           sizeof (objIsdpStatisticsV1PduReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsV1PduTransmit
*
* @purpose Get 'IsdpStatisticsV1PduTransmit'
 *@description  [IsdpStatisticsV1PduTransmit] Display the number of v1 ISDP pdu
* transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsV1PduTransmit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsV1PduTransmitValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsV1PduTransmitValue = usmdbIsdpTrafficV1PduTransmitGet();


  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsV1PduTransmitValue,
                     sizeof (objIsdpStatisticsV1PduTransmitValue));

  /* return the object value: IsdpStatisticsV1PduTransmit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsV1PduTransmitValue,
                           sizeof (objIsdpStatisticsV1PduTransmitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsV2PduReceived
*
* @purpose Get 'IsdpStatisticsV2PduReceived'
 *@description  [IsdpStatisticsV2PduReceived] Display the number of v2 ISDP pdu
* received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsV2PduReceived (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsV2PduReceivedValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsV2PduReceivedValue = usmdbIsdpTrafficV2PduReceivedGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsV2PduReceivedValue,
                     sizeof (objIsdpStatisticsV2PduReceivedValue));

  /* return the object value: IsdpStatisticsV2PduReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsV2PduReceivedValue,
                           sizeof (objIsdpStatisticsV2PduReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsV2PduTransmit
*
* @purpose Get 'IsdpStatisticsV2PduTransmit'
 *@description  [IsdpStatisticsV2PduTransmit] Display the number of v2 ISDP pdu
* transmitted.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsV2PduTransmit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsV2PduTransmitValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsV2PduTransmitValue = usmdbIsdpTrafficV2PduTransmitGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsV2PduTransmitValue,
                     sizeof (objIsdpStatisticsV2PduTransmitValue));

  /* return the object value: IsdpStatisticsV2PduTransmit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsV2PduTransmitValue,
                           sizeof (objIsdpStatisticsV2PduTransmitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsBadHeaderPduReceived
*
* @purpose Get 'IsdpStatisticsBadHeaderPduReceived'
 *@description  [IsdpStatisticsBadHeaderPduReceived] Display the number of ISDP
* pdu with bad header received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsBadHeaderPduReceived (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsBadHeaderPduReceivedValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsBadHeaderPduReceivedValue = usmdbIsdpTrafficBadHeaderPduReceivedGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsBadHeaderPduReceivedValue,
                     sizeof (objIsdpStatisticsBadHeaderPduReceivedValue));

  /* return the object value: IsdpStatisticsBadHeaderPduReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsBadHeaderPduReceivedValue,
                           sizeof (objIsdpStatisticsBadHeaderPduReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsChkSumErrorPduReceived
*
* @purpose Get 'IsdpStatisticsChkSumErrorPduReceived'
 *@description  [IsdpStatisticsChkSumErrorPduReceived] Display the number of
* ISDP pdu with chksum error received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsChkSumErrorPduReceived (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsChkSumErrorPduReceivedValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsChkSumErrorPduReceivedValue = usmdbIsdpTrafficChkSumErrorPduReceivedGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsChkSumErrorPduReceivedValue,
                     sizeof (objIsdpStatisticsChkSumErrorPduReceivedValue));

  /* return the object value: IsdpStatisticsChkSumErrorPduReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsChkSumErrorPduReceivedValue,
                           sizeof (objIsdpStatisticsChkSumErrorPduReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsFailurePduTransmit
*
* @purpose Get 'IsdpStatisticsFailurePduTransmit'
 *@description  [IsdpStatisticsFailurePduTransmit] Display the number of ISDP
* pdu transmition failures.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsFailurePduTransmit (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsFailurePduTransmitValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsFailurePduTransmitValue = usmdbIsdpTrafficFailurePduTransmitGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsFailurePduTransmitValue,
                     sizeof (objIsdpStatisticsFailurePduTransmitValue));

  /* return the object value: IsdpStatisticsFailurePduTransmit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsFailurePduTransmitValue,
                           sizeof (objIsdpStatisticsFailurePduTransmitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsInvalidFormatPduReceived
*
* @purpose Get 'IsdpStatisticsInvalidFormatPduReceived'
 *@description  [IsdpStatisticsInvalidFormatPduReceived] Display the number of
* ISDP pdu in invalid format received.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsInvalidFormatPduReceived (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsInvalidFormatPduReceivedValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsInvalidFormatPduReceivedValue = usmdbIsdpTrafficInvalidFormatPduReceivedGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsInvalidFormatPduReceivedValue,
                     sizeof (objIsdpStatisticsInvalidFormatPduReceivedValue));

  /* return the object value: IsdpStatisticsInvalidFormatPduReceived */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsInvalidFormatPduReceivedValue,
                           sizeof (objIsdpStatisticsInvalidFormatPduReceivedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsTableFull
*
* @purpose Get 'IsdpStatisticsTableFull'
 *@description  [IsdpStatisticsTableFull] Display the number of times ISDP entry
* table was full.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsTableFull (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsTableFullValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsTableFullValue = usmdbIsdpTrafficTableFullGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsTableFullValue,
                     sizeof (objIsdpStatisticsTableFullValue));

  /* return the object value: IsdpStatisticsTableFull */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsTableFullValue,
                           sizeof (objIsdpStatisticsTableFullValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpStatistics_IsdpStatisticsIpAddressTableFull
*
* @purpose Get 'IsdpStatisticsIpAddressTableFull'
 *@description  [IsdpStatisticsIpAddressTableFull] Display the number of times
* ISDP entry address table was full.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpStatistics_IsdpStatisticsIpAddressTableFull (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpStatisticsIpAddressTableFullValue;
  xLibU32_t mode;

  FPOBJ_TRACE_ENTER (bufp);

  if ((usmdbIsdpModeGet(&mode) != L7_SUCCESS) || (mode != L7_ENABLE))
  {
    owa.rc = XLIBRC_ISDP_NOT_ENABLED;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  objIsdpStatisticsIpAddressTableFullValue = usmdbIsdpTrafficIpAddressTableFullGet();

  FPOBJ_TRACE_VALUE (bufp, &objIsdpStatisticsIpAddressTableFullValue,
                     sizeof (objIsdpStatisticsIpAddressTableFullValue));

  /* return the object value: IsdpStatisticsIpAddressTableFull */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIsdpStatisticsIpAddressTableFullValue,
                           sizeof (objIsdpStatisticsIpAddressTableFullValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
