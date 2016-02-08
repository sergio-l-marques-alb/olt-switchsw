/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseUsrMgrGlobal.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  12 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseUsrMgrGlobal_obj.h"
#include "usmdb_user_mgmt_api.h"
#include "user_manager_exports.h"

/*******************************************************************************
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementMinLength
*
* @purpose Get 'PasswordManagementMinLength'
*
* @description [PasswordManagementMinLength]: min password length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementMinLength (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU16_t));
  xLibU16_t objPasswordManagementMinLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbMinPassLengthGet (&objPasswordManagementMinLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinLengthValue,
                     sizeof (objPasswordManagementMinLengthValue));

  /* return the object value: PasswordManagementMinLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementMinLengthValue,
                    sizeof (objPasswordManagementMinLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementMinLength
*
* @purpose Set 'PasswordManagementMinLength'
*
* @description [PasswordManagementMinLength]: min password length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementMinLength (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementMinLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementMinLengthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinLengthValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbMinPasswdSet (L7_UNIT_CURRENT, objPasswordManagementMinLengthValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementHistory
*
* @purpose Get 'PasswordManagementHistory'
*
* @description [PasswordManagementHistory]: configures the history table size
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementHistory (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU16_t));
  xLibU16_t objPasswordManagementHistoryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbHistoryLengthGet ( &objPasswordManagementHistoryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementHistoryValue,
                     sizeof (objPasswordManagementHistoryValue));

  /* return the object value: PasswordManagementHistory */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementHistoryValue,
                    sizeof (objPasswordManagementHistoryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementHistory
*
* @purpose Set 'PasswordManagementHistory'
*
* @description [PasswordManagementHistory]: configures the history table size
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementHistory (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementHistoryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementHistory */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementHistoryValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementHistoryValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbPasswdHistoryLengthSet (L7_UNIT_CURRENT,objPasswordManagementHistoryValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementAging
*
* @purpose Get 'PasswordManagementAging'
*
* @description [PasswordManagementAging]: the password aging value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementAging (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU16_t));
  xLibU16_t objPasswordManagementAgingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbPasswdAgingValueGet ( &objPasswordManagementAgingValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementAgingValue,
                     sizeof (objPasswordManagementAgingValue));

  /* return the object value: PasswordManagementAging */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementAgingValue,
                           sizeof (objPasswordManagementAgingValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementAging
*
* @purpose Set 'PasswordManagementAging'
*
* @description [PasswordManagementAging]: the password aging value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementAging (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementAgingValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementAging */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementAgingValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementAgingValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbPasswdAgingSet (L7_UNIT_CURRENT, objPasswordManagementAgingValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementLockAttempts
*
* @purpose Get 'PasswordManagementLockAttempts'
*
* @description [PasswordManagementLockAttempts]: the number of user lockout
*              attempts 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementLockAttempts (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU16_t));
  xLibU16_t objPasswordManagementLockAttemptsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLockoutAttemptsGet (&objPasswordManagementLockAttemptsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementLockAttemptsValue,
                     sizeof (objPasswordManagementLockAttemptsValue));

  /* return the object value: PasswordManagementLockAttempts */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementLockAttemptsValue,
                    sizeof (objPasswordManagementLockAttemptsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementLockAttempts
*
* @purpose Set 'PasswordManagementLockAttempts'
*
* @description [PasswordManagementLockAttempts]: the number of user lockout
*              attempts 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementLockAttempts (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementLockAttemptsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementLockAttempts */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objPasswordManagementLockAttemptsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementLockAttemptsValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbPasswdsLockoutSet (L7_UNIT_CURRENT, objPasswordManagementLockAttemptsValue);
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
* @function fpObjSet_baseUsrMgrGlobal_ConsolePassword
* @purpose Set 'ConsolePassword'
 *@description  [ConsolePassword] password for console login.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_ConsolePassword (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objConsolePasswordValue;
  memset(objConsolePasswordValue, 0x00, sizeof(objConsolePasswordValue));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ConsolePassword */
  owa.len = sizeof (objConsolePasswordValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objConsolePasswordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objConsolePasswordValue, owa.len);


  /* set the value in application */
  owa.l7rc = usmDbLinePasswordSet(ACCESS_LINE_CONSOLE,
                             objConsolePasswordValue,
                             L7_FALSE);
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
* @function fpObjSet_baseUsrMgrGlobal_TelnetPassword
* @purpose Set 'TelnetPassword'
 *@description  [TelnetPassword] password for Telnet login.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_TelnetPassword (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objTelnetPasswordValue;
  memset(objTelnetPasswordValue, 0x00, sizeof(objTelnetPasswordValue));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TelnetPassword */
  owa.len = sizeof (objTelnetPasswordValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objTelnetPasswordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTelnetPasswordValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLinePasswordSet(ACCESS_LINE_TELNET,
                             objTelnetPasswordValue,
                             L7_FALSE);
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
* @function fpObjSet_baseUsrMgrGlobal_SShPassword
* @purpose Set 'SSHPassword'
 *@description  [SSHPassword] password for Enable login.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_SSHPassword (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objSSHPasswordValue;
  memset(objSSHPasswordValue, 0x00, sizeof(objSSHPasswordValue));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TelnetPassword */
  owa.len = sizeof (objSSHPasswordValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objSSHPasswordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objSSHPasswordValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLinePasswordSet(ACCESS_LINE_SSH,
                             objSSHPasswordValue,
                             L7_FALSE);
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
* @function fpObjSet_baseUsrMgrGlobal_EnablePassword
* @purpose Set 'EnablePassword'
 *@description  [EnablePassword] password for Enable login.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_EnablePassword (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objEnablePasswordValue;
  memset(objEnablePasswordValue, 0x00, sizeof(objEnablePasswordValue));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TelnetPassword */
  owa.len = sizeof (objEnablePasswordValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objEnablePasswordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objEnablePasswordValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbEnablePasswordSet(L7_USER_MGR_MAX_ACCESSLEVEL,
                             objEnablePasswordValue,
                             L7_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

