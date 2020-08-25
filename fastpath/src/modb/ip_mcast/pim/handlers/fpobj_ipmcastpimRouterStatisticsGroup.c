
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimRouterStatisticsGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimRouterStatisticsGroup_obj.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimRouterStatisticsGroup_pimStarGEntries
*
* @purpose Get 'pimStarGEntries'
*
* @description [pimStarGEntries] The number of entries in the pimStarGTable.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimRouterStatisticsGroup_pimStarGEntries (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimStarGEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimStarGEntriesValue, sizeof (objpimStarGEntriesValue));

  /* return the object value: pimStarGEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGEntriesValue,
                           sizeof (objpimStarGEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimRouterStatisticsGroup_pimStarGIEntries
*
* @purpose Get 'pimStarGIEntries'
*
* @description [pimStarGIEntries] The number of entries in the pimStarGITable.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimRouterStatisticsGroup_pimStarGIEntries (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimStarGIEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimStarGIEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimStarGIEntriesValue, sizeof (objpimStarGIEntriesValue));

  /* return the object value: pimStarGIEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimStarGIEntriesValue,
                           sizeof (objpimStarGIEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimRouterStatisticsGroup_pimSGEntries
*
* @purpose Get 'pimSGEntries'
*
* @description [pimSGEntries] The number of entries in the pimSGTable.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimRouterStatisticsGroup_pimSGEntries (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimSGEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimSGEntriesValue, sizeof (objpimSGEntriesValue));

  /* return the object value: pimSGEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGEntriesValue,
                           sizeof (objpimSGEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimRouterStatisticsGroup_pimSGIEntries
*
* @purpose Get 'pimSGIEntries'
*
* @description [pimSGIEntries] The number of entries in the pimSGITable.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimRouterStatisticsGroup_pimSGIEntries (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGIEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimSGIEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimSGIEntriesValue, sizeof (objpimSGIEntriesValue));

  /* return the object value: pimSGIEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGIEntriesValue,
                           sizeof (objpimSGIEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimRouterStatisticsGroup_pimSGRptEntries
*
* @purpose Get 'pimSGRptEntries'
*
* @description [pimSGRptEntries] The number of entries in the pimSGRptTable.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimRouterStatisticsGroup_pimSGRptEntries (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimSGRptEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimSGRptEntriesValue, sizeof (objpimSGRptEntriesValue));

  /* return the object value: pimSGRptEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptEntriesValue,
                           sizeof (objpimSGRptEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimRouterStatisticsGroup_pimSGRptIEntries
*
* @purpose Get 'pimSGRptIEntries'
*
* @description [pimSGRptIEntries] The number of entries in the pimSGRptITable.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimRouterStatisticsGroup_pimSGRptIEntries (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimSGRptIEntriesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimSGRptIEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimSGRptIEntriesValue, sizeof (objpimSGRptIEntriesValue));

  /* return the object value: pimSGRptIEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimSGRptIEntriesValue,
                           sizeof (objpimSGRptIEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
