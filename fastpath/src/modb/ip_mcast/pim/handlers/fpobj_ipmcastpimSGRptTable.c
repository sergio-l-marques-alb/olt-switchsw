
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimSGRptTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimSGRptTable_obj.h"
#include "usmdb_mib_pimsm_api.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptTable_pimStarGAddressType
*
* @purpose Get 'pimStarGAddressType'
*
* @description [pimStarGAddressType] The address type of this multicast group.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptTable_pimStarGAddressType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGAddressTypeValue;
  xLibU32_t nextObjpimStarGAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGAddressType,
                          (xLibU8_t *) & objpimStarGAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjpimStarGAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStarGAddressTypeValue, owa.len);

    if (objpimStarGAddressTypeValue == L7_INET_ADDR_TYPE_IPV6)
    {
      owa.l7rc = L7_FAILURE;
    }
    else
    {
      nextObjpimStarGAddressTypeValue = objpimStarGAddressTypeValue + 1;
      owa.l7rc = L7_SUCCESS;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStarGAddressTypeValue, owa.len);

  /* return the object value: pimStarGAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimStarGAddressTypeValue,
                           sizeof (xLibU32_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptTable_pimStarGGrpAddress
*
* @purpose Get 'pimStarGGrpAddress'
*
* @description [pimStarGGrpAddress] The multicast group address. The InetAddressType is given by the pimSmStarGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptTable_pimStarGGrpAddress (void *wap, void *bufp)
{
  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimStarGGrpAddressValue;
  L7_inet_addr_t nextObjpimStarGGrpAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGGrpAddress,
                          (xLibU8_t *) &objpimStarGGrpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(&nextObjpimStarGGrpAddressValue, 0, sizeof(L7_inet_addr_t));
    
    owa.l7rc = usmDbPimsmStarGEntryNextGet (keypimStarGAddressTypeValue,
                                                                     &nextObjpimStarGGrpAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimStarGGrpAddressValue, owa.len);

    memcpy(&nextObjpimStarGGrpAddressValue, &objpimStarGGrpAddressValue, sizeof(L7_inet_addr_t));
    
    owa.l7rc = usmDbPimsmStarGEntryNextGet (keypimStarGAddressTypeValue,
                                                                     &nextObjpimStarGGrpAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimStarGGrpAddressValue, owa.len);

  /* return the object value: pimStarGGrpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimStarGGrpAddressValue,
                           sizeof (objpimStarGGrpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptTable_pimSGRptSrcAddress
*
* @purpose Get 'pimSGRptSrcAddress'
*
* @description [pimSGRptSrcAddress] The source address for this entry. The InetAddressType is given by the pimSmStarGAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptTable_pimSGRptSrcAddress (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objpimSGRptSrcAddressValue;
  L7_inet_addr_t nextObjpimSGRptSrcAddressValue;

  xLibU32_t ifindexg;  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimSGRptSrcAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimSGRptSrcAddress,
                          (xLibU8_t *) &objpimSGRptSrcAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    memset(&nextObjpimSGRptSrcAddressValue, 0, sizeof(L7_inet_addr_t));
    owa.l7rc = usmDbPimsmSGRptIEntryNextGet (keypimStarGAddressTypeValue,
                                     &keypimStarGGrpAddressValue, &nextObjpimSGRptSrcAddressValue,
                                     &ifindexg);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimSGRptSrcAddressValue, owa.len);

    memcpy(&nextObjpimSGRptSrcAddressValue, &objpimSGRptSrcAddressValue, sizeof(L7_inet_addr_t));
    
    owa.l7rc = usmDbPimsmSGRptIEntryNextGet (keypimStarGAddressTypeValue,
                                     &keypimStarGGrpAddressValue, &nextObjpimSGRptSrcAddressValue,
                                     &ifindexg);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimSGRptSrcAddressValue, owa.len);

  /* return the object value: pimSGRptSrcAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjpimSGRptSrcAddressValue,
                           sizeof (objpimSGRptSrcAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptTable_pimSGRptUpTime
*
* @purpose Get 'pimSGRptUpTime'
*
* @description [pimSGRptUpTime] The time since this entry was created by the local router.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptTable_pimSGRptUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;
  fpObjWa_t kwapimSGRptSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGRptSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptUpTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimSGRptSrcAddress */
  kwapimSGRptSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimSGRptSrcAddress,
                                            (xLibU8_t *) &keypimSGRptSrcAddressValue,
                                            &kwapimSGRptSrcAddress.len);
  if (kwapimSGRptSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptSrcAddress);
    return kwapimSGRptSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptSrcAddressValue, kwapimSGRptSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGRptUpTimeGet (keypimStarGAddressTypeValue,
                              &keypimStarGGrpAddressValue,
                              &keypimSGRptSrcAddressValue, &objpimSGRptUpTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRptUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptUpTimeValue,
                           sizeof (objpimSGRptUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptTable_pimSGRptUpstreamPruneState
*
* @purpose Get 'pimSGRptUpstreamPruneState'
*
* @description [pimSGRptUpstreamPruneState] Whether the local router should prune the source off the RPtree. This corresponds to the state of the upstream(S,G,rpt) state machine for triggered messages in the PIM-SMspecification [I-D.ietf-pim-sm-v2-new].
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptTable_pimSGRptUpstreamPruneState (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;
  fpObjWa_t kwapimSGRptSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGRptSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptUpstreamPruneStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimSGRptSrcAddress */
  kwapimSGRptSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimSGRptSrcAddress,
                                            (xLibU8_t *) &keypimSGRptSrcAddressValue,
                                            &kwapimSGRptSrcAddress.len);
  if (kwapimSGRptSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptSrcAddress);
    return kwapimSGRptSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptSrcAddressValue, kwapimSGRptSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGRptUpstreamPruneStateGet (keypimStarGAddressTypeValue,
                              &keypimStarGGrpAddressValue,
                              &keypimSGRptSrcAddressValue, &objpimSGRptUpstreamPruneStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRptUpstreamPruneState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptUpstreamPruneStateValue,
                           sizeof (objpimSGRptUpstreamPruneStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_ipmcastpimSGRptTable_pimSGRptUpstreamOverrideTimer
*
* @purpose Get 'pimSGRptUpstreamOverrideTimer'
*
* @description [pimSGRptUpstreamOverrideTimer] The time remaining before the local router sends atriggered (S,G,rpt) Join message on pimSmStarGRPFIfIndex.This timer is called the (S,G,rpt) Upstream Override Timerin the PIM-SM specification [I-D.ietf-pim-sm-v2-new]. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimSGRptTable_pimSGRptUpstreamOverrideTimer (void *wap, void *bufp)
{

  fpObjWa_t kwapimStarGAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keypimStarGAddressTypeValue;
  fpObjWa_t kwapimStarGGrpAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimStarGGrpAddressValue;
  fpObjWa_t kwapimSGRptSrcAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keypimSGRptSrcAddressValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptUpstreamOverrideTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimStarGAddressType */
  kwapimStarGAddressType.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGAddressType,
                                             (xLibU8_t *) & keypimStarGAddressTypeValue,
                                             &kwapimStarGAddressType.len);
  if (kwapimStarGAddressType.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGAddressType);
    return kwapimStarGAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGAddressTypeValue, kwapimStarGAddressType.len);

  /* retrieve key: pimStarGGrpAddress */
  kwapimStarGGrpAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimStarGGrpAddress,
                                            (xLibU8_t *) &keypimStarGGrpAddressValue,
                                            &kwapimStarGGrpAddress.len);
  if (kwapimStarGGrpAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimStarGGrpAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimStarGGrpAddress);
    return kwapimStarGGrpAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimStarGGrpAddressValue, kwapimStarGGrpAddress.len);

  /* retrieve key: pimSGRptSrcAddress */
  kwapimSGRptSrcAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastpimSGRptTable_pimSGRptSrcAddress,
                                            (xLibU8_t *) &keypimSGRptSrcAddressValue,
                                            &kwapimSGRptSrcAddress.len);
  if (kwapimSGRptSrcAddress.rc != XLIBRC_SUCCESS)
  {
    kwapimSGRptSrcAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwapimSGRptSrcAddress);
    return kwapimSGRptSrcAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keypimSGRptSrcAddressValue, kwapimSGRptSrcAddress.len);

  /* get the value from application */
  owa.l7rc = usmDbPimsmSGRptUpstreamOverrideTimerGet (keypimStarGAddressTypeValue,
                              &keypimStarGGrpAddressValue,
                              &keypimSGRptSrcAddressValue, &objpimSGRptUpstreamOverrideTimerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: pimSGRptUpstreamOverrideTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptUpstreamOverrideTimerValue,
                           sizeof (objpimSGRptUpstreamOverrideTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
