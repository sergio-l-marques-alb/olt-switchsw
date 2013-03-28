/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingStpPortConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to MST-object.xml
*
* @create  13 February 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingStpPortConfig_obj.h"
#include "usmdb_dot1s_api.h"
#include "dot1s_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_dot3ad_api.h"


/*To be placed in a common file*/
L7_RC_t
fpObjUtil_SwitchingStpPortConfig_Interface_NextGet(L7_uint32 cstPortInput, L7_uint32 *cstPort)
{
  L7_uint32 cistId = DOT1S_CIST_ID;
  L7_uint32 intIfNum = 0;
  L7_uint32 extIfNum = 0;
  L7_INTF_TYPES_t itype;

  L7_uint32 nextExtIfNum = 0;
  L7_uint32 prevExtIfNum = 0;

  *cstPort =0;


  if(cstPortInput == 0)
  {
    /* Get the first valid port interface*/
    intIfNum = 0;
    extIfNum = 0;

    while (usmDbGetNextVisibleExtIfNumber(prevExtIfNum , &nextExtIfNum) == L7_SUCCESS)
    {
      prevExtIfNum  = nextExtIfNum;
      if (usmDbIntIfNumFromExtIfNum(intIfNum, &nextExtIfNum) == L7_SUCCESS)
      {
        (void)usmDbIntfTypeGet(intIfNum, &itype);
        if ((itype == L7_LAG_INTF) || (itype == L7_PHYSICAL_INTF))
        {
          *cstPort = nextExtIfNum;
          return L7_SUCCESS;
        }
      }
    }
    return L7_FAILURE;
  }  

  while (usmDbDot1sPortNextGet(L7_UNIT_CURRENT, cistId, intIfNum, &intIfNum) == L7_SUCCESS)
  {
    /* convert internal interface number to external interface number */
    if (usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum) == L7_SUCCESS)
    {
      if (extIfNum > cstPortInput)
      {
        /* check to see if this is a visible external interface number */
        if (usmDbVisibleExtIfNumberCheck(L7_UNIT_CURRENT, extIfNum) == L7_SUCCESS)
        {
          *cstPort = extIfNum;
          return L7_SUCCESS;
        }
      }
    }
  }

  return L7_FAILURE;
}

/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface]: Specifies all configurable interfaces for STP.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    objInterfaceValue = 0;
    owa.l7rc = usmDbDot1sPortNextGet(L7_UNIT_CURRENT, DOT1S_CIST_ID, objInterfaceValue, &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbDot1sPortNextGet(L7_UNIT_CURRENT, DOT1S_CIST_ID, objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpPortState
*
* @purpose Get 'StpPortState'
*
* @description [StpPortState]: The administrative STP state for the port.
*              enable(1) - enables STP on the port. disable(2) - disables
*              STP on the port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortState (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);
 
  /* get the value from application */
  owa.l7rc = usmDbDot1sPortStateGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                     &objStpPortStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpPortState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpPortStateValue,
                           sizeof (objStpPortStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingStpPortConfig_StpPortState
*
* @purpose Set 'StpPortState'
*
* @description [StpPortState]: The administrative STP state for the port.
*              enable(1) - enables STP on the port. disable(2) - disables
*              STP on the port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStpPortConfig_StpPortState (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortStateValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StpPortState */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStpPortStateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStpPortStateValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1sPortStateSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                     objStpPortStateValue);
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
* @function fpObjGet_SwitchingStpPortConfig_StpPortStatsMstpBpduRx
*
* @purpose Get 'StpPortStatsMstpBpduRx'
*
* @description [StpPortStatsMstpBpduRx]: The MSTP BPDUs received on a specific
*              port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortStatsMstpBpduRx (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortStatsMstpBpduRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1sPortStatsMSTPBPDUsReceivedGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                             &objStpPortStatsMstpBpduRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpPortStatsMstpBpduRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpPortStatsMstpBpduRxValue,
                           sizeof (objStpPortStatsMstpBpduRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpPortStatsMstpBpduTx
*
* @purpose Get 'StpPortStatsMstpBpduTx'
*
* @description [StpPortStatsMstpBpduTx]:  The MSTP BPDUs sent on a specific
*              port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortStatsMstpBpduTx (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortStatsMstpBpduTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1sPortStatsMSTPBPDUsSentGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                         &objStpPortStatsMstpBpduTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpPortStatsMstpBpduTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpPortStatsMstpBpduTxValue,
                           sizeof (objStpPortStatsMstpBpduTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpPortStatsRstpBpduRx
*
* @purpose Get 'StpPortStatsRstpBpduRx'
*
* @description [StpPortStatsRstpBpduRx]: The RSTP BPDUs received on a specific
*              port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortStatsRstpBpduRx (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortStatsRstpBpduRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1sPortStatsRSTPBPDUsReceivedGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                             &objStpPortStatsRstpBpduRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpPortStatsRstpBpduRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpPortStatsRstpBpduRxValue,
                           sizeof (objStpPortStatsRstpBpduRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpPortStatsRstpBpduTx
*
* @purpose Get 'StpPortStatsRstpBpduTx'
*
* @description [StpPortStatsRstpBpduTx]:  The RSTP BPDUs sent on a specific
*              port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortStatsRstpBpduTx (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortStatsRstpBpduTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1sPortStatsRSTPBPDUsSentGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                         &objStpPortStatsRstpBpduTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpPortStatsRstpBpduTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpPortStatsRstpBpduTxValue,
                           sizeof (objStpPortStatsRstpBpduTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpPortStatsStpBpduRx
*
* @purpose Get 'StpPortStatsStpBpduRx'
*
* @description [StpPortStatsStpBpduRx]: The STP BPDUs received on a specific
*              port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortStatsStpBpduRx (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortStatsStpBpduRxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1sPortStatsSTPBPDUsReceivedGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                            &objStpPortStatsStpBpduRxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpPortStatsStpBpduRx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpPortStatsStpBpduRxValue,
                           sizeof (objStpPortStatsStpBpduRxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpPortStatsStpBpduTx
*
* @purpose Get 'StpPortStatsStpBpduTx'
*
* @description [StpPortStatsStpBpduTx]: The STP BPDUs sent from a specific
*              port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortStatsStpBpduTx (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortStatsStpBpduTxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1sPortStatsSTPBPDUsSentGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                        &objStpPortStatsStpBpduTxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpPortStatsStpBpduTx */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpPortStatsStpBpduTxValue,
                           sizeof (objStpPortStatsStpBpduTxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpPortUpTime
*
* @purpose Get 'StpPortUpTime'
*
* @description [StpPortUpTime]: Time since port was reset.It is displayed
*              in days, hours, minutes, and seconds. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortUpTime (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objStpPortUpTimeValue={0};
  usmDbTimeSpec_t val;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  memset(&val, 0, sizeof(usmDbTimeSpec_t));

  /* get the value from application */
  owa.l7rc = usmDbDot1sPortUpTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                      &val);
#if 0
  objStpPortUpTimeValue = val.seconds;
  objStpPortUpTimeValue += val.minutes * SECONDS_PER_MINUTE;
  objStpPortUpTimeValue += val.hours * SECONDS_PER_HOUR;
  objStpPortUpTimeValue += val.days * SECONDS_PER_DAY;

  objStpPortUpTimeValue *= 100; 
 #endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  sprintf(objStpPortUpTimeValue, "%d day %d hr %d min %d sec", val.days, val.hours, val.minutes, val.seconds);

  /* return the object value: StpPortUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpPortUpTimeValue,
                           strlen (objStpPortUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpPortMigrationCheck
*
* @purpose Get 'StpPortMigrationCheck'
*
* @description [StpPortMigrationCheck]: Force the specified port to transmit
*              RSTP or MSTP BPDU's. Supported values:false(0) - BPDUs are
*              not to be transmitted. true(1) - BPDUs are to be transmitted
*              A non-zero value indicates that BPDUs are to be sent on
*              the specified port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortMigrationCheck (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortMigrationCheckValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;

  /*Based on SNMP Implimentation, we have followed same approach */
  objStpPortMigrationCheckValue = L7_FALSE;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpPortMigrationCheck */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpPortMigrationCheckValue,
                           sizeof (objStpPortMigrationCheckValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingStpPortConfig_StpPortMigrationCheck
*
* @purpose Set 'StpPortMigrationCheck'
*
* @description [StpPortMigrationCheck]: Force the specified port to transmit
*              RSTP or MSTP BPDU's. Supported values:false(0) - BPDUs are
*              not to be transmitted. true(1) - BPDUs are to be transmitted
*              A non-zero value indicates that BPDUs are to be sent on
*              the specified port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStpPortConfig_StpPortMigrationCheck (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortMigrationCheckValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StpPortMigrationCheck */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objStpPortMigrationCheckValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStpPortMigrationCheckValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1sPortForceMigrationCheck (L7_UNIT_CURRENT, keyInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objStpPortMigrationCheckValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/* ObjectRemoved */
/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpPortHelloTime
*
* @purpose Get 'StpPortHelloTime'
*
* @description [StpPortHelloTime]: Hello time for the STP port. The default
*              value is 2. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpPortHelloTime (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortHelloTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDot1sCistPortAdminHelloTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                         &objStpPortHelloTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpPortHelloTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpPortHelloTimeValue,
                           sizeof (objStpPortHelloTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingStpPortConfig_StpPortHelloTime
*
* @purpose Set 'StpPortHelloTime'
*
* @description [StpPortHelloTime]: Hello time for the STP port. The default
*              value is 2. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStpPortConfig_StpPortHelloTime (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpPortHelloTimeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StpPortHelloTime */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objStpPortHelloTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStpPortHelloTimeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1sCistPortAdminHelloTimeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                         objStpPortHelloTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#endif

/*******************************************************************************
* @function fpObjSet_SwitchingStpPortConfig_Dot1sPortStatsClear
*
* @purpose Set 'Dot1sPortStatsClear'
*
* @description [Dot1sPortStatsClear]: TODO 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStpPortConfig_Dot1sPortStatsClear(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1sPortStatsClearValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: objDot1sPortStatsClearValue */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot1sPortStatsClearValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot1sPortStatsClearValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDot1sPortStatsClear (L7_UNIT_CURRENT, keyInterfaceValue);
    
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objDot1sPortStatsClearValue = L7_TRUE;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpPortConfig_StpIntfLAG
*
* @purpose Get 'StpIntfLAG'
 *@description  [StpIntfLAG] TODO
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpPortConfig_StpIntfLAG (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objStpIntfLAGValue;
  xLibU8_t name[L7_LAG_NAME_SIZE];

  xLibU32_t keyInterfaceValue, val;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpPortConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  memset(objStpIntfLAGValue, 0x00, sizeof(objStpIntfLAGValue));
  if (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, keyInterfaceValue, &val) == L7_SUCCESS)
  {
    memset(name,0x00,sizeof(name));
    owa.l7rc = usmDbDot3adNameGet(L7_UNIT_CURRENT, val, name);
    if (owa.l7rc == L7_SUCCESS)
    {
      osapiSnprintf(objStpIntfLAGValue, sizeof(objStpIntfLAGValue), "%s", name);
    }
  }
  else
  {
     /* Just return a string as None */
     osapiSnprintf(objStpIntfLAGValue,sizeof(objStpIntfLAGValue),"%s","None");
     owa.l7rc = L7_SUCCESS;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objStpIntfLAGValue, strlen (objStpIntfLAGValue));

  /* return the object value: StpIntfLAG */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpIntfLAGValue, strlen (objStpIntfLAGValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

