
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpLocalUserGroupAssociationTable.c
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
#include "_xe_captiveportalcpLocalUserGroupAssociationTable_obj.h"

#include "usmdb_cpdm_user_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationUserIndex
*
* @purpose Get 'cpLocalUserGroupAssociationUserIndex'
 *@description  [cpLocalUserGroupAssociationUserIndex] The local user entry
* index associated with a user group.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationUserIndex (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserGroupAssociationUserIndexValue;
  xLibU16_t nextObjcpLocalUserGroupAssociationUserIndexValue;
  xLibU16_t objcpLocalUserGroupAssociationGroupIndexValue;
  xLibU16_t nextObjcpLocalUserGroupAssociationGroupIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserGroupAssociationUserIndex */
  owa.len = sizeof (objcpLocalUserGroupAssociationUserIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationUserIndex,
                   (xLibU8_t *) & objcpLocalUserGroupAssociationUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpLocalUserGroupAssociationUserIndexValue = 0;
    objcpLocalUserGroupAssociationGroupIndexValue = 0;
    owa.l7rc = usmDbCpdmUserGroupAssocEntryNextGet (objcpLocalUserGroupAssociationUserIndexValue,
                    objcpLocalUserGroupAssociationGroupIndexValue, 
                    &nextObjcpLocalUserGroupAssociationUserIndexValue,
                    &nextObjcpLocalUserGroupAssociationGroupIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpLocalUserGroupAssociationUserIndexValue, owa.len);
    objcpLocalUserGroupAssociationGroupIndexValue = 0;
    do
    {
       owa.l7rc = usmDbCpdmUserGroupAssocEntryNextGet (objcpLocalUserGroupAssociationUserIndexValue,
                    objcpLocalUserGroupAssociationGroupIndexValue, 
                    &nextObjcpLocalUserGroupAssociationUserIndexValue,
                    &nextObjcpLocalUserGroupAssociationGroupIndexValue);
       objcpLocalUserGroupAssociationGroupIndexValue = nextObjcpLocalUserGroupAssociationGroupIndexValue;
    }
    while((objcpLocalUserGroupAssociationUserIndexValue == nextObjcpLocalUserGroupAssociationUserIndexValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpLocalUserGroupAssociationUserIndexValue, owa.len);

  /* return the object value: cpLocalUserGroupAssociationUserIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpLocalUserGroupAssociationUserIndexValue,
                           sizeof (nextObjcpLocalUserGroupAssociationUserIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationGroupIndex
*
* @purpose Get 'cpLocalUserGroupAssociationGroupIndex'
 *@description  [cpLocalUserGroupAssociationGroupIndex] The user group entry
* index associated with a local user.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationGroupIndex (void
                                                                                              *wap,
                                                                                              void
                                                                                              *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserGroupAssociationUserIndexValue;
  xLibU16_t nextObjcpLocalUserGroupAssociationUserIndexValue;
  xLibU32_t objcpLocalUserGroupAssociationGroupIndexValue;
  xLibU16_t nextObjcpLocalUserGroupAssociationGroupIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserGroupAssociationUserIndex */
  owa.len = sizeof (objcpLocalUserGroupAssociationUserIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationUserIndex,
                   (xLibU8_t *) & objcpLocalUserGroupAssociationUserIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpLocalUserGroupAssociationUserIndexValue, owa.len);

  /* retrieve key: cpLocalUserGroupAssociationGroupIndex */
  owa.len = sizeof (objcpLocalUserGroupAssociationGroupIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationGroupIndex,
                   (xLibU8_t *) & objcpLocalUserGroupAssociationGroupIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpLocalUserGroupAssociationGroupIndexValue = 0;

    owa.l7rc = usmDbCpdmUserGroupAssocEntryNextGet (objcpLocalUserGroupAssociationUserIndexValue,
                    objcpLocalUserGroupAssociationGroupIndexValue, 
                    &nextObjcpLocalUserGroupAssociationUserIndexValue,
                    &nextObjcpLocalUserGroupAssociationGroupIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpLocalUserGroupAssociationGroupIndexValue, owa.len);

    do
    {
       owa.l7rc = usmDbCpdmUserGroupAssocEntryNextGet (objcpLocalUserGroupAssociationUserIndexValue,
                       objcpLocalUserGroupAssociationGroupIndexValue, 
                       &nextObjcpLocalUserGroupAssociationUserIndexValue,
                       &nextObjcpLocalUserGroupAssociationGroupIndexValue);
       objcpLocalUserGroupAssociationUserIndexValue = nextObjcpLocalUserGroupAssociationUserIndexValue;
    }
    while ((objcpLocalUserGroupAssociationUserIndexValue == nextObjcpLocalUserGroupAssociationUserIndexValue) && (objcpLocalUserGroupAssociationGroupIndexValue == nextObjcpLocalUserGroupAssociationGroupIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if (!(objcpLocalUserGroupAssociationUserIndexValue == nextObjcpLocalUserGroupAssociationUserIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpLocalUserGroupAssociationGroupIndexValue, owa.len);

  /* return the object value: cpLocalUserGroupAssociationGroupIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpLocalUserGroupAssociationGroupIndexValue,
                           sizeof (nextObjcpLocalUserGroupAssociationGroupIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationRowStatus
*
* @purpose Get 'cpLocalUserGroupAssociationRowStatus'
 *@description  [cpLocalUserGroupAssociationRowStatus] The local user to user
* group association entry status.Supported values:active(1)      -
* valid entry   createAndGo(4) - creates a new entry      destroy(6) 
*    - removes an entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationRowStatus (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserGroupAssociationRowStatusValue;

  xLibU32_t keycpLocalUserGroupAssociationUserIndexValue;
  xLibU32_t keycpLocalUserGroupAssociationGroupIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpLocalUserGroupAssociationUserIndex */
  owa.len = sizeof (keycpLocalUserGroupAssociationUserIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationUserIndex,
                   (xLibU8_t *) & keycpLocalUserGroupAssociationUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserGroupAssociationUserIndexValue, owa.len);

  /* retrieve key: cpLocalUserGroupAssociationGroupIndex */
  owa.len = sizeof (keycpLocalUserGroupAssociationGroupIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationGroupIndex,
                   (xLibU8_t *) & keycpLocalUserGroupAssociationGroupIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserGroupAssociationGroupIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmUserGroupAssocEntryGet (keycpLocalUserGroupAssociationUserIndexValue,
                  keycpLocalUserGroupAssociationGroupIndexValue);

  if (owa.l7rc == L7_SUCCESS)
    objcpLocalUserGroupAssociationRowStatusValue = L7_ROW_STATUS_ACTIVE;
  else
    objcpLocalUserGroupAssociationRowStatusValue = L7_ROW_STATUS_INVALID;

  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserGroupAssociationRowStatusValue,
                     sizeof (objcpLocalUserGroupAssociationRowStatusValue));

  /* return the object value: cpLocalUserGroupAssociationRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpLocalUserGroupAssociationRowStatusValue,
                           sizeof (objcpLocalUserGroupAssociationRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationRowStatus
*
* @purpose Set 'cpLocalUserGroupAssociationRowStatus'
 *@description  [cpLocalUserGroupAssociationRowStatus] The local user to user
* group association entry status.Supported values:active(1)      -
* valid entry   createAndGo(4) - creates a new entry      destroy(6) 
*    - removes an entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationRowStatus (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpLocalUserGroupAssociationRowStatusValue;

  xLibU32_t keycpLocalUserGroupAssociationUserIndexValue;
  xLibU32_t keycpLocalUserGroupAssociationGroupIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: cpLocalUserGroupAssociationRowStatus */
  owa.len = sizeof (objcpLocalUserGroupAssociationRowStatusValue);
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objcpLocalUserGroupAssociationRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcpLocalUserGroupAssociationRowStatusValue, owa.len);

  /* retrieve key: cpLocalUserGroupAssociationUserIndex */
  owa.len = sizeof (keycpLocalUserGroupAssociationUserIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationUserIndex,
                   (xLibU8_t *) & keycpLocalUserGroupAssociationUserIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserGroupAssociationUserIndexValue, owa.len);

  /* retrieve key: cpLocalUserGroupAssociationGroupIndex */
  owa.len = sizeof (keycpLocalUserGroupAssociationGroupIndexValue);
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_captiveportalcpLocalUserGroupAssociationTable_cpLocalUserGroupAssociationGroupIndex,
                   (xLibU8_t *) & keycpLocalUserGroupAssociationGroupIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpLocalUserGroupAssociationGroupIndexValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (usmDbCpdmUserGroupAssocEntryGet(keycpLocalUserGroupAssociationUserIndexValue, keycpLocalUserGroupAssociationGroupIndexValue) != L7_SUCCESS)
  {
    if (objcpLocalUserGroupAssociationRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
        {
           owa.l7rc = usmDbCpdmUserGroupAssocEntryAdd (keycpLocalUserGroupAssociationUserIndexValue,
                           keycpLocalUserGroupAssociationGroupIndexValue);
        }
  }
  else if (objcpLocalUserGroupAssociationRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbCpdmUserGroupAssocEntryDelete (keycpLocalUserGroupAssociationUserIndexValue,
                                   keycpLocalUserGroupAssociationGroupIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
