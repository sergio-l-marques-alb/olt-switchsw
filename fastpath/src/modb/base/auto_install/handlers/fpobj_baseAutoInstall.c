
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseAutoInstall.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  30 June 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseAutoInstall_obj.h"
#include "usmdb_auto_install_api.h"

/*******************************************************************************
* @function fpObjGet_baseAutoInstall_AutoInstallStartStop
*
* @purpose Get 'AutoInstallStartStop'
 *@description  [AutoInstallStartStop] Start/stop autoinstall procedure.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseAutoInstall_AutoInstallStartStop (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallStartStopValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbAutoInstallStartStopGet (&objAutoInstallStartStopValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallStartStopValue, sizeof (objAutoInstallStartStopValue));

  /* return the object value: AutoInstallStartStop */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoInstallStartStopValue,
                           sizeof (objAutoInstallStartStopValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseAutoInstall_AutoInstallStartStop
*
* @purpose Set 'AutoInstallStartStop'
 *@description  [AutoInstallStartStop] Start/stop autoinstall procedure.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseAutoInstall_AutoInstallStartStop (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallStartStopValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AutoInstallStartStop */
  owa.len = sizeof (objAutoInstallStartStopValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAutoInstallStartStopValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallStartStopValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbAutoInstallStartStopSet (objAutoInstallStartStopValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseAutoInstall_AutoInstallAutoSave
*
* @purpose Get 'AutoInstallAutoSave'
 *@description  [AutoInstallAutoSave] Enable or disable auto saving the
* installed configuration.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseAutoInstall_AutoInstallAutoSave (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallAutoSaveValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbAutoInstallAutoSaveGet (&objAutoInstallAutoSaveValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallAutoSaveValue, sizeof (objAutoInstallAutoSaveValue));

  /* return the object value: AutoInstallAutoSave */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoInstallAutoSaveValue,
                           sizeof (objAutoInstallAutoSaveValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseAutoInstall_AutoInstallAutoSave
*
* @purpose Set 'AutoInstallAutoSave'
 *@description  [AutoInstallAutoSave] Enable or disable auto saving the
* installed configuration.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseAutoInstall_AutoInstallAutoSave (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallAutoSaveValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AutoInstallAutoSave */
  owa.len = sizeof (objAutoInstallAutoSaveValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAutoInstallAutoSaveValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallAutoSaveValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbAutoInstallAutoSaveSet (objAutoInstallAutoSaveValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseAutoInstall_AutoInstallAutoImage
*
* @purpose Get 'AutoInstallAutoImage'
 *@description  [AutoInstallAutoImage] Get the auto image download status.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseAutoInstall_AutoInstallAutoImage (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallAutoImageValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbAutoInstallAutoImageGet (&objAutoInstallAutoImageValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallAutoImageValue, sizeof (objAutoInstallAutoImageValue));

  /* return the object value: AutoInstallAutoImage */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoInstallAutoImageValue,
                           sizeof (objAutoInstallAutoImageValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseAutoInstall_AutoInstallAutoImage
*
* @purpose Set 'AutoInstallAutoImage'
 *@description  [AutoInstallAutoImage] Enable or disable auto downloading image.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseAutoInstall_AutoInstallAutoImage (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallAutoImageValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AutoInstallAutoImage */
  owa.len = sizeof (objAutoInstallAutoImageValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAutoInstallAutoImageValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallAutoImageValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbAutoInstallAutoImageSet (objAutoInstallAutoImageValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseAutoInstall_AutoInstallAutoReboot
*
* @purpose Get 'AutoInstallAutoReboot'
 *@description  [AutoInstallAutoReboot] Get the auto reboot status.
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseAutoInstall_AutoInstallAutoReboot (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallAutoRebootValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbAutoInstallAutoRebootGet (&objAutoInstallAutoRebootValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallAutoRebootValue, sizeof (objAutoInstallAutoRebootValue));

  /* return the object value: AutoInstallAutoReboot */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoInstallAutoRebootValue,
                           sizeof (objAutoInstallAutoRebootValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseAutoInstall_AutoInstallAutoReboot
*
* @purpose Set 'AutoInstallAutoReboot'
 *@description  [AutoInstallAutoReboot] Enable or disable rebooting after successful
 * image download.     
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseAutoInstall_AutoInstallAutoReboot (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallAutoRebootValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AutoInstallAutoReboot */
  owa.len = sizeof (objAutoInstallAutoRebootValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAutoInstallAutoRebootValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallAutoRebootValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbAutoInstallAutoRebootSet (objAutoInstallAutoRebootValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseAutoInstall_AutoInstallCurrentStatus
*
* @purpose Get 'AutoInstallCurrentStatus'
 *@description  [AutoInstallCurrentStatus] Get the status of auto-install.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseAutoInstall_AutoInstallCurrentStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objAutoInstallCurrentStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbAutoInstallCurrentStatusGet (objAutoInstallCurrentStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objAutoInstallCurrentStatusValue,
                     strlen (objAutoInstallCurrentStatusValue));

  /* return the object value: AutoInstallCurrentStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAutoInstallCurrentStatusValue,
                           strlen (objAutoInstallCurrentStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseAutoInstall_AutoInstallFileDownloadRetryCount
*
* @purpose Set 'AutoInstallFileDownloadRetryCount'
*
* @description  [AutoInstallFileDownloadRetryCount] file Download retry count
*
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseAutoInstall_AutoInstallFileDownloadRetryCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallFileDownloadRetryCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AutoInstallSpecificFileRetryInifinite */
  owa.len = sizeof (objAutoInstallFileDownloadRetryCountValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAutoInstallFileDownloadRetryCountValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallFileDownloadRetryCountValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbAutoInstallFileDownLoadRetryCountSet (objAutoInstallFileDownloadRetryCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseAutoInstall_AutoInstallFileDownloadRetryCount
*
* @purpose Get 'AutoInstallFileDownloadRetryCount'
*
* @description  [AutoInstallFileDownloadRetryCount] file Download retry count
*
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseAutoInstall_AutoInstallFileDownloadRetryCount (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objAutoInstallFileDownloadRetryCountValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbAutoInstallFileDownLoadRetryCountGet (&objAutoInstallFileDownloadRetryCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAutoInstallFileDownloadRetryCountValue,
                     sizeof (objAutoInstallFileDownloadRetryCountValue));

  /* return the object value: AutoInstallFileDownloadRetryCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoInstallFileDownloadRetryCountValue,
                           sizeof (objAutoInstallFileDownloadRetryCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
