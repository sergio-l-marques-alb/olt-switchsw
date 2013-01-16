/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPStats.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to LLDP-object.xml
*
* @create  6 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingLLDPStats_obj.h"
#include "usmdb_lldp_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_TxPortNum
*
* @purpose Get 'TxPortNum'
*
* @description [TxPortNum]: Transmit Port 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_TxPortNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxPortNumValue;
  xLibU32_t nextObjTxPortNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TxPortNum */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_TxPortNum,
                          (xLibU8_t *) & objTxPortNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    objTxPortNumValue = 0;
    owa.l7rc =  usmDbValidIntIfNumFirstGet(&nextObjTxPortNumValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTxPortNumValue, owa.len);
    owa.l7rc =
      usmDbValidIntIfNumNext(objTxPortNumValue,
                             &nextObjTxPortNumValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjTxPortNumValue, owa.len);
  /* return the object value: TxPortNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjTxPortNumValue,
                           sizeof (objTxPortNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_TxPortFramesTotal
*
* @purpose Get 'TxPortFramesTotal'
*
* @description [TxPortFramesTotal]: The number of LLDP frames transmitted
*              by this LLDP agent on the indicated port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_TxPortFramesTotal (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxPortFramesTotalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_TxPortNum,
                          (xLibU8_t *) & keyTxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsTxPortFramesTotalGet ( keyTxPortNumValue,
                                        &objTxPortFramesTotalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TxPortFramesTotal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxPortFramesTotalValue,
                           sizeof (objTxPortFramesTotalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_RxPortNum
*
* @purpose Get 'RxPortNum'
*
* @description [RxPortNum]: Receive Port 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_RxPortNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxPortNumValue;
  xLibU32_t nextObjRxPortNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & objRxPortNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    objRxPortNumValue = 0;
    owa.l7rc = usmDbValidIntIfNumFirstGet ( &nextObjRxPortNumValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRxPortNumValue, owa.len);
    owa.l7rc =
      usmDbValidIntIfNumNext(objRxPortNumValue,
                              &nextObjRxPortNumValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRxPortNumValue, owa.len);

  /* return the object value: RxPortNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRxPortNumValue,
                           sizeof (objRxPortNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_LldpEnabledPortNum
*
* @purpose Get 'LldpEnabledPortNum'
*
* @description [LldpEnabledPortNum]: Lldp Enabled Port
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_LldpEnabledPortNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEnabledPortNumValue;
  xLibU32_t nextObjEnabledPortNumValue;
  L7_uint32   rxMode         = L7_DISABLE;
  L7_uint32   txMode         = L7_DISABLE;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_LldpEnabledPortNum,
                          (xLibU8_t *) & objEnabledPortNumValue, &owa.len);
  nextObjEnabledPortNumValue = objEnabledPortNumValue;
  if (owa.rc != XLIBRC_SUCCESS) 
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objEnabledPortNumValue , objEnabledPortNumValue);
    owa.l7rc = usmDbValidIntIfNumFirstGet(&objEnabledPortNumValue);
    do
    {
      kwa1.l7rc = usmDbLldpIntfRxModeGet(objEnabledPortNumValue, &rxMode);
      kwa2.l7rc = usmDbLldpIntfTxModeGet(objEnabledPortNumValue, &txMode);

      if (kwa1.l7rc == L7_SUCCESS || kwa2.l7rc == L7_SUCCESS)
      {
        /* return the object value: LldpEnabledPortNum */
        if (rxMode == L7_ENABLE || txMode == L7_ENABLE)
        {
          break;
        }
      }
      owa.l7rc = usmDbValidIntIfNumNext(objEnabledPortNumValue, &objEnabledPortNumValue);
    }
    while (owa.l7rc == L7_SUCCESS);
  }
  else 
  {
    do
    {
        owa.l7rc = usmDbValidIntIfNumNext(nextObjEnabledPortNumValue, &nextObjEnabledPortNumValue);
        kwa1.l7rc = usmDbLldpIntfRxModeGet(nextObjEnabledPortNumValue, &rxMode);
        kwa2.l7rc = usmDbLldpIntfTxModeGet(nextObjEnabledPortNumValue, &txMode);
        objEnabledPortNumValue = nextObjEnabledPortNumValue;
        FPOBJ_TRACE_CURRENT_KEY (bufp, &objEnabledPortNumValue , objEnabledPortNumValue);
 
        if (kwa1.l7rc == L7_SUCCESS || kwa2.l7rc == L7_SUCCESS)
        {
          /* return the object value: LldpEnabledPortNum */
          if (rxMode == L7_ENABLE || txMode == L7_ENABLE)
          {
            break;
          }
        }
    }
    while (owa.l7rc == L7_SUCCESS);
  }  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEnabledPortNumValue,
                          sizeof (objEnabledPortNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_RxPortFramesDiscardedTotal
*
* @purpose Get 'RxPortFramesDiscardedTotal'
*
* @description [RxPortFramesDiscardedTotal]: The number of LLDP frames received
*              by this LLDP agent on the indicated port, and then discarded
*              for any reason. This counter can provide an indication
*              that LLDP header formating problems may exist with the
*              local LLDP agent in the sending system or that LL 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_RxPortFramesDiscardedTotal (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxPortFramesDiscardedTotalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & keyRxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRxPortFramesDiscardedGet (keyRxPortNumValue,
                                            &objRxPortFramesDiscardedTotalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxPortFramesDiscardedTotal */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objRxPortFramesDiscardedTotalValue,
                    sizeof (objRxPortFramesDiscardedTotalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_RxPortFramesErrors
*
* @purpose Get 'RxPortFramesErrors'
*
* @description [RxPortFramesErrors]: The number of invalid LLDP frames received
*              by this LLDP agent on the indicated port, while this
*              LLDP agent is enabled. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_RxPortFramesErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxPortFramesErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & keyRxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRxPortFramesErrorsGet (keyRxPortNumValue,
                                         &objRxPortFramesErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxPortFramesErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxPortFramesErrorsValue,
                           sizeof (objRxPortFramesErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_RxPortFramesTotal
*
* @purpose Get 'RxPortFramesTotal'
*
* @description [RxPortFramesTotal]: The number of valid LLDP frames received
*              by this LLDP agent on the indicated port, while this LLDP
*              agent is enabled. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_RxPortFramesTotal (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxPortFramesTotalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & keyRxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRxPortFramesTotalGet ( keyRxPortNumValue,
                                        &objRxPortFramesTotalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxPortFramesTotal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxPortFramesTotalValue,
                           sizeof (objRxPortFramesTotalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_RxPortTLVsDiscardedTotal
*
* @purpose Get 'RxPortTLVsDiscardedTotal'
*
* @description [RxPortTLVsDiscardedTotal]: The number of LLDP TLVs discarded
*              for any reason by this LLDP agent on the indicated port.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_RxPortTLVsDiscardedTotal (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxPortTLVsDiscardedTotalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & keyRxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRxPortTLVsDiscardedGet ( keyRxPortNumValue,
                                          &objRxPortTLVsDiscardedTotalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxPortTLVsDiscardedTotal */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objRxPortTLVsDiscardedTotalValue,
                    sizeof (objRxPortTLVsDiscardedTotalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_RxPortTLVsUnrecognizedTotal
*
* @purpose Get 'RxPortTLVsUnrecognizedTotal'
*
* @description [RxPortTLVsUnrecognizedTotal]: The number of LLDP TLVs received
*              on the given port that are not recognized by this LLDP
*              agent on the indicated port.An unrecognized TLV is referred
*              to as the TLV whose type value is in the range of reserved
*              TLV types (000 1001 - 111 1110) in T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_RxPortTLVsUnrecognizedTotal (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxPortTLVsUnrecognizedTotalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & keyRxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRxPortTLVsUnrecognizedGet ( keyRxPortNumValue,
                                             &objRxPortTLVsUnrecognizedTotalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxPortTLVsUnrecognizedTotal */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objRxPortTLVsUnrecognizedTotalValue,
                    sizeof (objRxPortTLVsUnrecognizedTotalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_RxPortAgeoutsTotal
*
* @purpose Get 'RxPortAgeoutsTotal'
*
* @description [RxPortAgeoutsTotal]: The counter that represents the number
*              of age-outs that occurred on a given port. An age-out is
*              the number of times the complete set of information advertised
*              by a particular MSAP has been deleted from tables contained
*              in RemoteSystemsData and lldpExtens 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_RxPortAgeoutsTotal (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxPortAgeoutsTotalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & keyRxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpStatsRxPortAgeoutsGet ( keyRxPortNumValue,
                                             &objRxPortAgeoutsTotalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxPortAgeoutsTotal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxPortAgeoutsTotalValue,
                           sizeof (objRxPortAgeoutsTotalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_LldpStatsRxPortTLVs8021
*
* @purpose Get 'LldpStatsRxPortTLVs8021'
*
* @description [LldpStatsRxPortTLVs8021]: The counter that represents the
*              LLDP TLV 8021 count for the specified interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_LldpStatsRxPortTLVs8021(void *wap, void *bufp) 
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpStatsRxPortTLVs8021Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & keyRxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpStatsRxPortTLVs8021Get ( keyRxPortNumValue,
                                             &objLldpStatsRxPortTLVs8021Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxPortAgeoutsTotal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLldpStatsRxPortTLVs8021Value,
                           sizeof (objLldpStatsRxPortTLVs8021Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_LldpStatsRxPortTLVs8023
*
* @purpose Get 'LldpStatsRxPortTLVs8023'
*
* @description [LldpStatsRxPortTLVs8023]: The counter that represents the
*              LLDP TLV 8023 count for the specified interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_LldpStatsRxPortTLVs8023(void *wap, void *bufp) 
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpStatsRxPortTLVs8023Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & keyRxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpStatsRxPortTLVs8023Get ( keyRxPortNumValue,
                                             &objLldpStatsRxPortTLVs8023Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxPortAgeoutsTotal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLldpStatsRxPortTLVs8023Value,
                           sizeof (objLldpStatsRxPortTLVs8023Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_LldpStatsRxPortTLVsMED
*
* @purpose Get 'LldpStatsRxPortTLVsMED'
*
* @description [LldpStatsRxPortTLVsMED]: The counter that represents the LLDP
*              MED TLV count for the specified interface. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_LldpStatsRxPortTLVsMED(void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRxPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpStatsRxPortTLVsMEDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RxPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPStats_RxPortNum,
                          (xLibU8_t *) & keyRxPortNumValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRxPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpStatsRxPortTLVsMEDGet ( keyRxPortNumValue,
                                             &objLldpStatsRxPortTLVsMEDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxPortAgeoutsTotal */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLldpStatsRxPortTLVsMEDValue,
                           sizeof (objLldpStatsRxPortTLVsMEDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_TablesDeletes
*
* @purpose Get 'TablesDeletes'
*
* @description [TablesDeletes]: The number of times the complete set of information
*              advertised by a particular MSAP has been deleted
*              from tables contained in lldpRemoteSystemsData and lldpExtensions
*              objects. This counter should be incremented only once
*              when the complete set of informat 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_TablesDeletes (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTablesDeletesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRemoteDeletesGet ( &objTablesDeletesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTablesDeletesValue,
                     sizeof (objTablesDeletesValue));

  /* return the object value: TablesDeletes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTablesDeletesValue,
                           sizeof (objTablesDeletesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_TablesDrops
*
* @purpose Get 'TablesDrops'
*
* @description [TablesDrops]: The number of times the complete set of information
*              advertised by a particular MSAP could not be entered
*              into tables contained in lldpRemoteSystemsData and lldpExtensions
*              objects because of insufficient resources. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_TablesDrops (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTablesDropsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRemoteDropsGet (&objTablesDropsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTablesDropsValue, sizeof (objTablesDropsValue));

  /* return the object value: TablesDrops */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTablesDropsValue,
                           sizeof (objTablesDropsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_TablesInserts
*
* @purpose Get 'TablesInserts'
*
* @description [TablesInserts]: The number of times the complete set of information
*              advertised by a particular MSAP has been inserted
*              into tables contained in lldpRemoteSystemsData and lldpExtensions
*              objects. The complete set of information received from
*              a particular MSAP should be inse 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_TablesInserts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTablesInsertsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRemoteInsertsGet ( &objTablesInsertsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTablesInsertsValue,
                     sizeof (objTablesInsertsValue));

  /* return the object value: TablesInserts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTablesInsertsValue,
                           sizeof (objTablesInsertsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_TablesAgeouts
*
* @purpose Get 'TablesAgeouts'
*
* @description [TablesAgeouts]: The number of times the complete set of information
*              advertised by a particular MSAP has been deleted
*              from tables contained in lldpRemoteSystemsData and lldpExtensions
*              objects because the information timeliness interval
*              has expired.This counter 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_TablesAgeouts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTablesAgeoutsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRemoteAgeoutsGet ( &objTablesAgeoutsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTablesAgeoutsValue,
                     sizeof (objTablesAgeoutsValue));

  /* return the object value: TablesAgeouts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTablesAgeoutsValue,
                           sizeof (objTablesAgeoutsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPStats_LastChangeTime
*
* @purpose Get 'LastChangeTime'
*
* @description [LastChangeTime]: The value of sysUpTime object (defined in
*              IETF RFC 3418) at the time an entry is created, modified,
*              or deleted in the in tables associated with the lldpRemoteSystemsData
*              objects and all LLDP extension objects associated
*              with remote systems 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPStats_LastChangeTime(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLastChangeTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpStatsRemoteLastUpdateGet ( &objLastChangeTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLastChangeTimeValue,
                     sizeof (objLastChangeTimeValue));

  /* return the object value: TablesAgeouts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLastChangeTimeValue,
                           sizeof (objLastChangeTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
          

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPStats_ClearStatistics
*
* @purpose Set 'ClearStatistics'
*
* @description [ClearStatistics] Clears all 802.1AB statistics
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPStats_ClearStatistics (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClearStatisticsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ClearStatistics */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objClearStatisticsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClearStatisticsValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbLldpStatsClear ();
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


