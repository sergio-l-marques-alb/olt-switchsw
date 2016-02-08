/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_securitySSHClient.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ssh-object.xml
*
* @create  14 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_securitySSHClient_obj.h"
#include "usmdb_sshc_api.h"
#include "sshd_exports.h"

/*******************************************************************************
* @function fpObjGet_securitySSHClient_TransferRemoteUsername
*
* @purpose Get 'TransferRemoteUsername'
*
* @description [TransferRemoteUsername]: username of remote host 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHClient_TransferRemoteUsername (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTransferRemoteUsernameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSshcTransferRemoteUsernameGet (L7_UNIT_CURRENT,
                                        objTransferRemoteUsernameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTransferRemoteUsernameValue,
                     strlen (objTransferRemoteUsernameValue));

  /* return the object value: TransferRemoteUsername */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTransferRemoteUsernameValue,
                           strlen (objTransferRemoteUsernameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSHClient_TransferRemoteUsername
*
* @purpose Set 'TransferRemoteUsername'
*
* @description [TransferRemoteUsername]: username of remote host 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHClient_TransferRemoteUsername (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTransferRemoteUsernameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TransferRemoteUsername */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objTransferRemoteUsernameValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTransferRemoteUsernameValue, owa.len);

  if(strlen(objTransferRemoteUsernameValue) > L7_SSHC_USERNAME_SIZE_MAX)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* set the value in application */
  owa.l7rc =
    usmDbSshcTransferRemoteUsernameSet (L7_UNIT_CURRENT,
                                        objTransferRemoteUsernameValue);
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
* @function fpObjGet_securitySSHClient_TransferRemotePassword
*
* @purpose Get 'TransferRemotePassword'
*
* @description [TransferRemotePassword]: password of remote host 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitySSHClient_TransferRemotePassword (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTransferRemotePasswordValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSshcTransferRemotePasswordGet (L7_UNIT_CURRENT,
                                        objTransferRemotePasswordValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTransferRemotePasswordValue,
                     strlen (objTransferRemotePasswordValue));

  /* return the object value: TransferRemotePassword */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTransferRemotePasswordValue,
                           strlen (objTransferRemotePasswordValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitySSHClient_TransferRemotePassword
*
* @purpose Set 'TransferRemotePassword'
*
* @description [TransferRemotePassword]: password of remote host 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitySSHClient_TransferRemotePassword (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTransferRemotePasswordValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TransferRemotePassword */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objTransferRemotePasswordValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objTransferRemotePasswordValue, owa.len);

    if(strlen(objTransferRemotePasswordValue) > L7_SSHC_PASSWORD_SIZE_MAX)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc =
    usmDbSshcTransferRemotePasswordSet (L7_UNIT_CURRENT,
                                        objTransferRemotePasswordValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
