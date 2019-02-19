
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseTrapFlags.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  15 April 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseTrapFlags_obj.h"
#include "usmdb_trapmgr_api.h"

/*******************************************************************************
* @function fpObjGet_baseTrapFlags_Authentication
*
* @purpose Get 'Authentication'
*
* @description [Authentication] Authentication Trap flag
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapFlags_Authentication (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTrapAuthSwGet (L7_UNIT_CURRENT, &objAuthenticationValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationValue, sizeof (objAuthenticationValue));

  /* return the object value: Authentication */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAuthenticationValue,
                           sizeof (objAuthenticationValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapFlags_Authentication
*
* @purpose Set 'Authentication'
*
* @description [Authentication] Authentication Trap flag
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapFlags_Authentication (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAuthenticationValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Authentication */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAuthenticationValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAuthenticationValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbTrapAuthSwSet (L7_UNIT_CURRENT, objAuthenticationValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseTrapFlags_LinkUpDown
*
* @purpose Get 'LinkUpDown'
*
* @description [LinkUpDown] Link UpDown trap flag
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapFlags_LinkUpDown (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLinkUpDownValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTrapLinkStatusSwGet (L7_UNIT_CURRENT, &objLinkUpDownValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objLinkUpDownValue, sizeof (objLinkUpDownValue));

  /* return the object value: LinkUpDown */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLinkUpDownValue, sizeof (objLinkUpDownValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapFlags_LinkUpDown
*
* @purpose Set 'LinkUpDown'
*
* @description [LinkUpDown] Link UpDown trap flag
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapFlags_LinkUpDown (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLinkUpDownValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LinkUpDown */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLinkUpDownValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLinkUpDownValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbTrapLinkStatusSwSet (L7_UNIT_CURRENT, objLinkUpDownValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseTrapFlags_MultipleUsers
*
* @purpose Get 'MultipleUsers'
*
* @description [MultipleUsers] Multiple users Trap flag
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapFlags_MultipleUsers (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMultipleUsersValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTrapMultiUsersSwGet (L7_UNIT_CURRENT, &objMultipleUsersValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMultipleUsersValue, sizeof (objMultipleUsersValue));

  /* return the object value: MultipleUsers */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMultipleUsersValue,
                           sizeof (objMultipleUsersValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapFlags_MultipleUsers
*
* @purpose Set 'MultipleUsers'
*
* @description [MultipleUsers] Multiple users Trap flag
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapFlags_MultipleUsers (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMultipleUsersValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MultipleUsers */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMultipleUsersValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMultipleUsersValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbTrapMultiUsersSwSet (L7_UNIT_CURRENT, objMultipleUsersValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseTrapFlags_SpanningTree
*
* @purpose Get 'SpanningTree'
*
* @description [SpanningTree] Spanning Tree trap flag
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseTrapFlags_SpanningTree (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSpanningTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbTrapSpanningTreeSwGet (L7_UNIT_CURRENT, &objSpanningTreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objSpanningTreeValue, sizeof (objSpanningTreeValue));

  /* return the object value: SpanningTree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSpanningTreeValue,
                           sizeof (objSpanningTreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseTrapFlags_SpanningTree
*
* @purpose Set 'SpanningTree'
*
* @description [SpanningTree] Spanning Tree trap flag
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseTrapFlags_SpanningTree (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSpanningTreeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SpanningTree */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSpanningTreeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSpanningTreeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbTrapSpanningTreeSwSet (L7_UNIT_CURRENT, objSpanningTreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
