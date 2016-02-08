/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basesysImage.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  14 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_basesysImage_obj.h"
#include "usmdb_dim_api.h"

#ifdef L7_STACKING_PACKAGE
#include "usmdb_unitmgr_api.h"
#endif


/*******************************************************************************
* @function fpObjGet_basesysImage_sysUnitIndex
*
* @purpose Get 'sysUnitIndex'
 *@description  [sysUnitIndex] the unit number   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysUnitIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysUnitIndexValue;
  xLibU32_t nextObjsysUnitIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (objsysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & objsysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
#ifdef L7_STACKING_PACKAGE
    objsysUnitIndexValue = 0;
    owa.l7rc = usmDbUnitMgrStackMemberGetNext (objsysUnitIndexValue, &nextObjsysUnitIndexValue);
#else
    nextObjsysUnitIndexValue = 0;
    owa.l7rc = usmDbUnitMgrNumberGet(&nextObjsysUnitIndexValue);
#endif
   
  }
  else
  {
  
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objsysUnitIndexValue, owa.len);
#ifdef L7_STACKING_PACKAGE
    owa.l7rc = usmDbUnitMgrStackMemberGetNext (objsysUnitIndexValue, &nextObjsysUnitIndexValue);
#else
   owa.l7rc = L7_FAILURE;
#endif

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjsysUnitIndexValue, owa.len);

  /* return the object value: sysUnitIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjsysUnitIndexValue,
                           sizeof (nextObjsysUnitIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesysImage_sysImage1Version
*
* @purpose Get 'sysImage1Version'
 *@description  [sysImage1Version] the version information for a image1   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysImage1Version (void *wap, void *bufp)
{

  xLibU32_t keysysUnitIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysImage1VersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbImageVersionGet ( keysysUnitIndexValue,
                                USMDB_IMAGE1_NAME, objsysImage1VersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: sysImage1Version */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysImage1VersionValue,
                           strlen (objsysImage1VersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesysImage_sysImage2Version
*
* @purpose Get 'sysImage2Version'
 *@description  [sysImage2Version] the version information for a image2   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysImage2Version (void *wap, void *bufp)
{

  xLibU32_t keysysUnitIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysImage2VersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbImageVersionGet (keysysUnitIndexValue, 
                                USMDB_IMAGE2_NAME, objsysImage2VersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: sysImage2Version */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysImage2VersionValue,
                           strlen (objsysImage2VersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesysImage_sysImage1Descr
*
* @purpose Get 'sysImage1Descr'
 *@description  [sysImage1Descr] Image1 Description   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysImage1Descr (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysImage1DescrValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbImageDescrGet ( USMDB_IMAGE1_NAME,
                                             objsysImage1DescrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objsysImage1DescrValue, strlen (objsysImage1DescrValue));

  /* return the object value: sysImage1Descr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysImage1DescrValue,
                           strlen (objsysImage1DescrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesysImage_sysImage1Descr
*
* @purpose Set 'sysImage1Descr'
 *@description  [sysImage1Descr] Image1 Description   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysImage_sysImage1Descr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysImage1DescrValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysImage1Descr */
  owa.len = sizeof (objsysImage1DescrValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objsysImage1DescrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysImage1DescrValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
   owa.l7rc = usmDbImageDescrSet ( USMDB_IMAGE1_NAME,
                                                 objsysImage1DescrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basesysImage_sysImage2Descr
*
* @purpose Get 'sysImage2Descr'
 *@description  [sysImage2Descr] Image2 Description   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysImage2Descr (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysImage2DescrValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbImageDescrGet ( USMDB_IMAGE2_NAME,
                                             objsysImage2DescrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objsysImage2DescrValue, strlen (objsysImage2DescrValue));

  /* return the object value: sysImage2Descr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysImage2DescrValue,
                           strlen (objsysImage2DescrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesysImage_sysImage2Descr
*
* @purpose Set 'sysImage2Descr'
 *@description  [sysImage2Descr] Image2 Description   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysImage_sysImage2Descr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysImage2DescrValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysImage2Descr */
  owa.len = sizeof (objsysImage2DescrValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objsysImage2DescrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysImage2DescrValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbImageDescrSet ( USMDB_IMAGE2_NAME,
                                                 objsysImage2DescrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basesysImage_sysActiveImageName
*
* @purpose Get 'sysActiveImageName'
 *@description  [sysActiveImageName] Currently active image name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysActiveImageName (void *wap, void *bufp)
{

  xLibU32_t keysysUnitIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysActiveImageNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbActiveImageNameGet (keysysUnitIndexValue, objsysActiveImageNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: sysActiveImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysActiveImageNameValue,
                           strlen (objsysActiveImageNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
 * @function fpObjGet_basesysImage_sysActivatedImageName
 *
 * @purpose Get 'sysActivateedImageName'
  *@description  [sysActivatedImageName] Currently activated image name
 * @notes
 *
 * @return
 *******************************************************************************/
 xLibRC_t fpObjGet_basesysImage_sysActivatedImageName (void *wap, void *bufp)
 {

   xLibU32_t keysysUnitIndexValue;

   fpObjWa_t owa = FPOBJ_INIT_WA2 ();
   xLibStr256_t objsysActivatedImageNameValue;
   FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: sysUnitIndex */
   owa.len = sizeof (keysysUnitIndexValue);
   owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
   if (owa.rc != XLIBRC_SUCCESS)
   {
     owa.rc = XLIBRC_FILTER_MISSING;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }

   FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);
 
   /* get the value from application */
   owa.l7rc =
     usmDbActivatedImageNameGet (keysysUnitIndexValue, objsysActivatedImageNameValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }

   /* return the object value: sysActivatedImageName */
   owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysActivatedImageNameValue,
                            strlen (objsysActivatedImageNameValue));
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;

 }


/*******************************************************************************
* @function fpObjGet_basesysImage_sysBackupImageName
*
* @purpose Get 'sysBackupImageName'
 *@description  [sysBackupImageName] the backup image file name   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysBackupImageName (void *wap, void *bufp)
{

  xLibU32_t keysysUnitIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objsysBackupImageNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbBackupImageNameGet (keysysUnitIndexValue, objsysBackupImageNameValue);
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: sysBackupImageName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysBackupImageNameValue,
                           strlen (objsysBackupImageNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_basesysImage_sysDeleteBackupImage
*
* @purpose Set 'sysDeleteBackupImage'
 *@description  [sysDeleteBackupImage] Deletes BackupImage
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysImage_sysDeleteBackupImage (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysDeleteBackupImageValue;

  xLibU32_t keysysUnitIndexValue;
  xLibStr256_t keysysBackupImageNameValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysDeleteBackupImage */
  owa.len = sizeof (objsysDeleteBackupImageValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysDeleteBackupImageValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysDeleteBackupImageValue, owa.len);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* retrieve key: sysBackupImageName */
  owa.len = sizeof (keysysBackupImageNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysBackupImageName,
                          (xLibU8_t *) & keysysBackupImageNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysBackupImageNameValue, owa.len);


  /* set the value in application */
  owa.l7rc = usmDbImageDelete (keysysUnitIndexValue, keysysBackupImageNameValue);
  
  if(owa.l7rc == L7_NOT_EXIST)
  {
    owa.rc = XLIBRC_IMAGE_NOT_FOUND;
  }
  if(owa.l7rc == L7_REQUEST_DENIED)
  {
    owa.rc = XLIBRC_IMAGE_REQ_DENIED;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_IMAGE_NOT_FOUND;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjSet_basesysImage_sysActivateImage1
*
* @purpose Set 'sysActivateImage1'
 *@description  [sysActivateImage1] Make the image1 as active image.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysImage_sysActivateImage1 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysActivateImage1Value;

  xLibU32_t keysysUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysActivateImage1 */
  owa.len = sizeof (objsysActivateImage1Value);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysActivateImage1Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysActivateImage1Value, owa.len);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbImageActivate (keysysUnitIndexValue, USMDB_IMAGE1_NAME);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_IMAGE1ACTIVEFAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesysImage_sysDeleteImage1
*
* @purpose Set 'sysDeleteImage1'
 *@description  [sysDeleteImage1] Deletes Image1   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysImage_sysDeleteImage1 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysDeleteImage1Value;

  xLibU32_t keysysUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysDeleteImage1 */
  owa.len = sizeof (objsysDeleteImage1Value);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysDeleteImage1Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysDeleteImage1Value, owa.len);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbImageDelete (keysysUnitIndexValue, USMDB_IMAGE1_NAME);
  
  if(owa.l7rc == L7_NOT_EXIST)
  {
    owa.rc = XLIBRC_IMAGE_NOT_FOUND;
  }
  if(owa.l7rc == L7_REQUEST_DENIED)
  {
    owa.rc = XLIBRC_IMAGE_REQ_DENIED;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_IMAGE_NOT_FOUND;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesysImage_sysActivateImage2
*
* @purpose Set 'sysActivateImage2'
 *@description  [sysActivateImage2] Make the image2 as active image.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysImage_sysActivateImage2 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysActivateImage2Value;

  xLibU32_t keysysUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysActivateImage2 */
  owa.len = sizeof (objsysActivateImage2Value);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysActivateImage2Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysActivateImage2Value, owa.len);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbImageActivate (keysysUnitIndexValue, USMDB_IMAGE2_NAME);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_IMAGE2ACTIVEFAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesysImage_sysDeleteImage2
*
* @purpose Set 'sysDeleteImage2'
 *@description  [sysDeleteImage2] Deletes Image2   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysImage_sysDeleteImage2 (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysDeleteImage2Value;

  xLibU32_t keysysUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysDeleteImage2 */
  owa.len = sizeof (objsysDeleteImage2Value);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysDeleteImage2Value, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysDeleteImage2Value, owa.len);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbImageDelete (keysysUnitIndexValue, USMDB_IMAGE2_NAME);

  if(owa.l7rc == L7_NOT_EXIST)
  {
    owa.rc = XLIBRC_IMAGE_NOT_FOUND;
  }
  if(owa.l7rc == L7_REQUEST_DENIED)
  {
    owa.rc = XLIBRC_IMAGE_REQ_DENIED;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_IMAGE_NOT_FOUND;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_basesysImage_sysUpdateBootcode
*
* @purpose Set 'sysUpdateBootcode'
 *@description  [sysUpdateBootcode] Updates the Bootcode from the currently
* activated Image   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysImage_sysUpdateBootcode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysUpdateBootcodeValue;

  xLibU32_t keysysUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysUpdateBootcode */
  owa.len = sizeof (objsysUpdateBootcodeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysUpdateBootcodeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysUpdateBootcodeValue, owa.len);

    /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbBootCodeUpdate (keysysUnitIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}

/*******************************************************************************
* @function fpObjGet_basesysImage_BootImageVer
*
* @purpose Get 'BootImageVer'
 *@description  [BootImageVer] <HTML>Retrieves the boot image version details
* for a given image   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_BootImageVer (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objBootImageVerValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  memset(objBootImageVerValue,0x00,sizeof(objBootImageVerValue));
  owa.l7rc = usmDbBootImageVersionGet (L7_UNIT_CURRENT, RUNNING_IMAGE, objBootImageVerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objBootImageVerValue, strlen (objBootImageVerValue));

  /* return the object value: BootImageVer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objBootImageVerValue, strlen (objBootImageVerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesysImage_sysActiveImageVersion
*
* @purpose Get 'sysActiveImageVersion'
 *@description  [sysActiveImageVersion] the version information for active Image for a give unit index
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysActiveImageVersion (void *wap, void *bufp)
{

  xLibU32_t keysysUnitIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objActiveImageVersionValue;
  xLibStr6_t objsysActivatedImageNameValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

   /* get the value from application */
   owa.l7rc =
     usmDbActiveImageNameGet (keysysUnitIndexValue, objsysActivatedImageNameValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
   
  /* get the value from application */
      owa.l7rc = usmDbImageVersionGet (keysysUnitIndexValue, 
                                objsysActivatedImageNameValue, objActiveImageVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
   /* return the object value: sysActiveImageVersion : Default */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objActiveImageVersionValue, strlen ("0.0.0.0")); 
  }
  else
  {
      /* return the object value: sysActiveImageVersion */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objActiveImageVersionValue,
                           strlen (objActiveImageVersionValue));
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesysImage_sysBackupImageVersion
*
* @purpose Get 'sysBackupImageVersion'
 *@description  [sysBackupImageVersion] the version information for backup Image for a give unit index
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysBackupImageVersion (void *wap, void *bufp)
{

  xLibU32_t keysysUnitIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objBackupImageVersionValue;
  xLibStr6_t objsysActivatedImageNameValue;
  xLibStr6_t objsysBackupImageNameValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

   /* get the value from application */
   owa.l7rc =
     usmDbActiveImageNameGet (keysysUnitIndexValue, objsysActivatedImageNameValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }

   /* set Back Image Name */
  if( osapiStrncmp(objsysActivatedImageNameValue, "image1",  strlen("image1")))
  {
   osapiSnprintf(objsysBackupImageNameValue, strlen("image1"), "%s");
  }

  else
  {
 osapiSnprintf(objsysBackupImageNameValue, strlen("image2"), "%s");
  }

   /* get the value from application */
  owa.l7rc = usmDbImageVersionGet (keysysUnitIndexValue, 
                                objsysBackupImageNameValue, objBackupImageVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
   /* return the object value: sysBackupImageVersion : Default */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objBackupImageVersionValue, strlen ("0.0.0.0"));
  }
  else
  {
      /* return the object value: sysBackupImageVersion */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objBackupImageVersionValue,
                           strlen (objBackupImageVersionValue));
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_basesysImage_sysActivatedImageVersion
*
* @purpose Get 'sysActivatedImageVersion'
 *@description  [sysActivatedImageVersion] the version information for active Image for a give unit index
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysActivatedImageVersion (void *wap, void *bufp)
{

  xLibU32_t keysysUnitIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objActiveImageVersionValue;
  xLibStr6_t objsysActivatedImageNameValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);

   /* get the value from application */
   owa.l7rc =
     usmDbActivatedImageNameGet (keysysUnitIndexValue, objsysActivatedImageNameValue);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
   
  /* get the value from application */
      owa.l7rc = usmDbImageVersionGet (keysysUnitIndexValue, 
                                objsysActivatedImageNameValue, objActiveImageVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
   /* return the object value: sysActivatedImageVersion : Default */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objActiveImageVersionValue, strlen ("0.0.0.0")); 
  }
  else
  {
      /* return the object value: sysActivatedImageVersion */
      owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objActiveImageVersionValue,
                           strlen (objActiveImageVersionValue));
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_basesysImage_sysActivateImage
*
* @purpose Get 'sysActivateImage'
*
* @description sysActivateImage  
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysImage_sysActivateImage (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keysysUnitIndexValue;
  xLibU32_t objsysActivateImageValue;
  xLibStr256_t imagename;
 
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) & keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keysysUnitIndexValue, owa.len);
  
  /* get the value from application */
  owa.l7rc = 
       usmDbActivatedImageNameGet (keysysUnitIndexValue, imagename);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  if (!FPOBJ_CMP_STR256(imagename, USMDB_IMAGE1_NAME))
  {
      objsysActivateImageValue=L7_XUI_IMAGE1;
  }
  else
  {
      objsysActivateImageValue=L7_XUI_IMAGE2;
   }

  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysActivateImageValue,
                           sizeof (objsysActivateImageValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_basesysImage_sysActivateImage
*
* @purpose Set 'sysActivateImage'
 *@description  [sysActivateImage] <HTML>Activate sysActivateImage   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysImage_sysActivateImage (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objsysActivateImageValue;
  xLibU32_t keysysUnitIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysActivateImage */
  owa.len = sizeof (objsysActivateImageValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objsysActivateImageValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysActivateImageValue, owa.len);

  /* retrieve key: sysUnitIndex */
  owa.len = sizeof (keysysUnitIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_basesysImage_sysUnitIndex,
                          (xLibU8_t *) &keysysUnitIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keysysUnitIndexValue, owa.len);
  
  /* set the value in application */
  if(objsysActivateImageValue == L7_XUI_IMAGE1)
  {
    owa.l7rc = usmDbImageActivate (keysysUnitIndexValue, USMDB_IMAGE1_NAME);
  }
  if(objsysActivateImageValue == L7_XUI_IMAGE2)
  {
    owa.l7rc = usmDbImageActivate (keysysUnitIndexValue, USMDB_IMAGE2_NAME);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

