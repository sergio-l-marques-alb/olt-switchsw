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
  L7_ushort16 min_len;
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

  /*Get the min len from application*/
  owa.l7rc = usmDbMinPassLengthGet(&min_len);
  if(owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  owa.len--;
  if( ((owa.len > 0) && ((owa.len < min_len) || (owa.len > L7_MAX_PASSWORD_SIZE))) )
  {
      owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }


  /* set the value in application */
  owa.l7rc = usmDbLinePasswordSet(ACCESS_LINE_CONSOLE,
                             objConsolePasswordValue,
                             L7_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_PASSWORD_SET_FAILURE;    /* TODO: Change if required */
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
  L7_ushort16 min_len;
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

  /*Get the min len from application*/
  owa.l7rc = usmDbMinPassLengthGet(&min_len);
  if(owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  owa.len--;
  if( ((owa.len > 0) && ((owa.len < min_len) || (owa.len > L7_MAX_PASSWORD_SIZE))) )
  {
      owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbLinePasswordSet(ACCESS_LINE_TELNET,
                             objTelnetPasswordValue,
                             L7_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_PASSWORD_SET_FAILURE;    /* TODO: Change if required */
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
  L7_ushort16 min_len;
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

  /*Get the min len from application*/
  owa.l7rc = usmDbMinPassLengthGet(&min_len);
  if(owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  owa.len--;
  if( ((owa.len > 0) && ((owa.len < min_len) || (owa.len > L7_MAX_PASSWORD_SIZE))) )
  {
      owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbLinePasswordSet(ACCESS_LINE_SSH,
                             objSSHPasswordValue,
                             L7_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_PASSWORD_SET_FAILURE;    /* TODO: Change if required */
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
  L7_ushort16 min_len;

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

  /* set the value in application */
  owa.l7rc = usmDbMinPassLengthGet(&min_len);
  if(owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  owa.len--;
  if( ((owa.len > 0) && ((owa.len < min_len) || (owa.len > L7_MAX_PASSWORD_SIZE))) )
  {
      owa.rc = XLIBRC_PASSWORD_SET_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbEnablePasswordSet(L7_USER_MGR_MAX_ACCESSLEVEL,
                             objEnablePasswordValue,
                             L7_FALSE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_PASSWORD_SET_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/**************************** STRONG PASSWORD *********************************/
/*******************************************************************************
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementPasswdStrengthCheck
*
* @purpose Get 'PasswordManagementPasswdStrengthCheck'
*
* @description Password Strength Check feature
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementPasswdStrengthCheck (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU16_t));
  xLibU32_t objPasswordManagementPasswdStrengthCheckValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswordStrengthCheckConfigGet (&objPasswordManagementPasswdStrengthCheckValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementPasswdStrengthCheckValue,
                     sizeof (objPasswordManagementPasswdStrengthCheckValue));

  /* return the object value: PasswordManagementPasswdStrengthCheck */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementPasswdStrengthCheckValue,
                    sizeof (objPasswordManagementPasswdStrengthCheckValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementPasswdStrengthCheck
*
* @purpose Set 'PasswordManagementPasswdStrengthCheck'
*
* @description [PasswordManagementPasswdStrengthCheck]: password strength feature 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementPasswdStrengthCheck(void *wap,
                                                                         void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementPasswdStrengthCheckValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementPasswdStrengthCheck */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementPasswdStrengthCheckValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementPasswdStrengthCheckValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUserMgrPasswordStrengthCheckConfigSet (objPasswordManagementPasswdStrengthCheckValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementMinUpperCaseCharLength
*
* @purpose Get 'PasswordManagementMinUcaseCharLength'
*
* @description [PasswordManagementMinUcaseCharLength]: min password UCASE Char 
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementMinUpperCaseCharLength (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU32_t objPasswordManagementMinUCaseLengthValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU8_t  val;

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswdMinUppercaseCharsLengthGet (&val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objPasswordManagementMinUCaseLengthValue = val;
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinUCaseLengthValue,
                     sizeof (objPasswordManagementMinUCaseLengthValue));

  /* return the object value: PasswordManagementMinLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementMinUCaseLengthValue,
                    sizeof (objPasswordManagementMinUCaseLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementMinUppercaseCharLength
*
* @purpose Set 'PasswordManagementMinUpperCaseCharLength'
*
* @description [PasswordManagementMinUpperCaseCharLength]: min password LCASE
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementMinUpperCaseCharLength (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinUCaseLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementMinUCaseLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementMinUCaseLengthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinUCaseLengthValue, owa.len);

  /* set the value in application */
  owa.l7rc =
   usmDbUserMgrPasswdMinUppercaseCharsLengthSet(objPasswordManagementMinUCaseLengthValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementMinLowerCaseCharLength
*
* @purpose Get 'PasswordManagementMinLcaseCharLength'
*
* @description [PasswordManagementMinLcaseCharLength]: min password LCASE Char 
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementMinLowerCaseCharLength (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinLCaseLengthValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU8_t  val;

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswdMinLowercaseCharsLengthGet (&val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objPasswordManagementMinLCaseLengthValue = val;
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinLCaseLengthValue,
                     sizeof (objPasswordManagementMinLCaseLengthValue));

  /* return the object value: PasswordManagementMinLCASELength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementMinLCaseLengthValue,
                    sizeof (objPasswordManagementMinLCaseLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementMinLowercaseCharLength
*
* @purpose Set 'PasswordManagementMinLowerCaseCharLength'
*
* @description [PasswordManagementMinLowerCaseCharLength]: min password LCASE
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementMinLowerCaseCharLength (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinLCaseLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementMinLCaseLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementMinLCaseLengthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinLCaseLengthValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUserMgrPasswdMinLowercaseCharsLengthSet(objPasswordManagementMinLCaseLengthValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementMinNumericCharLength
*
* @purpose Get 'PasswordManagementMinNumericCharLength'
*
* @description [PasswordManagementMinNumericCharLength]: min password Numeric Char 
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementMinNumericCharLength (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinNumCharLengthValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU8_t  val;

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswdMinNumericCharsLengthGet (&val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objPasswordManagementMinNumCharLengthValue = val; 
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinNumCharLengthValue,
                     sizeof (objPasswordManagementMinNumCharLengthValue));

  /* return the object value: PasswordManagementMinNumCharLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementMinNumCharLengthValue,
                    sizeof (objPasswordManagementMinNumCharLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementMinNumericCharLength
*
* @purpose Set 'PasswordManagementMinNumericCharLength'
*
* @description [PasswordManagementMinNumericCharLength]: min password Numeric
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementMinNumericCharLength (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinNumCharLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementMinNumCharLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementMinNumCharLengthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinNumCharLengthValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUserMgrPasswdMinNumericCharsLengthSet(objPasswordManagementMinNumCharLengthValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementMinSplCharLength
*
* @purpose Get 'PasswordManagementMinSplCharLength'
*
* @description [PasswordManagementMinSplCharLength]: min password Special Char 
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementMinSpecialCharLength (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinSplCharLengthValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU8_t  val;

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswdMinSpecialCharsLengthGet (&val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objPasswordManagementMinSplCharLengthValue = val; 
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinSplCharLengthValue,
                     sizeof (objPasswordManagementMinSplCharLengthValue));

  /* return the object value: PasswordManagementMinSplCharLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementMinSplCharLengthValue,
                    sizeof (objPasswordManagementMinSplCharLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementMinSpecialCharLength
*
* @purpose Set 'PasswordManagementMinSplCharLength'
*
* @description [PasswordManagementMinSplCharLength]: min password Special char
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementMinSpecialCharLength (void *wap,
                                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinSplCharLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementMinSplCharLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementMinSplCharLengthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinSplCharLengthValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUserMgrPasswdMinSpecialCharsLengthSet(objPasswordManagementMinSplCharLengthValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementMaxConsCharLength
*
* @purpose Get 'PasswordManagementMaxConsCharLength'
*
* @description [PasswordManagementMinSplCharLength]: max password Consecutive Char 
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementMaxConsecutiveCharLength (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMaxConsCharLengthValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU8_t  val;

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswdMaxConsecutiveCharsLengthGet (&val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objPasswordManagementMaxConsCharLengthValue = val;
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMaxConsCharLengthValue,
                     sizeof (objPasswordManagementMaxConsCharLengthValue));

  /* return the object value: PasswordManagementMaxConsCharLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementMaxConsCharLengthValue,
                    sizeof (objPasswordManagementMaxConsCharLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementMaxConsCharLength
*
* @purpose Set 'PasswordManagementMaxConsCharLength'
*
* @description [PasswordManagementMaxConsCharLength]: max consecutive char
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementMaxConsecutiveCharLength (void *wap,
                                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMaxConsCharLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementMaxConsCharLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementMaxConsCharLengthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMaxConsCharLengthValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUserMgrPasswdMaxConsecutiveCharsLengthSet(objPasswordManagementMaxConsCharLengthValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementMaxRepeatedCharLength
*
* @purpose Get 'PasswordManagementMaxRepeatedCharLength'
*
* @description [PasswordManagementMinRepeatedCharLength]: max password Repeated Char 
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementMaxRepeatedCharLength (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMaxRepeatedCharLengthValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU8_t  val;

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswdMaxRepeatedCharsLengthGet (&val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objPasswordManagementMaxRepeatedCharLengthValue = val;
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMaxRepeatedCharLengthValue,
                     sizeof (objPasswordManagementMaxRepeatedCharLengthValue));

  /* return the object value: PasswordManagementMaxRepeatedCharLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementMaxRepeatedCharLengthValue,
                    sizeof (objPasswordManagementMaxRepeatedCharLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementMaxRepeatedCharLength
*
* @purpose Set 'PasswordManagementMaxRepeatedCharLength'
*
* @description [PasswordManagementMaxRepeatedCharLength]: max repeated char
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementMaxRepeatedCharLength (void *wap,
                                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMaxRepeatedCharLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementMaxRepeatedCharLength */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementMaxRepeatedCharLengthValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMaxRepeatedCharLengthValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUserMgrPasswdMaxRepeatedCharsLengthSet(objPasswordManagementMaxRepeatedCharLengthValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementMinCharacterClasses
*
* @purpose Get 'PasswordManagementMinCharacterClasses'
*
* @description [PasswordManagementMinCharacterClasses]: min password Special Char 
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementMinCharacterClasses (void *wap,
                                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinCharacterClassesValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU8_t val;

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswdMinCharacterClassesGet (&val); 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objPasswordManagementMinCharacterClassesValue = val;
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinCharacterClassesValue,
                     sizeof (objPasswordManagementMinCharacterClassesValue));

  /* return the object value: PasswordManagementMinCharacterClasses */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementMinCharacterClassesValue,
                    sizeof (objPasswordManagementMinCharacterClassesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementMinCharacterClasses
*
* @purpose Set 'PasswordManagementMinCharacterClasses'
*
* @description [PasswordManagementMinCharacterClasses]: min password Special char
*              length value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementMinCharacterClasses (void *wap,
                                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementMinCharacterClassesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementMinCharacterClasses */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementMinCharacterClassesValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementMinCharacterClassesValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbUserMgrPasswdMinCharacterClassesSet(objPasswordManagementMinCharacterClassesValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementExcludeKeyword
*
* @purpose Get 'PasswordManagementExcludeKeyword'
*
* @description [PasswordManagementExcludeKeyword]: exclude keyword  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementExcludeKeyword (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPasswordManagementExcludeKeywordValue;
  xLibStr256_t nextObjPasswordManagementExcludeKeywordValue;
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objPasswordManagementExcludeKeywordValue);
  owa.len = sizeof (nextObjPasswordManagementExcludeKeywordValue);
  
  memset(objPasswordManagementExcludeKeywordValue, 0x00, sizeof(objPasswordManagementExcludeKeywordValue));
  memset(nextObjPasswordManagementExcludeKeywordValue, 0x00, sizeof(nextObjPasswordManagementExcludeKeywordValue));

  /* retrieve key: Exclude Keyword */
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrGlobal_PasswordManagementExcludeKeyword,
                          (xLibU8_t *) objPasswordManagementExcludeKeywordValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(nextObjPasswordManagementExcludeKeywordValue, 0x00, sizeof(nextObjPasswordManagementExcludeKeywordValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objPasswordManagementExcludeKeywordValue, owa.len);
    osapiStrncpy(nextObjPasswordManagementExcludeKeywordValue, objPasswordManagementExcludeKeywordValue,
                            sizeof(objPasswordManagementExcludeKeywordValue));
  }

  owa.l7rc = usmDbUserMgrPasswdExcludeKeywordNextGet(nextObjPasswordManagementExcludeKeywordValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPasswordManagementExcludeKeywordValue, owa.len);
 
  owa.len = osapiStrnlen(nextObjPasswordManagementExcludeKeywordValue, sizeof(nextObjPasswordManagementExcludeKeywordValue));

  /* return the object value: nextObjPasswordManagementExcludeKeywordValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjPasswordManagementExcludeKeywordValue, owa.len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementExcludeKeywordStatus
*
* @purpose Get 'PasswordManagementExcludeKeywordStatus'
*
* @description [PasswordManagementExcludeKeywordStatus]: Status 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementExcludeKeywordStatus (void *wap,
                                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementExcludeKeywordStatusValue;
  xLibStr256_t keyPasswordManagementExcludeKeyword;
  xLibStr256_t keyword;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Keyword */
  owa.len = sizeof (keyPasswordManagementExcludeKeyword);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrGlobal_PasswordManagementExcludeKeyword,
                          (xLibU8_t *) keyPasswordManagementExcludeKeyword, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  memset(keyword, 0x00, sizeof(keyword));
  do
  {
    if((owa.l7rc = usmDbUserMgrPasswdExcludeKeywordNextGet(keyword)) == L7_SUCCESS)
    {
      if(osapiStrncmp(keyword, keyPasswordManagementExcludeKeyword, sizeof(keyword)) == 0)
      {
        objPasswordManagementExcludeKeywordStatusValue = L7_ROW_STATUS_ACTIVE;
        break;
      }
    }
  }while(owa.l7rc == L7_SUCCESS);

  if (owa.l7rc != L7_SUCCESS)
  {
    objPasswordManagementExcludeKeywordStatusValue = L7_ROW_STATUS_INVALID;
  }

  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementExcludeKeywordStatusValue,
                     sizeof (objPasswordManagementExcludeKeywordStatusValue));

  /* return the object value: PasswordManagementExcludeKeywordStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) &objPasswordManagementExcludeKeywordStatusValue,
                    sizeof(objPasswordManagementExcludeKeywordStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementExcludeKeywordStatus
*
* @purpose Set 'PasswordManagementExcludeKeywordStatus'
*
* @description [PasswordManagementExcludeKeywordStatus]: exclude keyword Status
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementExcludeKeywordStatus (void *wap,
                                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPasswordManagementExcludeKeywordStatusValue;
  xLibStr256_t keyPasswordManagementExcludeKeyword;
  FPOBJ_TRACE_ENTER (bufp);

  memset(keyPasswordManagementExcludeKeyword, 0x00, sizeof(keyPasswordManagementExcludeKeyword));

  /* retrieve object: PasswordManagementExcludeKeyword */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &objPasswordManagementExcludeKeywordStatusValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementExcludeKeywordStatusValue, owa.len);

  /* retrieve key: Keyword */
  owa.len = sizeof (keyPasswordManagementExcludeKeyword);
  owa.rc = xLibFilterGet (wap, XOBJ_baseUsrMgrGlobal_PasswordManagementExcludeKeyword,
                          (xLibU8_t *) keyPasswordManagementExcludeKeyword, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, keyPasswordManagementExcludeKeyword, owa.len);

  /* set the value in application */
  owa.l7rc = L7_FAILURE;
  if (objPasswordManagementExcludeKeywordStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbUserMgrPasswdExcludeKeywordSet(keyPasswordManagementExcludeKeyword);
    if(owa.l7rc != L7_SUCCESS)
    {
      switch(owa.l7rc)
      {
        case L7_USER_MGR_PASSWD_EXCLUDE_KEYWORD_ALREADY_EXIST:
             owa.rc = XLIBRC_USERMGR_PASSWD_EXCLUDE_KEYWORD_ALREADY_EXIST;
             break;
        case L7_TABLE_IS_FULL:
             owa.rc = XLIBRC_USERMGR_PASSWD_EXCEED_MAX_KEYWORDS;
             break;
        case L7_USER_MGR_PASSWD_INVALID_QUOTATION_CHAR:
             owa.rc = XLIBRC_USERMGR_PASSWD_EXCLUDE_KEYWORD_INVALID_QUOTATION_SPECIAL_CHAR;
             break;
        case L7_USER_MGR_PASSWD_INVALID_QUESTIONMARK_CHAR: 
             owa.rc = XLIBRC_USERMGR_PASSWD_EXCLUDE_KEYWORD_INVALID_QUESTIONMARK_SPECIAL_CHAR;
             break;
        default:
             owa.rc = XLIBRC_FAILURE; 
      } 
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if(objPasswordManagementExcludeKeywordStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbUserMgrPasswdExcludeKeywordDelete(keyPasswordManagementExcludeKeyword);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
    
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementStrengthScore
*
* @purpose Get 'PasswordManagementScore'
*
* @description [PasswordManagementScore]: Password Score Value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementStrengthScore (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU32_t objPasswordManagementScoreValue;
  FPOBJ_TRACE_ENTER (bufp);
  xLibU8_t  val;

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswdStrengthScoreGet (&val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objPasswordManagementScoreValue = val;
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementScoreValue,
                     sizeof (objPasswordManagementScoreValue));

  /* return the object value: PasswordManagementMinLength */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objPasswordManagementScoreValue,
                    sizeof (objPasswordManagementScoreValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjSet_baseUsrMgrGlobal_PasswordManagementStrengthScore
*
* @purpose Set 'PasswordManagementScore'
*
* @description [PasswordManagementStrengthScore]: password strength Score
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseUsrMgrGlobal_PasswordManagementStrengthScore (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU32_t objPasswordManagementScoreValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PasswordManagementScore */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objPasswordManagementScoreValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPasswordManagementScoreValue, owa.len);

  /* set the value in application */
  owa.l7rc =
   usmDbUserMgrPasswdStrengthScoreSet(objPasswordManagementScoreValue);
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
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementLastPasswordSetResult
*
* @purpose Get 'PasswordManagementLastPasswordSetResult'
*
* @description [PasswordManagementLastPasswordSetResult]: Last Password Set Result 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementLastPasswordSetResult (void *wap,
                                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPasswordManagementLastPasswordSetResultValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbUserMgrPasswordLastSetResultStringGet (objPasswordManagementLastPasswordSetResultValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objPasswordManagementLastPasswordSetResultValue,
                     sizeof (objPasswordManagementLastPasswordSetResultValue));

  /* return the object value: PasswordManagementLastPasswordSetResultValue */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objPasswordManagementLastPasswordSetResultValue,
                    strlen(objPasswordManagementLastPasswordSetResultValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseUsrMgrGlobal_PasswordManagementIsExcludeKeywordMaxLimit
*
* @purpose Get 'PasswordManagementIsExcludeKeywordMaxLimit'
*
* @description [PasswordManagementIsExcludeKeywordMaxLimit]: return the boolean
*               TRUE for max limit
*
* @return 
*******************************************************************************/
xLibRC_t fpObjGet_baseUsrMgrGlobal_PasswordManagementIsExcludeKeywordMaxLimit (void *wap,
                                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objPasswordManagementIsExcludeKeywordMaxLimitValue;
  xLibStr256_t excludeKeyword;
  xLibU32_t count=0;
  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objPasswordManagementIsExcludeKeywordMaxLimitValue);
  
  memset(excludeKeyword, 0x00, sizeof(excludeKeyword));

  do
  {
    owa.l7rc = usmDbUserMgrPasswdExcludeKeywordNextGet(excludeKeyword);
    if (owa.l7rc == L7_SUCCESS)
    {
      count++;
    }
  }while(owa.l7rc == L7_SUCCESS);

  objPasswordManagementIsExcludeKeywordMaxLimitValue = (count == L7_PASSWD_EXCLUDE_KEYWORDS_MAX+1)?L7_TRUE:L7_FALSE;
  
  FPOBJ_TRACE_NEW_KEY (bufp, &objPasswordManagementIsExcludeKeywordMaxLimitValue, owa.len);
 
  /* return the object value: objPasswordManagementIsExcludeKeywordMaxLimitValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objPasswordManagementIsExcludeKeywordMaxLimitValue, owa.len);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


