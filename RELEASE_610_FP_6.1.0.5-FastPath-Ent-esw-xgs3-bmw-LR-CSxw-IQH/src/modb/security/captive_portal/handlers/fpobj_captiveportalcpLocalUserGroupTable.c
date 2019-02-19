
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpLocalUserGroupTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to captiveportal-object.xml
*
* @create  06 July 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_captiveportalcpLocalUserGroupTable_obj.h"

#include "usmdb_cpdm_user_api.h"
#include "captive_portal_defaultconfig.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupIndex
*
* @purpose Get 'cpLocalUserGroupIndex'
 *@description  [cpLocalUserGroupIndex] The user group entry index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserGroupIndexValue;
  xLibU32_t nextObjcpLocalUserGroupIndexValue;
  xLibU16_t cpLocalUserGroupIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserGroupIndex */
  owa.len = sizeof (objcpLocalUserGroupIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserGroupTable_cpLocalUserGroupIndex,
                          (xLibU8_t *) & objcpLocalUserGroupIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpLocalUserGroupIndexValue = 0;
    owa.l7rc = usmDbCpdmUserGroupEntryNextGet (objcpLocalUserGroupIndexValue,
                    &cpLocalUserGroupIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpLocalUserGroupIndexValue, owa.len);
    owa.l7rc = usmDbCpdmUserGroupEntryNextGet (objcpLocalUserGroupIndexValue,
                    &cpLocalUserGroupIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  nextObjcpLocalUserGroupIndexValue = (xLibU32_t)cpLocalUserGroupIndexValue;

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpLocalUserGroupIndexValue, owa.len);

  /* return the object value: cpLocalUserGroupIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpLocalUserGroupIndexValue,
                           sizeof (nextObjcpLocalUserGroupIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupName
*
* @purpose Get 'cpLocalUserGroupName'
 *@description  [cpLocalUserGroupName] The user group name, limited to
* alpha-numeric strings (including the '-' and '_' characters).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpLocalUserGroupNameValue;

  xLibU32_t keycpLocalUserGroupIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserGroupIndex */
  owa.len = sizeof (keycpLocalUserGroupIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserGroupTable_cpLocalUserGroupIndex,
                          (xLibU8_t *) & keycpLocalUserGroupIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserGroupIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserGroupEntryNameGet (keycpLocalUserGroupIndexValue,
                              objcpLocalUserGroupNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objcpLocalUserGroupNameValue, strlen (objcpLocalUserGroupNameValue));

  /* return the object value: cpLocalUserGroupName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objcpLocalUserGroupNameValue,
                           strlen (objcpLocalUserGroupNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupName
*
* @purpose Set 'cpLocalUserGroupName'
 *@description  [cpLocalUserGroupName] The user group name, limited to
* alpha-numeric strings (including the '-' and '_' characters).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupName (void *wap, void *bufp)
{
 
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objcpLocalUserGroupNameValue;
  xLibStr256_t tmpGroupName;
 
  xLibU32_t keycpLocalUserGroupIndexValue;
  xLibU16_t cpLocalUserGroupId;
 
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve object: cpLocalUserGroupName */
  owa.len = sizeof (objcpLocalUserGroupNameValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objcpLocalUserGroupNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objcpLocalUserGroupNameValue, owa.len);
 
  /* retrieve key: cpLocalUserGroupIndex */
  owa.len = sizeof (keycpLocalUserGroupIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserGroupTable_cpLocalUserGroupIndex,
                          (xLibU8_t *) & keycpLocalUserGroupIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserGroupIndexValue, owa.len);
 
  if (L7_SUCCESS==usmDbCpdmUserGroupEntryByNameGet(objcpLocalUserGroupNameValue,&cpLocalUserGroupId)) 
  {
    FPOBJ_CLR_STR256(tmpGroupName);
    if (usmDbCpdmUserGroupEntryNameGet(keycpLocalUserGroupIndexValue, tmpGroupName) == L7_SUCCESS)
    {
       if (osapiStrncmp("",tmpGroupName,CP_USER_LOCAL_USERGROUP_MAX) == 0)
            usmDbCpdmUserGroupEntryDelete (keycpLocalUserGroupIndexValue);
    }
    owa.rc = XLIBRC_CP_FAILED_USER_GROUP_EXISTS;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  /* set the value in application */
  owa.l7rc = usmDbCpdmUserGroupEntryNameSet (keycpLocalUserGroupIndexValue,
                              objcpLocalUserGroupNameValue);
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
 
}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupRowStatus
*
* @purpose Get 'cpLocalUserGroupRowStatus'
 *@description  [cpLocalUserGroupRowStatus] The user group entry
* status.Supported values:  active(1)      - valid entry    createAndGo(4) -
* creates a new entry     destroy(6)     - removes an entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupRowStatus (void *wap,
                                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserGroupRowStatusValue;

  xLibU32_t keycpLocalUserGroupIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserGroupIndex */
  owa.len = sizeof (keycpLocalUserGroupIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserGroupTable_cpLocalUserGroupIndex,
                          (xLibU8_t *) & keycpLocalUserGroupIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserGroupIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserGroupEntryGet (keycpLocalUserGroupIndexValue);

  if (owa.l7rc == L7_SUCCESS)
    objcpLocalUserGroupRowStatusValue = L7_ROW_STATUS_ACTIVE;
  else
    objcpLocalUserGroupRowStatusValue = L7_ROW_STATUS_INVALID;

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserGroupRowStatusValue,
                     sizeof (objcpLocalUserGroupRowStatusValue));

  /* return the object value: cpLocalUserGroupRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserGroupRowStatusValue,
                           sizeof (objcpLocalUserGroupRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupRowStatus
*
* @purpose Set 'cpLocalUserGroupRowStatus'
 *@description  [cpLocalUserGroupRowStatus] The user group entry
* status.Supported values:  active(1)      - valid entry    createAndGo(4) -
* creates a new entry     destroy(6)     - removes an entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_captiveportalcpLocalUserGroupTable_cpLocalUserGroupRowStatus (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserGroupRowStatusValue;

  xLibU32_t keycpLocalUserGroupIndexValue;
  xLibStr256_t objcpLocalUserGroupNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserGroupRowStatus */
  owa.len = sizeof (objcpLocalUserGroupRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objcpLocalUserGroupRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserGroupRowStatusValue, owa.len);

  /* retrieve key: cpLocalUserGroupIndex */
  owa.len = sizeof (keycpLocalUserGroupIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserGroupTable_cpLocalUserGroupIndex,
                          (xLibU8_t *) & keycpLocalUserGroupIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserGroupIndexValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objcpLocalUserGroupRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
  /* retrieve group name from filters */
  owa.len = sizeof (objcpLocalUserGroupNameValue);
  FPOBJ_CLR_STR256(objcpLocalUserGroupNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_captiveportalcpLocalUserGroupTable_cpLocalUserGroupName,
                          (xLibU8_t *) objcpLocalUserGroupNameValue, &owa.len);

    /* Get next available group id */
    keycpLocalUserGroupIndexValue = GP_ID_MIN;
    while ((L7_SUCCESS==owa.l7rc) && (keycpLocalUserGroupIndexValue <= GP_ID_MAX))
    {
        owa.l7rc = usmDbCpdmUserGroupEntryGet(keycpLocalUserGroupIndexValue);
        if (L7_SUCCESS==owa.l7rc)
        {
          keycpLocalUserGroupIndexValue++;
        }
     }

    owa.l7rc = usmDbCpdmUserGroupEntryAdd (keycpLocalUserGroupIndexValue);

    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_CP_FAILED_TO_ADD_USER_GROUP;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    owa.rc = xLibFilterSet(wap,XOBJ_captiveportalcpLocalUserGroupTable_cpLocalUserGroupIndex,0,(xLibU8_t *) &keycpLocalUserGroupIndexValue,sizeof(keycpLocalUserGroupIndexValue));
  }
  else if (objcpLocalUserGroupRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* First, replace the 'to be' deleted group associations with the default group */
    usmDbCpdmUserGroupAssocEntryGroupRename(keycpLocalUserGroupIndexValue,GP_ID_MIN);
    
    if (usmDbCpdmUserGroupEntryDelete (keycpLocalUserGroupIndexValue) != L7_SUCCESS)
    {
        owa.rc = XLIBRC_CP_FAILED_TO_DELETE_USER_GROUP;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
