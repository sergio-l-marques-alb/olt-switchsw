
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_securitydot1xLogicalPort.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to security-object.xml
*
* @create  20 April 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_securitydot1xLogicalPort_obj.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_util_api.h"
/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xPhysicalPort
*
* @purpose Get 'dot1xPhysicalPort'
 *@description  [dot1xPhysicalPort] The dot1x Logical Port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xPhysicalPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdot1xPhysicalPortValue;
  xLibU32_t nextObjdot1xPhysicalPortValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xPhysicalPort */
  owa.len = sizeof (objdot1xPhysicalPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xPhysicalPort,
                          (xLibU8_t *) & objdot1xPhysicalPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objdot1xPhysicalPortValue);
    owa.l7rc = usmDbValidIntIfNumFirstGet (&nextObjdot1xPhysicalPortValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1xPhysicalPortValue, owa.len);
    do
    {
      owa.l7rc=usmDbGetNextPhysicalIntIfNumber ( objdot1xPhysicalPortValue,
                                    &nextObjdot1xPhysicalPortValue);
    }
    while (FPOBJ_CMP_U32 (objdot1xPhysicalPortValue, nextObjdot1xPhysicalPortValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1xPhysicalPortValue, owa.len);

  /* return the object value: dot1xPhysicalPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1xPhysicalPortValue,
                           sizeof (nextObjdot1xPhysicalPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xAllLogicalPorts
*
* @purpose Get 'dot1xAllLogicalPort'
 *@description  [dot1xAllLogicalPort] The dot1x Logical Ports for unauthenticated also.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xAllLogicalPorts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdot1xLogicalPortValue;
  xLibU32_t nextObjdot1xLogicalPortValue;
  xLibU32_t lIntIfNum;
  xLibU32_t objdot1xPhysicalPortValue;
  xLibU32_t physport;
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: dot1xPhysicalPort */
  owa.len = sizeof (objdot1xPhysicalPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xPhysicalPort,
                          (xLibU8_t *) & objdot1xPhysicalPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
     owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1xPhysicalPortValue, owa.len);


  /* retrieve key: dot1xLogicalPort */
  owa.len = sizeof (objdot1xLogicalPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xAllLogicalPorts,
                          (xLibU8_t *) & objdot1xLogicalPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objdot1xLogicalPortValue);
    owa.l7rc = usmDbDot1xLogicalPortFirstGet(objdot1xPhysicalPortValue,
                                     &lIntIfNum);
    do
    {
      if (owa.l7rc != L7_SUCCESS)
      {
        break;
      }
      if (((owa.l7rc = usmDbDot1xPhysicalPortGet(L7_UNIT_CURRENT,lIntIfNum,&physport))== L7_SUCCESS) &&
                           (physport != objdot1xPhysicalPortValue))
      {
         owa.l7rc = L7_FAILURE;
         break;
      }
      else if (owa.l7rc == L7_SUCCESS)
      {
        nextObjdot1xLogicalPortValue = lIntIfNum;
        break;
      }
    }while((owa.l7rc=usmDbDot1xLogicalPortNextGet(lIntIfNum,&lIntIfNum)) == L7_SUCCESS);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1xLogicalPortValue, owa.len);
    lIntIfNum = objdot1xLogicalPortValue;

    while ((owa.l7rc = usmDbDot1xLogicalPortNextGet (lIntIfNum,&lIntIfNum))== L7_SUCCESS)
    {
      if (((owa.l7rc = usmDbDot1xPhysicalPortGet(L7_UNIT_CURRENT,lIntIfNum,&physport))== L7_SUCCESS) &&
                           (physport != objdot1xPhysicalPortValue))
      {
         owa.l7rc = L7_FAILURE;
         break;
      }
      else if (owa.l7rc == L7_SUCCESS)
      {
        nextObjdot1xLogicalPortValue = lIntIfNum;
        break;
      }
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1xLogicalPortValue, owa.len);

  /* return the object value: dot1xLogicalPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1xLogicalPortValue,
                           sizeof (nextObjdot1xLogicalPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPort
*
* @purpose Get 'dot1xLogicalPort'
 *@description  [dot1xLogicalPort] The dot1x Logical Port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdot1xLogicalPortValue;
  xLibU32_t nextObjdot1xLogicalPortValue;
  xLibU32_t lIntIfNum, reason;
  xLibU32_t val,controlMode;
  xLibU32_t objdot1xPhysicalPortValue;
  L7_BOOL flag = L7_FALSE;
  xLibU32_t physport;
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: dot1xPhysicalPort */
  owa.len = sizeof (objdot1xPhysicalPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xPhysicalPort,
                          (xLibU8_t *) & objdot1xPhysicalPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1xPhysicalPortValue, owa.len);

  /* Getting Logical port is valid only if it satisfies the below conditions */
   if(usmDbDot1xPortControlModeGet(L7_UNIT_CURRENT, objdot1xPhysicalPortValue, &controlMode) == L7_SUCCESS &&
         (controlMode == L7_DOT1X_PORT_AUTO_MAC_BASED || controlMode == L7_DOT1X_PORT_AUTO))
   {
       if ((controlMode == L7_DOT1X_PORT_AUTO_MAC_BASED) ||
               ((controlMode == L7_DOT1X_PORT_AUTO) &&
               (usmDbDot1xPortVlanAssignedReasonGet(L7_UNIT_CURRENT,objdot1xPhysicalPortValue,&reason)== L7_SUCCESS)&&
               (reason != L7_DOT1X_GUEST_VLAN)))
        {
          flag = L7_TRUE; 
        }
     
   }
   if (flag == L7_FALSE)
   {
     owa.rc = XLIBRC_ENDOF_TABLE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
   else if(!(usmDbDot1xPortAuthControlledPortStatusGet(L7_UNIT_CURRENT, objdot1xPhysicalPortValue, &val) == L7_SUCCESS &&
              val == L7_DOT1X_PORT_STATUS_AUTHORIZED))
   {
     owa.rc = XLIBRC_ENDOF_TABLE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
       
   }

  /* retrieve key: dot1xLogicalPort */
  owa.len = sizeof (objdot1xLogicalPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                          (xLibU8_t *) & objdot1xLogicalPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objdot1xLogicalPortValue);
    owa.l7rc = usmDbDot1xLogicalPortFirstGet(objdot1xPhysicalPortValue,
                                     &lIntIfNum);
    do
    {
      val = 0;
      if ((usmDbDot1xPhysicalPortGet(L7_UNIT_CURRENT,lIntIfNum,&physport)== L7_SUCCESS) &&
                           (physport != objdot1xPhysicalPortValue))
      {
         owa.l7rc = L7_FAILURE; 
         break; 
      }
      if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&val) == L7_SUCCESS)&&
                           (val == L7_DOT1X_APM_AUTHENTICATED))
      {
        nextObjdot1xLogicalPortValue = lIntIfNum;
        break; 
      } 
    }while((owa.l7rc=usmDbDot1xLogicalPortNextGet(lIntIfNum,&lIntIfNum)) == L7_SUCCESS);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1xLogicalPortValue, owa.len);
    val = 0;
    lIntIfNum = objdot1xLogicalPortValue;

    while ((owa.l7rc = usmDbDot1xLogicalPortNextGet (lIntIfNum,&lIntIfNum))== L7_SUCCESS) 
    {
      val = 0;
      if ((usmDbDot1xPhysicalPortGet(L7_UNIT_CURRENT,lIntIfNum,&physport)== L7_SUCCESS) &&
                           (physport != objdot1xPhysicalPortValue))
      {
         owa.l7rc = L7_FAILURE; 
         break; 
      }
      if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&val) == L7_SUCCESS)&&
                           (val == L7_DOT1X_APM_AUTHENTICATED))
      {
        nextObjdot1xLogicalPortValue = lIntIfNum;
        break; 
      } 
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1xLogicalPortValue, owa.len);

  /* return the object value: dot1xLogicalPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1xLogicalPortValue,
                           sizeof (nextObjdot1xLogicalPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortPaeState
*
* @purpose Get 'dot1xLogicalPortPaeState'
*
* @description [dot1xLogicalPortPaeState] The LogicalPortPaeState.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortPaeState (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xLogicalPortPaeStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xLogicalPortPaeStateGet ( keydot1xLogicalPortValue,
                              &objdot1xLogicalPortPaeStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xLogicalPortPaeState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xLogicalPortPaeStateValue,
                           sizeof (objdot1xLogicalPortPaeStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortSessionTimeout
*
* @purpose Get 'dot1xLogicalPortSessionTimeout'
*
* @description [dot1xLogicalPortSessionTimeout] The LogicalPort Session Timeoute.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortSessionTimeout (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  /* xLibU32_t objdot1xLogicalPortSessionTimeoutValue; */
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  /* get the value from application */
  /*owa.l7rc = usmDbDot1xLogicalPortSessionTimeoutGet ( keydot1xLogicalPortValue,
                              &objdot1xLogicalPortSessionTimeoutValue); */
  owa.l7rc  = L7_FAILURE;
  owa.rc =  XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortSessionTime
*
* @purpose Get 'dot1xLogicalPortSessionTime'
*
* @description [dot1xLogicalPortSessionTime] The LogicalPort Session Time.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortSessionTime (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xLogicalPortSessionTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  /* get the value from application */
  owa.l7rc = usmdbDot1xPortSessionTimeGet (L7_UNIT_CURRENT, keydot1xLogicalPortValue,
                              &objdot1xLogicalPortSessionTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xLogicalPortSessionTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xLogicalPortSessionTimeValue,
                           sizeof (objdot1xLogicalPortSessionTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortSupplicantMacAddr
*
* @purpose Get 'dot1xLogicalPortSupplicantMacAddr'
*
* @description [dot1xLogicalPortSupplicantMacAddr] The LogicalPort Supplicant MacAddr.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortSupplicantMacAddr (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objdot1xLogicalPortSupplicantMacAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xLogicalPortSupplicantMacAddrGet ( keydot1xLogicalPortValue,
                              objdot1xLogicalPortSupplicantMacAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xLogicalPortSupplicantMacAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1xLogicalPortSupplicantMacAddrValue,
                           sizeof (objdot1xLogicalPortSupplicantMacAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortTerminationAction
*
* @purpose Get 'dot1xLogicalPortTerminationAction'
*
* @description [dot1xLogicalPortTerminationAction] The Logical Port Termination Action.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortTerminationAction (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  /*xLibU32_t objdot1xLogicalPortTerminationActionValue;*/
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  owa.l7rc  = L7_FAILURE;
  owa.rc =  XLIBRC_NOT_SUPPORTED;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortUserName
*
* @purpose Get 'dot1xLogicalPortUserName'
*
* @description [dot1xLogicalPortUserName] The Logical Port User Name.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortUserName (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1xLogicalPortUserNameValue;
  memset(objdot1xLogicalPortUserNameValue,0,sizeof (objdot1xLogicalPortUserNameValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  /* get the value from application */
  owa.l7rc = usmdbDot1xLogicalPortUserNameGet (L7_UNIT_CURRENT, keydot1xLogicalPortValue,
                              objdot1xLogicalPortUserNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xLogicalPortUserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1xLogicalPortUserNameValue,
                           strlen (objdot1xLogicalPortUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortVlanAssignment
*
* @purpose Get 'dot1xLogicalPortVlanAssignment'
*
* @description [dot1xLogicalPortVlanAssignment] The Logical Port Vlan Assignment.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortVlanAssignment (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xLogicalPortVlanAssignmentValue;
  xLibU32_t objdot1xLogicalPortVlanAssignmentReason;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xLogicalPortVlanAssignmentGet ( keydot1xLogicalPortValue,
                                                                                  &objdot1xLogicalPortVlanAssignmentValue,
                                                                                  &objdot1xLogicalPortVlanAssignmentReason);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xLogicalPortVlanAssignment */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xLogicalPortVlanAssignmentValue,
                           sizeof (objdot1xLogicalPortVlanAssignmentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortFilterId
*
* @purpose Get 'dot1xLogicalPortFilterId'
*
* @description [dot1xLogicalPortFilterId] The Logical Port FilterId.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortFilterId (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1xLogicalPortFilterIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  /* get the value from application */
  owa.l7rc = usmdbDot1xLogicalPortFilterIdGet (L7_UNIT_CURRENT, keydot1xLogicalPortValue,
                              objdot1xLogicalPortFilterIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xLogicalPortFilterId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1xLogicalPortFilterIdValue,
                           strlen (objdot1xLogicalPortFilterIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortBackendAuthState
*
* @purpose Get 'dot1xLogicalPortBackendAuthState'
*
* @description [dot1xLogicalPortBackendAuthState] The Logical Port BackendAuthState.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortBackendAuthState (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdot1xLogicalPortBackendAuthStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xLogicalPortBackendAuthStateGet ( keydot1xLogicalPortValue,
                              &objdot1xLogicalPortBackendAuthStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xLogicalPortBackendAuthState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xLogicalPortBackendAuthStateValue,
                           sizeof (objdot1xLogicalPortBackendAuthStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortVlanAssociated
*
* @purpose Get 'dot1xLogicalPortVlanAssociated'
 *@description  [dot1xLogicalPortVlanAssociated] The Logical Port Vlan
* Assignment.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xLogicalPortVlanAssociated (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdot1xLogicalPortVlanAssociatedValue;

  xLibU32_t keydot1xLogicalPortValue;
  xLibU32_t objdot1xLogicalPortVlanAssociatedValueId;

  keydot1xLogicalPortValue =0;
  objdot1xLogicalPortVlanAssociatedValueId =0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  owa.len = sizeof (keydot1xLogicalPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xLogicalPort,
                          (xLibU8_t *) & keydot1xLogicalPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xLogicalPortVlanAssignmentGet ( keydot1xLogicalPortValue,
                              &objdot1xLogicalPortVlanAssociatedValueId,&objdot1xLogicalPortVlanAssociatedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objdot1xLogicalPortVlanAssociatedValue,
                     sizeof (objdot1xLogicalPortVlanAssociatedValue));

  /* return the object value: dot1xLogicalPortVlanAssociated */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdot1xLogicalPortVlanAssociatedValue,
                           sizeof (objdot1xLogicalPortVlanAssociatedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xAuthLogicalPort
*
* @purpose Get 'dot1xAuthLogicalPort'
 *@description  [dot1xAuthLogicalPort] The dot1x Logical Port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xAuthLogicalPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objdot1xLogicalPortValue;
  xLibU32_t nextObjdot1xLogicalPortValue;
  xLibU32_t lIntIfNum;
  xLibU32_t val,controlMode;
  xLibU32_t objdot1xPhysicalPortValue;
  L7_BOOL flag = L7_FALSE;
  xLibU32_t physport;
  FPOBJ_TRACE_ENTER (bufp);
  /* retrieve key: dot1xPhysicalPort */
  owa.len = sizeof (objdot1xPhysicalPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xPhysicalPort,
                          (xLibU8_t *) & objdot1xPhysicalPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1xPhysicalPortValue, owa.len);
  
  owa.rc = XLIBRC_FAILURE;
  /* Getting Logical port is valid only if it satisfies the below conditions */
   if((usmDbDot1xPortControlModeGet(L7_UNIT_CURRENT, objdot1xPhysicalPortValue, &controlMode) == L7_SUCCESS) &&
       ((controlMode == L7_DOT1X_PORT_AUTO_MAC_BASED) ||
               (controlMode == L7_DOT1X_PORT_AUTO)) &&
       ((usmDbDot1xPortAuthControlledPortStatusGet(L7_UNIT_CURRENT, objdot1xPhysicalPortValue, &val) == L7_SUCCESS) &&
              (val == L7_DOT1X_PORT_STATUS_AUTHORIZED)))
   
   {
       flag = L7_TRUE;
   }

  if (flag != L7_TRUE)
  {
     owa.rc = XLIBRC_ENDOF_TABLE;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }     

  /* retrieve key: dot1xLogicalPort */
  owa.len = sizeof (objdot1xLogicalPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xAuthLogicalPort,
                          (xLibU8_t *) & objdot1xLogicalPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objdot1xLogicalPortValue);
    owa.l7rc = usmDbDot1xLogicalPortFirstGet(objdot1xPhysicalPortValue,
                                     &lIntIfNum);
    do
    {
      val = 0;
      if ((usmDbDot1xPhysicalPortGet(L7_UNIT_CURRENT,lIntIfNum,&physport)== L7_SUCCESS) &&
                           (physport != objdot1xPhysicalPortValue))
      {
         owa.l7rc = L7_FAILURE; 
         break; 
      }
      if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&val) == L7_SUCCESS)&&
                           (val == L7_DOT1X_APM_AUTHENTICATED))
      {
        nextObjdot1xLogicalPortValue = lIntIfNum;
        break; 
      } 
    }while((owa.l7rc=usmDbDot1xLogicalPortNextGet(lIntIfNum,&lIntIfNum)) == L7_SUCCESS);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdot1xLogicalPortValue, owa.len);
    val = 0;
    lIntIfNum = objdot1xLogicalPortValue;

    while ((owa.l7rc = usmDbDot1xLogicalPortNextGet (lIntIfNum,&lIntIfNum))== L7_SUCCESS) 
    {
      val = 0;
      if ((usmDbDot1xPhysicalPortGet(L7_UNIT_CURRENT,lIntIfNum,&physport)== L7_SUCCESS) &&
                           (physport != objdot1xPhysicalPortValue))
      {
         owa.l7rc = L7_FAILURE; 
         break; 
      }
      if ((usmDbDot1xLogicalPortPaeStateGet(lIntIfNum,&val) == L7_SUCCESS)&&
                           (val == L7_DOT1X_APM_AUTHENTICATED))
      {
        nextObjdot1xLogicalPortValue = lIntIfNum;
        break; 
      } 
    }
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdot1xLogicalPortValue, owa.len);

  /* return the object value: dot1xLogicalPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdot1xLogicalPortValue,
                           sizeof (nextObjdot1xLogicalPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydot1xLogicalPort_dot1xAuthLogicalPortUserName
*
* @purpose Get 'dot1xAuthLogicalPortUserName'
*
* @description [dot1xAuthLogicalPortUserName] The Logical Port User Name.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydot1xLogicalPort_dot1xAuthLogicalPortUserName (void *wap, void *bufp)
{

  fpObjWa_t kwadot1xLogicalPort = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydot1xLogicalPortValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdot1xLogicalPortUserNameValue;
  memset(objdot1xLogicalPortUserNameValue,0,sizeof (objdot1xLogicalPortUserNameValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dot1xLogicalPort */
  kwadot1xLogicalPort.rc = xLibFilterGet (wap, XOBJ_securitydot1xLogicalPort_dot1xAuthLogicalPort,
                                          (xLibU8_t *) & keydot1xLogicalPortValue,
                                          &kwadot1xLogicalPort.len);
  if (kwadot1xLogicalPort.rc != XLIBRC_SUCCESS)
  {
    kwadot1xLogicalPort.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadot1xLogicalPort);
    return kwadot1xLogicalPort.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydot1xLogicalPortValue, kwadot1xLogicalPort.len);

  /* get the value from application */
  owa.l7rc = usmdbDot1xLogicalPortUserNameGet (L7_UNIT_CURRENT, keydot1xLogicalPortValue,
                              objdot1xLogicalPortUserNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dot1xLogicalPortUserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdot1xLogicalPortUserNameValue,
                           strlen (objdot1xLogicalPortUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


