/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_Switchingdot1dGarpPortTimerConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to GARP-object.xml
*
* @create  6 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_Switchingdot1dGarpPortTimerConfig_obj.h"
#include "usmdb_garp.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_Switchingdot1dGarpPortTimerConfig_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface]: Interface to be configured for GARP Parameters
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGarpPortTimerConfig_Interface (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                 0, &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);

    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                               0, objInterfaceValue, &nextObjInterfaceValue);
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
* @function fpObjGet_Switchingdot1dGarpPortTimerConfig_JoinTime
*
* @purpose Get 'JoinTime'
*
* @description [JoinTime]: The GARP Join time, in centiseconds. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGarpPortTimerConfig_JoinTime (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objJoinTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbGarpJoinTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                   &objJoinTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: JoinTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objJoinTimeValue,
                           sizeof (objJoinTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1dGarpPortTimerConfig_JoinTime
*
* @purpose Set 'JoinTime'
*
* @description [JoinTime]: The GARP Join time, in centiseconds. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dGarpPortTimerConfig_JoinTime (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objJoinTimeValue;
  xLibU32_t objLeaveTimeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: JoinTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objJoinTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objJoinTimeValue, owa.len);

  if ((objJoinTimeValue % 10) != 0)
  {
      owa.rc = XLIBRC_INPUT_NOT_MULTIPLEOF_10;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc; 
  }

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* retrieve LeaveTime */
  owa.l7rc = usmDbGarpLeaveTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                    &objLeaveTimeValue);
  if(owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
	
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLeaveTimeValue, owa.len);

	
  if(objLeaveTimeValue < (2 * objJoinTimeValue))
  {
    owa.rc = XLIBRC_LEAVE_JOIN_TIME_COND_FAILURE;
  }
  else 
  {
    /* set the value in application */
	
    owa.l7rc = usmDbGarpJoinTimeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                     objJoinTimeValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1dGarpPortTimerConfig_LeaveTime
*
* @purpose Get 'LeaveTime'
*
* @description [LeaveTime]: The GARP Leave time, in centiseconds. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGarpPortTimerConfig_LeaveTime (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaveTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbGarpLeaveTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                    &objLeaveTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LeaveTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLeaveTimeValue,
                           sizeof (objLeaveTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1dGarpPortTimerConfig_LeaveTime
*
* @purpose Set 'LeaveTime'
*
* @description [LeaveTime]: The GARP Leave time, in centiseconds. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dGarpPortTimerConfig_LeaveTime (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaveTimeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t objJoinTimeValue;
  xLibU32_t objLeaveAllTimeValue;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LeaveTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLeaveTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLeaveTimeValue, owa.len);

  if ((objLeaveTimeValue % 10) != 0)
  {
      owa.rc = XLIBRC_INPUT_NOT_MULTIPLEOF_10;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc; 
  }


  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* retrieve JoinTime */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_JoinTime,
                          (xLibU8_t *) &objJoinTimeValue, &kwa.len);
  if ( (kwa.rc != XLIBRC_SUCCESS) || ((objJoinTimeValue % 10) != 0) )
  {
	  owa.l7rc = usmDbGarpJoinTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
		                                   &objJoinTimeValue);
	  if (owa.l7rc != L7_SUCCESS)
	  {
	    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
	  }
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objJoinTimeValue, kwa.len);


  /* retrieve Leavealltimer */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_LeaveAllTime,
                          (xLibU8_t *) & objLeaveAllTimeValue, &kwa.len);
  if ( (kwa.rc != XLIBRC_SUCCESS) || ((objLeaveAllTimeValue % 10) != 0) )
  {
	  owa.l7rc = usmDbGarpLeaveAllTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
		                                   &objLeaveAllTimeValue);
	  if (owa.l7rc != L7_SUCCESS)
	  {
	    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
	    FPOBJ_TRACE_EXIT (bufp, owa);
	    return owa.rc;
	  }
  }	
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLeaveAllTimeValue, owa.len);

  /** LeaveAllTimer should be greater than lavetimer */
  if(objLeaveAllTimeValue <= objLeaveTimeValue )
  {
    owa.rc = XLIBRC_LEAVE_LEAVEALL_TIME_COND_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
		

  /* The leaveTime must be greator than or equal to twice the
  ** joinTime
  **/
 
  if( objLeaveTimeValue < (2 * objJoinTimeValue))
  {
    owa.rc = XLIBRC_LEAVE_JOIN_TIME_COND_FAILURE;     
  }  
  else 
  {
    /* set the value in application */
    owa.l7rc = usmDbGarpLeaveTimeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                     objLeaveTimeValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_Switchingdot1dGarpPortTimerConfig_LeaveAllTime
*
* @purpose Get 'LeaveAllTime'
*
* @description [LeaveAllTime]: The GARP LeaveAll time, in centiseconds 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGarpPortTimerConfig_LeaveAllTime (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaveAllTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbGarpLeaveAllTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                       &objLeaveAllTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* return the object value: LeaveAllTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLeaveAllTimeValue,
                           sizeof (objLeaveAllTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_Switchingdot1dGarpPortTimerConfig_LeaveAllTime
*
* @purpose Set 'LeaveAllTime'
*
* @description [LeaveAllTime]: The GARP LeaveAll time, in centiseconds 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_Switchingdot1dGarpPortTimerConfig_LeaveAllTime (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLeaveAllTimeValue;
  xLibU32_t objLeaveTimeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LeaveAllTime */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLeaveAllTimeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLeaveAllTimeValue, owa.len);

  if ((objLeaveAllTimeValue % 10) != 0)
  {
      owa.rc = XLIBRC_INPUT_NOT_MULTIPLEOF_10;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc; 
  }

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* retrieve LeaveTime */
  owa.l7rc = usmDbGarpLeaveTimeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                    &objLeaveTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objLeaveTimeValue, owa.len);
 
  /* The leaveAll Time must be greator than leave time */
  if (objLeaveAllTimeValue <= objLeaveTimeValue )
  {
    owa.rc = XLIBRC_LEAVE_LEAVEALL_TIME_COND_FAILURE;
  }
  else 
  {
    /* set the value in application */
    owa.l7rc = usmDbGarpLeaveAllTimeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                       objLeaveAllTimeValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE; 
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_Switchingdot1dGarpPortTimerConfig_RxGvrpPdus
*
* @purpose Get 'RxGvrpPdus'
*
* @description [RxGvrpPdus]: the number of recieved GVRP PDUs 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGarpPortTimerConfig_RxGvrpPdus (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRxGvrpPdusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =usmDbRxGvrpPdusGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                &objRxGvrpPdusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RxGvrpPdus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRxGvrpPdusValue,
                           sizeof (objRxGvrpPdusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_Switchingdot1dGarpPortTimerConfig_TxGvrpPdus
*
* @purpose Get 'TxGvrpPdus'
*
* @description [TxGvrpPdus]: the number of transmited GVRP PDUs 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_Switchingdot1dGarpPortTimerConfig_TxGvrpPdus (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxGvrpPdusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_Switchingdot1dGarpPortTimerConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.l7rc != L7_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbTxGvrpPdusGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                 &objTxGvrpPdusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TxGvrpPdus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxGvrpPdusValue,
                           sizeof (objTxGvrpPdusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
