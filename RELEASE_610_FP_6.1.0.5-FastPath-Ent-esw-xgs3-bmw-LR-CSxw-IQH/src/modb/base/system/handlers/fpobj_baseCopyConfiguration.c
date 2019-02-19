
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseCopyConfiguration.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  10 October 2008, Friday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseCopyConfiguration_obj.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_sim_api.h"

/*******************************************************************************
* @function fpObjGet_baseCopyConfiguration_SourceFileName
*
* @purpose Get 'SourceFileName'
 *@description  [SourceFileName]  select source file name.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCopyConfiguration_SourceFileName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseCopyConfiguration_SourceFileName
*
* @purpose Set 'SourceFileName'
 *@description  [SourceFileName]  select source file name.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseCopyConfiguration_SourceFileName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseCopyConfiguration_DestinationFileName
*
* @purpose Get 'DestinationFileName'
 *@description  [DestinationFileName]  select destination file name.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseCopyConfiguration_DestinationFileName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */

  owa.rc = XLIBRC_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseCopyConfiguration_DestinationFileName
*
* @purpose Set 'DestinationFileName'
 *@description  [DestinationFileName]  select source file name.    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseCopyConfiguration_DestinationFileName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseCopyConfiguration_copyConfigFile
*
* @purpose Set 'copyConfigFile'
 *@description  [copyConfigFile]    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseCopyConfiguration_copyConfigFile (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcopyConfigFileValue;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keySourceFileNameValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDestinationFilenameValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objcopyConfigFileValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcopyConfigFileValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcopyConfigFileValue, owa.len);

	/* retrieve key: keySourceFileNameValue */
	kwa1.rc = xLibFilterGet (wap, XOBJ_baseCopyConfiguration_SourceFileName,
	                        (xLibU8_t *) & keySourceFileNameValue, &kwa1.len);
	if (kwa1.rc != XLIBRC_SUCCESS)
	{
	  kwa1.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
	  FPOBJ_TRACE_EXIT (bufp, kwa1);
	  return kwa1.rc;
	}
	FPOBJ_TRACE_CURRENT_KEY (bufp, &keySourceFileNameValue, kwa1.len);

		/* retrieve key: keyDestinationFilenameValue */
	kwa2.rc = xLibFilterGet (wap, XOBJ_baseCopyConfiguration_DestinationFileName,
	                        (xLibU8_t *) & keyDestinationFilenameValue, &kwa2.len);
	if (kwa2.rc != XLIBRC_SUCCESS)
	{
	  kwa2.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
	  FPOBJ_TRACE_EXIT (bufp, kwa2);
	  return kwa2.rc;
	}
	FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDestinationFilenameValue, kwa2.len);

      switch (keySourceFileNameValue)
      {
        case L7_XUI_RUNNING_CONFIG:
          switch (keyDestinationFilenameValue)
          {
            case L7_XUI_STARTUP_CONFIG:
              usmDbSwDevCtrlSaveConfigurationSet(L7_UNIT_CURRENT, L7_SAVE_CONFIG_TYPE_STARTUP);
              break;
            case L7_XUI_BACKUP_CONFIG:
              usmDbSwDevCtrlSaveConfigurationSet(L7_UNIT_CURRENT, L7_SAVE_CONFIG_TYPE_BACKUP);
              break;
            default:
              owa.rc = XLIBRC_INVALID_CONFIGFILE_TYPE;
				owa.l7rc = L7_FAILURE;
              break;
          }
          break;

        case L7_XUI_STARTUP_CONFIG:
          if (L7_XUI_BACKUP_CONFIG == keyDestinationFilenameValue)
          {
            if (osapiFsCopyFile(L7_SCRIPTS_STARTUP_CONFIG_FILENAME,
                                L7_SCRIPTS_BACKUP_CONFIG_FILENAME) == L7_SUCCESS)
            {
              usmDbTrapMgrCopyFinishedTrapSend();
            }
            else
            {
              usmDbTrapMgrCopyFailedTrapSend();
				owa.l7rc = L7_FAILURE;
            }
          }
			else
			{
			   owa.rc = XLIBRC_INVALID_CONFIGFILE_TYPE;
				owa.l7rc = L7_FAILURE;
			}
          break;

        case L7_XUI_BACKUP_CONFIG:
          if (L7_XUI_STARTUP_CONFIG == keyDestinationFilenameValue)
          {
            if (osapiFsCopyFile(L7_SCRIPTS_BACKUP_CONFIG_FILENAME,
                                L7_SCRIPTS_STARTUP_CONFIG_FILENAME) == L7_SUCCESS)
            {
              usmDbTrapMgrCopyFinishedTrapSend();
            }
            else
            {
              usmDbTrapMgrCopyFailedTrapSend();
				owa.l7rc = L7_FAILURE;
            }
          }
			else
			{
			   owa.rc = XLIBRC_INVALID_CONFIGFILE_TYPE;
				owa.l7rc = L7_FAILURE;
			}
          break;

        default:
          owa.rc = XLIBRC_INVALID_CONFIGFILE_TYPE;
			owa.l7rc = L7_FAILURE;
          break;
      }

  /* set the value in application */
	
  if (owa.l7rc != L7_SUCCESS)
  {
    if(owa.rc != XLIBRC_INVALID_CONFIGFILE_TYPE )
    	owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseCopyConfiguration_saveStartupConfigFile
*
* @purpose Set 'saveStartupConfigFile'
 *@description  [saveStartupConfigFile]    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseCopyConfiguration_saveStartupConfigFile (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSaveStartupConfigFileValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objSaveStartupConfigFileValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSaveStartupConfigFileValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSaveStartupConfigFileValue, owa.len);

  /* set the value in application */
	
  owa.l7rc =  usmDbSwDevCtrlSaveConfigurationSet(L7_UNIT_CURRENT, L7_SAVE_CONFIG_TYPE_STARTUP);;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseCopyConfiguration_saveStartupConfigFile
*
* @purpose Set 'saveStartupConfigFile'
 *@description  [saveStartupConfigFile]    
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseCopyConfiguration_saveBackupConfigFile (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSaveBackupConfigFileValue;

  FPOBJ_TRACE_ENTER (bufp);

  owa.len = sizeof (objSaveBackupConfigFileValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSaveBackupConfigFileValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSaveBackupConfigFileValue, owa.len);



  /* set the value in application */
	
  owa.l7rc =  usmDbSwDevCtrlSaveConfigurationSet(L7_UNIT_CURRENT, L7_SAVE_CONFIG_TYPE_BACKUP);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

