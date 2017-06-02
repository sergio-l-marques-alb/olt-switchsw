
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_securitymgmtacal.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to mgmtacal-object.xml
*
* @create  07 October 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_securitymgmtacal_obj.h"

#include "usmdb_macal_api.h"

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalListName
*
* @purpose Get 'macalListName'
 *@description  [macalListName] <HTML>To create or manage a new management ACAL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalListName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objmacalListNameValue;
  /*xLibStr256_t nextObjmacalListNameValue;*/

  FPOBJ_TRACE_ENTER (bufp);

  memset(objmacalListNameValue,0x00,sizeof(objmacalListNameValue));

  /* retrieve key: macalListName */
  owa.len = sizeof (objmacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) objmacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objmacalListNameValue, owa.len);
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;

  }

  owa.l7rc = usmDbMacalNameGet (objmacalListNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, objmacalListNameValue, owa.len);

  /* return the object value: macalListName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmacalListNameValue,
                           strlen (objmacalListNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalListPrioriry
*
* @purpose Get 'macalListPrioriry'
 *@description  [macalListPrioriry] <HTML>Get the  ACAL rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalListPrioriry (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalListPrioriryValue;
  xLibU32_t nextObjmacalListPrioriryValue;

  xLibStr256_t keymacalListNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (objmacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & objmacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbMacalRuleFirstGet (keymacalListNameValue,&nextObjmacalListPrioriryValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmacalListPrioriryValue, owa.len);
    owa.l7rc = usmDbMacalRuleNextGet(keymacalListNameValue,
                                    objmacalListPrioriryValue, &nextObjmacalListPrioriryValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmacalListPrioriryValue, owa.len);

  /* return the object value: macalListPrioriry */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmacalListPrioriryValue,
                           sizeof (nextObjmacalListPrioriryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalListRowStatus
*
* @purpose Get 'macalListRowStatus'
 *@description  [macalListRowStatus] <HTML>To add or Delete an ACAL rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalListRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalListRowStatusValue;

  xLibStr256_t keymacalListNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keymacalListNameValue, &objmacalListRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmacalListRowStatusValue, sizeof (objmacalListRowStatusValue));

  /* return the object value: macalListRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacalListRowStatusValue,
                           sizeof (objmacalListRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitymgmtacal_macalListRowStatus
*
* @purpose Set 'macalListRowStatus'
 *@description  [macalListRowStatus] <HTML>To add or Delete an ACAL rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitymgmtacal_macalListRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalListRowStatusValue;

  xLibStr256_t keymacalListNameValue;
  xLibStr256_t macalListNameValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macalListRowStatus */
  owa.len = sizeof (objmacalListRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmacalListRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmacalListRowStatusValue, owa.len);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objmacalListRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbMacalListCreate (keymacalListNameValue);
  }
  else if (objmacalListRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
     /* get the value from application */
     owa.l7rc =  usmDbMacalActiveListGet(macalListNameValue);
     if(owa.l7rc == L7_SUCCESS)
     {
         if ( strcmp(macalListNameValue,keymacalListNameValue) == L7_NULL )
         {
              owa.rc = XLIBRC_CURRENTLY_ACTIVE;    /* TODO: Change if required */
	       return owa.rc;
         }
     }
    owa.l7rc = usmDbMacalListDelete (keymacalListNameValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalRuleRowStatus
*
* @purpose Get 'macalRuleRowStatus'
 *@description  [macalRuleRowStatus] <HTML>To add or Delete an ACAL rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalRuleRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleRowStatusValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keymacalListNameValue,
                              keymacalListPrioriryValue, &objmacalRuleRowStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleRowStatusValue, sizeof (objmacalRuleRowStatusValue));

  /* return the object value: macalRuleRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacalRuleRowStatusValue,
                           sizeof (objmacalRuleRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitymgmtacal_macalRuleRowStatus
*
* @purpose Set 'macalRuleRowStatus'
 *@description  [macalRuleRowStatus] <HTML>To add or Delete an ACAL rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitymgmtacal_macalRuleRowStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleRowStatusValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;
  xLibStr256_t macalListNameValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macalRuleRowStatus */
  owa.len = sizeof (objmacalRuleRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmacalRuleRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleRowStatusValue, owa.len);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objmacalRuleRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbMacalRuleEntryAdd (keymacalListNameValue,&keymacalListPrioriryValue);
  }
  else if (objmacalRuleRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
      /* get the value from application */
     owa.l7rc =  usmDbMacalActiveListGet(macalListNameValue);
     if(owa.l7rc == L7_SUCCESS)
     {
         if ( strcmp(macalListNameValue,keymacalListNameValue) == L7_NULL )
         {
              owa.rc = XLIBRC_CURRENTLY_ACTIVE;    /* TODO: Change if required */
	       return owa.rc;
         }
     }
    owa.l7rc = usmDbMacalRuleRemove (keymacalListNameValue,keymacalListPrioriryValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalActiveDeActive
*
* @purpose Get 'macalActiveDeActive'
 *@description  [macalActiveDeActive] <HTML>Activate or Deactivate an ACAL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalActiveDeActive (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalActiveDeActiveValue;

  xLibStr256_t keymacalListNameValue;
  xLibStr256_t macalListNameValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* get the value from application */
  owa.l7rc =  usmDbMacalActiveListGet(macalListNameValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ( strcmp(macalListNameValue,keymacalListNameValue) == L7_NULL )
  {
      objmacalActiveDeActiveValue = L7_ENABLE;
  }
  else
  {
     objmacalActiveDeActiveValue = L7_DISABLE;
  }
  	
  FPOBJ_TRACE_VALUE (bufp, &objmacalActiveDeActiveValue, sizeof (objmacalActiveDeActiveValue));

  /* return the object value: macalActiveDeActive */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacalActiveDeActiveValue,
                           sizeof (objmacalActiveDeActiveValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitymgmtacal_macalActiveDeActive
*
* @purpose Set 'macalActiveDeActive'
 *@description  [macalActiveDeActive] <HTML>Activate or Deactivate an ACAL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitymgmtacal_macalActiveDeActive (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalActiveDeActiveValue;

  xLibStr256_t keymacalListNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macalActiveDeActive */
  owa.len = sizeof (objmacalActiveDeActiveValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmacalActiveDeActiveValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmacalActiveDeActiveValue, owa.len);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);
  
  /* set the value in application */
  if(objmacalActiveDeActiveValue == L7_ENABLE)
  {
    owa.l7rc = usmDbMacalActivate (keymacalListNameValue);
  }
  else
  {
    owa.l7rc = usmDbMacalDeactivate (keymacalListNameValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalRuleAction
*
* @purpose Get 'macalRuleActionAdd'
 *@description  [macalRuleActionAdd] <HTML>To Add an Action to the Created Macal
* Rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalRuleAction (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleActionAddValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbMacalRuleActionGet (keymacalListNameValue,
                              keymacalListPrioriryValue, &objmacalRuleActionAddValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleActionAddValue, sizeof (objmacalRuleActionAddValue));

  /* return the object value: macalRuleAction */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacalRuleActionAddValue,
                           sizeof (objmacalRuleActionAddValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitymgmtacal_macalRuleAction
*
* @purpose Set 'macalRuleActionAdd'
 *@description  [macalRuleActionAdd] <HTML>To Add an Action to the Created Macal
* Rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitymgmtacal_macalRuleAction (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleActionAddValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macalRuleAction */
  owa.len = sizeof (objmacalRuleActionAddValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmacalRuleActionAddValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleActionAddValue, owa.len);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbMacalRuleActionAdd (keymacalListNameValue,
                              keymacalListPrioriryValue, objmacalRuleActionAddValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalRuleSrcIp
*
* @purpose Get 'macalRuleSrcIp'
 *@description  [macalRuleSrcIp] <HTML>To Set or Get the source ip address  to
* an ACAL rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalRuleSrcIp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objmacalRuleSrcIpValue;
  xLibIpV4_t objmacalRuleSrcIpMaskDummy;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbMacalRuleSrcIpMaskGet (keymacalListNameValue,
                              keymacalListPrioriryValue, &objmacalRuleSrcIpValue,&objmacalRuleSrcIpMaskDummy);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleSrcIpValue, sizeof (objmacalRuleSrcIpValue));

  /* return the object value: macalRuleSrcIp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacalRuleSrcIpValue,
                           sizeof (objmacalRuleSrcIpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitymgmtacal_macalRuleSrcIp
*
* @purpose Set 'macalRuleSrcIp'
 *@description  [macalRuleSrcIp] <HTML>To Set or Get the source ip address  to
* an ACAL rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitymgmtacal_macalRuleSrcIp (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objmacalRuleSrcIpValue;
  xLibIpV4_t objmacalRuleSrcIpMaskDummy;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macalRuleSrcIp */
  owa.len = sizeof (objmacalRuleSrcIpValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmacalRuleSrcIpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleSrcIpValue, owa.len);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);
  
  /* retrieve object: macalRuleSrcIpMask */
  owa.len = sizeof (objmacalRuleSrcIpMaskDummy);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalRuleSrcMask,
                          (xLibU8_t *) &objmacalRuleSrcIpMaskDummy, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* set the value in application */
  owa.l7rc = usmDbMacalRuleSrcIpMaskAdd (keymacalListNameValue,
                              keymacalListPrioriryValue, objmacalRuleSrcIpValue,objmacalRuleSrcIpMaskDummy);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalRuleSrcMask
*
* @purpose Get 'macalRuleSrcMask'
 *@description  [macalRuleSrcMask] <HTML>To Set or Get the Src IP Mask to an
* ACAL rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalRuleSrcMask (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objmacalRuleSrcMaskValue;
  xLibIpV4_t objmacalRuleSrcIpValueDummy;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbMacalRuleSrcIpMaskGet (keymacalListNameValue,
                              keymacalListPrioriryValue, &objmacalRuleSrcIpValueDummy, &objmacalRuleSrcMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleSrcMaskValue, sizeof (objmacalRuleSrcMaskValue));

  /* return the object value: macalRuleSrcMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacalRuleSrcMaskValue,
                           sizeof (objmacalRuleSrcMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitymgmtacal_macalRuleSrcMask
*
* @purpose Set 'macalRuleSrcMask'
 *@description  [macalRuleSrcMask] <HTML>To Set or Get the Src IP Mask to an
* ACAL rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitymgmtacal_macalRuleSrcMask (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objmacalRuleSrcMaskValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macalRuleSrcMask */
  owa.len = sizeof (objmacalRuleSrcMaskValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmacalRuleSrcMaskValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleSrcMaskValue, owa.len);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalRuleInterface
*
* @purpose Get 'macalRuleInterface'
 *@description  [macalRuleInterface] <HTML>To Add the Specified Interface for a
* given Rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalRuleInterface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleInterfaceValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbMacalRuleInterfaceGet (keymacalListNameValue,
                              keymacalListPrioriryValue, &objmacalRuleInterfaceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleInterfaceValue, sizeof (objmacalRuleInterfaceValue));

  /* return the object value: macalRuleInterface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacalRuleInterfaceValue,
                           sizeof (objmacalRuleInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitymgmtacal_macalRuleInterface
*
* @purpose Set 'macalRuleInterface'
 *@description  [macalRuleInterface] <HTML>To Add the Specified Interface for a
* given Rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitymgmtacal_macalRuleInterface (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleInterfaceValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macalRuleInterface */
  owa.len = sizeof (objmacalRuleInterfaceValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmacalRuleInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleInterfaceValue, owa.len);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbMacalRuleInterfaceAdd (keymacalListNameValue,
                              keymacalListPrioriryValue, objmacalRuleInterfaceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalRuleVlan
*
* @purpose Get 'macalRuleVlan'
 *@description  [macalRuleVlan] <HTML>To add or show a VLAN to a rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalRuleVlan (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleVlanValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbMacalRuleVlanGet (keymacalListNameValue,
                              keymacalListPrioriryValue, &objmacalRuleVlanValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleVlanValue, sizeof (objmacalRuleVlanValue));

  /* return the object value: macalRuleVlan */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacalRuleVlanValue,
                           sizeof (objmacalRuleVlanValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitymgmtacal_macalRuleVlan
*
* @purpose Set 'macalRuleVlan'
 *@description  [macalRuleVlan] <HTML>To add or show a VLAN to a rule   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitymgmtacal_macalRuleVlan (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleVlanValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macalRuleVlan */
  owa.len = sizeof (objmacalRuleVlanValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmacalRuleVlanValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleVlanValue, owa.len);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbMacalRuleVlanAdd (keymacalListNameValue,
                              keymacalListPrioriryValue, objmacalRuleVlanValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalRuleService
*
* @purpose Get 'macalRuleService'
 *@description  [macalRuleService] <HTML>To Add or show the configured Service
* for a given macal   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalRuleService (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleServiceValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbMacalRuleServiceGet (keymacalListNameValue,
                              keymacalListPrioriryValue, &objmacalRuleServiceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleServiceValue, sizeof (objmacalRuleServiceValue));

  /* return the object value: macalRuleService */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmacalRuleServiceValue,
                           sizeof (objmacalRuleServiceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitymgmtacal_macalRuleService
*
* @purpose Set 'macalRuleService'
 *@description  [macalRuleService] <HTML>To Add or show the configured Service
* for a given macal   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitymgmtacal_macalRuleService (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmacalRuleServiceValue;

  xLibStr256_t keymacalListNameValue;
  xLibU32_t keymacalListPrioriryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: macalRuleService */
  owa.len = sizeof (objmacalRuleServiceValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmacalRuleServiceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmacalRuleServiceValue, owa.len);

  /* retrieve key: macalListName */
  owa.len = sizeof (keymacalListNameValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListName,
                          (xLibU8_t *) keymacalListNameValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keymacalListNameValue, owa.len);

  /* retrieve key: macalListPrioriry */
  owa.len = sizeof (keymacalListPrioriryValue);
  owa.rc = xLibFilterGet (wap, XOBJ_securitymgmtacal_macalListPrioriry,
                          (xLibU8_t *) & keymacalListPrioriryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymacalListPrioriryValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbMacalRuleServiceAdd (keymacalListNameValue,
                              keymacalListPrioriryValue, objmacalRuleServiceValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_securitymgmtacal_macalActiveProfileGet
*
* @purpose Get 'macalActiveProfileGet'
 *@description  [macalActiveProfileGet] <HTML>To Get the  Current Active Profile   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitymgmtacal_macalActiveProfileGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr256_t objmacalActiveProfileGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMacalActiveListGet (objmacalActiveProfileGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objmacalActiveProfileGetValue, strlen (objmacalActiveProfileGetValue));

  /* return the object value: macalActiveProfileGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmacalActiveProfileGetValue,
                           strlen (objmacalActiveProfileGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
