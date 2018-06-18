
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingFdbStats.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  26 May 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingFdbStats_obj.h"
#include "usmdb_mib_bridge_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingFdbStats_BadAdds
*
* @purpose Get 'BadAdds'
 *@description  [BadAdds] The number of failed FDB insert attempts due to
* failure to allocate space   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbStats_BadAdds (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBadAddsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbFdbBadAddsGet(L7_UNIT_CURRENT, &objBadAddsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBadAddsValue, sizeof (objBadAddsValue));

  /* return the object value: BadAdds */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBadAddsValue, sizeof (objBadAddsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbStats_MaxTableEntries
*
* @purpose Get 'MaxTableEntries'
 *@description  [MaxTableEntries] The maximum number of entries that the FDB
* table can hold   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbStats_MaxTableEntries (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxTableEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbFdbMaxTableEntriesGet(L7_UNIT_CURRENT, &objMaxTableEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMaxTableEntriesValue, sizeof (objMaxTableEntriesValue));

  /* return the object value: MaxTableEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxTableEntriesValue,
                           sizeof (objMaxTableEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbStats_ActiveAddrEntries
*
* @purpose Get 'ActiveAddrEntries'
 *@description  [ActiveAddrEntries] Current Active entries in the FDB table   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbStats_ActiveAddrEntries (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objActiveAddrEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbActiveAddrEntriesGet(L7_UNIT_CURRENT, &objActiveAddrEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objActiveAddrEntriesValue, sizeof (objActiveAddrEntriesValue));

  /* return the object value: ActiveAddrEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objActiveAddrEntriesValue,
                           sizeof (objActiveAddrEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbStats_MostEntries
*
* @purpose Get 'MostEntries'
 *@description  [MostEntries] Most entries in the FDB table   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbStats_MostEntries (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMostEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMostAddrEntriesGet(L7_UNIT_CURRENT, &objMostEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMostEntriesValue, sizeof (objMostEntriesValue));

  /* return the object value: MostEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMostEntriesValue, sizeof (objMostEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingFdbStats_MostEntriesReset
*
* @purpose Set 'MostEntriesReset'
 *@description  [MostEntriesReset] Reset the most entries value in the FDB table
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingFdbStats_MostEntriesReset (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMostEntriesResetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MostEntriesReset */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMostEntriesResetValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMostEntriesResetValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbMostAddrEntriesReset(L7_UNIT_CURRENT);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbStats_FdbDynamicEntriesGet
*
* @purpose Get 'FdbDynamicEntriesGet'
 *@description  [FdbDynamicEntriesGet] <HTML>Get count of dynamic entries in the
* L2FDB.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbStats_FdbDynamicEntriesGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbDynamicEntriesGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbFdbDynamicEntriesGet (&objFdbDynamicEntriesGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objFdbDynamicEntriesGetValue, sizeof (objFdbDynamicEntriesGetValue));

  /* return the object value: FdbDynamicEntriesGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbDynamicEntriesGetValue,
                           sizeof (objFdbDynamicEntriesGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbStats_FdbInternalEntriesGet
*
* @purpose Get 'FdbInternalEntriesGet'
 *@description  [FdbInternalEntriesGet] <HTML>Get count of internally added
* entries in the L2FDB.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbStats_FdbInternalEntriesGet (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objFdbInternalEntriesGetValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbFdbInternalEntriesGet (&objFdbInternalEntriesGetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objFdbInternalEntriesGetValue, sizeof (objFdbInternalEntriesGetValue));

  /* return the object value: FdbInternalEntriesGet */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFdbInternalEntriesGetValue,
                           sizeof (objFdbInternalEntriesGetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
