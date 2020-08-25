
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_securityDot1xSupplicantIntfConfig.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to security-object.xml
*
* @create  28 June 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_securityDot1xSupplicantIntfConfig_obj.h"
#include "usmdb_dot1x_api.h"
#include "usmdb_util_api.h"
#include "usmdb_user_mgmt_api.h"

#include "usmdb_user_mgmt_api.h"

extern L7_RC_t
fpObjUtil_baseUsrMgrUserConfig_UserIndexNextGet ( xLibS32_t userIndex, L7_int32 *userIndexNext);

L7_RC_t usmDbNextSupplicantIfNumberGet(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  L7_uint32 objInterfaceValue =0, nextObjInterfaceValue = 0; 
  L7_uchar8 cap = 0;
  L7_RC_t rc = L7_FAILURE;
   
  objInterfaceValue = extIfNum;

  while(usmDbNextPhysicalExtIfNumberGet(UnitIndex, objInterfaceValue, &nextObjInterfaceValue) == L7_SUCCESS)
  {
     rc = usmDbDot1xPortPaeCapabilitiesGet(UnitIndex, nextObjInterfaceValue, &cap);

     if ( rc==L7_SUCCESS)
     {
       if (cap & L7_DOT1X_PAE_PORT_SUPP_CAPABLE)
       {
         *nextExtIfNum = nextObjInterfaceValue;
         return L7_SUCCESS;   
       }
     }
     objInterfaceValue = nextObjInterfaceValue;  
  }

  return L7_FAILURE;   
}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] A table that contains the configuration objects for
* the Supplicant PAE associated with each port. An entry appears in
* this table for each port that may authenticate itself when
* challenged by a remote system.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_Interface (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbNextSupplicantIfNumberGet(L7_UNIT_CURRENT, 0, &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbNextSupplicantIfNumberGet(L7_UNIT_CURRENT, objInterfaceValue, &nextObjInterfaceValue);
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
                           sizeof (nextObjInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#if 0
/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_Capability
*
* @purpose Get 'Capability'
 *@description  [Capability] This object indicates whether DHCP L2 Relay is
* enabled globally or not.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_Capability (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCapabilityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortPaeCapabilitiesGet(L7_UNIT_CURRENT, keyInterfaceValue, &objCapabilityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Capability */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCapabilityValue, sizeof (objCapabilityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityDot1xSupplicantIntfConfig_Capability
*
* @purpose Set 'Capability'
 *@description  [Capability] This object indicates whether DHCP L2 Relay is
* enabled globally or not.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityDot1xSupplicantIntfConfig_Capability (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objCapabilityValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Capability */
  owa.len = sizeof (objCapabilityValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objCapabilityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCapabilityValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortPaeCapabilitiesSet(keyInterfaceValue, objCapabilityValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
#endif
/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_MaxStart
*
* @purpose Get 'MaxStart'
 *@description  [MaxStart] The value of the maxStart constant currently in use
* by the Supplicant PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_MaxStart (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMaxStartValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortMaxStartGet(keyInterfaceValue, &objMaxStartValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MaxStart */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxStartValue, sizeof (objMaxStartValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityDot1xSupplicantIntfConfig_MaxStart
*
* @purpose Set 'MaxStart'
 *@description  [MaxStart] The value of the maxStart constant currently in use
* by the Supplicant PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityDot1xSupplicantIntfConfig_MaxStart (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMaxStartValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MaxStart */
  owa.len = sizeof (objMaxStartValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMaxStartValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxStartValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xSupplicantPortMaxStartSet(keyInterfaceValue, objMaxStartValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_StartPeriod
*
* @purpose Get 'StartPeriod'
 *@description  [StartPeriod] The value, in seconds, of the startPeriod constant
* currently in use by the Supplicant PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_StartPeriod (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortStartPeriodGet(keyInterfaceValue, &objStartPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StartPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartPeriodValue, sizeof (objStartPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityDot1xSupplicantIntfConfig_StartPeriod
*
* @purpose Set 'StartPeriod'
 *@description  [StartPeriod] The value, in seconds, of the startPeriod constant
* currently in use by the Supplicant PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityDot1xSupplicantIntfConfig_StartPeriod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStartPeriodValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StartPeriod */
  owa.len = sizeof (objStartPeriodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStartPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStartPeriodValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xSupplicantPortStartPeriodSet(keyInterfaceValue, objStartPeriodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_HeldPeriod
*
* @purpose Get 'HeldPeriod'
 *@description  [HeldPeriod] The value, in seconds, of the heldPeriod constant
* currently in use by the Supplicant PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_HeldPeriod (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHeldPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortHeldPeriodGet(keyInterfaceValue, &objHeldPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HeldPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objHeldPeriodValue, sizeof (objHeldPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityDot1xSupplicantIntfConfig_HeldPeriod
*
* @purpose Set 'HeldPeriod'
 *@description  [HeldPeriod] The value, in seconds, of the heldPeriod constant
* currently in use by the Supplicant PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityDot1xSupplicantIntfConfig_HeldPeriod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objHeldPeriodValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HeldPeriod */
  owa.len = sizeof (objHeldPeriodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objHeldPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objHeldPeriodValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xSupplicantPortHeldPeriodSet(keyInterfaceValue, objHeldPeriodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_AuthPeriod
*
* @purpose Get 'AuthPeriod'
 *@description  [AuthPeriod] The value, in seconds, of the authPeriod constant
* currently in use by the Supplicant PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_AuthPeriod (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthPeriodValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortAuthPeriodGet(keyInterfaceValue, &objAuthPeriodValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AuthPeriod */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthPeriodValue, sizeof (objAuthPeriodValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityDot1xSupplicantIntfConfig_AuthPeriod
*
* @purpose Set 'AuthPeriod'
 *@description  [AuthPeriod] The value, in seconds, of the authPeriod constant
* currently in use by the Supplicant PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityDot1xSupplicantIntfConfig_AuthPeriod (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAuthPeriodValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AuthPeriod */
  owa.len = sizeof (objAuthPeriodValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthPeriodValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthPeriodValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xSupplicantPortAuthPeriodSet(keyInterfaceValue, objAuthPeriodValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_ControlMode
*
* @purpose Get 'ControlMode'
 *@description  [ControlMode] Current port's control Mode
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_ControlMode (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortControlModeGet(keyInterfaceValue, &objControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objControlModeValue, sizeof (objControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityDot1xSupplicantIntfConfig_ControlMode
*
* @purpose Set 'ControlMode'
 *@description  [ControlMode] Current port's control Mode
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityDot1xSupplicantIntfConfig_ControlMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objControlModeValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ControlMode */
  owa.len = sizeof (objControlModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objControlModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objControlModeValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xSupplicantPortControlModeSet(keyInterfaceValue, objControlModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_UserName
*
* @purpose Get 'UserName'
 *@description  [UserName] Dot1x Supplicant user name.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_UserName (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objUserNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objUserNameValue, 0x0, sizeof(objUserNameValue));
  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortUserNameGet(keyInterfaceValue, objUserNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objUserNameValue, strlen (objUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityDot1xSupplicantIntfConfig_UserName
*
* @purpose Set 'UserName'
 *@description  [UserName] Dot1x Supplicant user name.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityDot1xSupplicantIntfConfig_UserName (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objUserNameValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objUserNameValue, 0x0, sizeof(objUserNameValue));
 
  /* retrieve object: UserName */
  owa.len = sizeof (objUserNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objUserNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objUserNameValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xSupplicantPortUserNameSet(keyInterfaceValue, objUserNameValue, strlen(objUserNameValue));

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjList_securityDot1xSupplicantIntfConfig_UserName
*
* @purpose List 'UserName'
 *@description  [UserName] Dot1x Supplicant user name.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_securityDot1xSupplicantIntfConfig_UserName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objUserNameValue;
  xLibStr256_t nextObjUserNameValue;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 nextObjUserIndexValue = 0; 

  memset(objUserNameValue, 0x0, sizeof(objUserNameValue));
  memset(nextObjUserNameValue, 0x0, sizeof(nextObjUserNameValue));

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objUserNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_UserName,
                          (xLibU8_t *) objUserNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    rc =
      fpObjUtil_baseUsrMgrUserConfig_UserIndexNextGet ( -1,
                                       &nextObjUserIndexValue);
    owa.rc = usmDbLoginsGet(L7_UNIT_CURRENT, nextObjUserIndexValue, nextObjUserNameValue);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objUserNameValue, owa.len);
   
    rc = usmDbUserLoginIndexGet(L7_UNIT_CURRENT, objUserNameValue, &nextObjUserIndexValue);  
    
    if (rc == L7_SUCCESS)
    {
      rc = fpObjUtil_baseUsrMgrUserConfig_UserIndexNextGet (nextObjUserIndexValue, &nextObjUserIndexValue);

      owa.rc = usmDbLoginsGet(L7_UNIT_CURRENT, nextObjUserIndexValue, nextObjUserNameValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS || (rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjUserNameValue, owa.len);

  /* return the object value: UserName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjUserNameValue, strlen (nextObjUserNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_PaeState
*
* @purpose Get 'PaeState'
 *@description  [PaeState] The current state of the Supplicant PAE state machine.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_PaeState (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPaeStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortPaeStateGet(L7_UNIT_CURRENT, keyInterfaceValue, &objPaeStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PaeState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPaeStateValue, sizeof (objPaeStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_Status
*
* @purpose Get 'Status'
 *@description  [Status] The current value of the controlled  port status
* parameter of the port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_Status (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortStatusGet(keyInterfaceValue, (L7_uchar8*) &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue, sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_SbmState
*
* @purpose Get 'SbmState'
 *@description  [SbmState] The current state value of the Supplicant Backend
* state machine status parameter of the port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_SbmState (void *wap, void *bufp)
{

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSbmStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xSupplicantPortSbmStateGet(L7_UNIT_CURRENT, keyInterfaceValue, &objSbmStateValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SbmState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSbmStateValue, sizeof (objSbmStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_MacAuthBypass
*
* @purpose Get 'MacAuthBypass'
 *@description  [MacAuthBypass] <HTML>Get or Set the configured MAB value on the
* port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_MacAuthBypass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMacAuthBypassValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortMABEnabledGet (L7_UNIT_CURRENT, keyInterfaceValue, &objMacAuthBypassValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMacAuthBypassValue, sizeof (objMacAuthBypassValue));

  /* return the object value: MacAuthBypass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMacAuthBypassValue,
                           sizeof (objMacAuthBypassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securityDot1xSupplicantIntfConfig_MacAuthBypass
*
* @purpose Set 'MacAuthBypass'
 *@description  [MacAuthBypass] <HTML>Get or Set the configured MAB value on the
* port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securityDot1xSupplicantIntfConfig_MacAuthBypass (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objMacAuthBypassValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MacAuthBypass */
  owa.len = sizeof (objMacAuthBypassValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMacAuthBypassValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMacAuthBypassValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1xPortMABEnabledSet (L7_UNIT_CURRENT, keyInterfaceValue, objMacAuthBypassValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securityDot1xSupplicantIntfConfig_macAuthBypassOperMode
*
* @purpose Get 'macAuthBypassOperMode'
 *@description  [macAuthBypassOperMode] <HTML>Get the operational MAB value on
* the port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securityDot1xSupplicantIntfConfig_macAuthBypassOperMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacAuthBypassOperModeValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securityDot1xSupplicantIntfConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1xPortOperMABEnabledGet (L7_UNIT_CURRENT, keyInterfaceValue, &objmacAuthBypassOperModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmacAuthBypassOperModeValue, sizeof (objmacAuthBypassOperModeValue));

  /* return the object value: macAuthBypassOperMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacAuthBypassOperModeValue,
                           sizeof (objmacAuthBypassOperModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
