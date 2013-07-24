/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServGeneral.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to diffserv-object.xml
*
* @create  2 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_qosDiffServGeneral_obj.h"
#include "usmdb_mib_diffserv_api.h"

/*******************************************************************************
* @function fpObjGet_qosDiffServGeneral_NextFree
*
* @purpose Get 'NextFree'
*
* @description [NextFree]: This object contains an unused value for agentDiffServRedirectId,
*              or a zero to indicate that none exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGeneral_NextFree (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServRedirectIndexNext (L7_UNIT_CURRENT, &objNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objNextFreeValue == 0 )
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* ADD TABLE FULL ERROR CODE */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objNextFreeValue, sizeof (objNextFreeValue));

  /* return the object value: NextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNextFreeValue,
                           sizeof (objNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGeneral_DiffServAuxMfClfrNextFree
*
* @purpose Get 'DiffServAuxMfClfrNextFree'
*
* @description [DiffServAuxMfClfrNextFree]: This object contains an unused
*              value for agentDiffServAuxMfClfrId , or a zero to indicate
*              that none exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGeneral_DiffServAuxMfClfrNextFree (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServAuxMfClfrNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrIndexNext (L7_UNIT_CURRENT,
                                     &objDiffServAuxMfClfrNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objDiffServAuxMfClfrNextFreeValue == 0 )
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* ADD TABLE FULL ERROR CODE */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
	
  FPOBJ_TRACE_VALUE (bufp, &objDiffServAuxMfClfrNextFreeValue,
                     sizeof (objDiffServAuxMfClfrNextFreeValue));

  /* return the object value: DiffServAuxMfClfrNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDiffServAuxMfClfrNextFreeValue,
                    sizeof (objDiffServAuxMfClfrNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGeneral_DiffServAssignQueueNextFree
*
* @purpose Get 'DiffServAssignQueueNextFree'
*
* @description [DiffServAssignQueueNextFree]: This object contains an unused
*              value for agentDiffServAssignIndex, or a zero to indicate
*              that none exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGeneral_DiffServAssignQueueNextFree (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServAssignQueueNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAssignQueueIndexNext (L7_UNIT_CURRENT,
                                       &objDiffServAssignQueueNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objDiffServAssignQueueNextFreeValue == 0 )
  {
    owa.rc = XLIBRC_FAILURE;    /* ADD TABLE FULL ERROR CODE */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objDiffServAssignQueueNextFreeValue,
                     sizeof (objDiffServAssignQueueNextFreeValue));

  /* return the object value: DiffServAssignQueueNextFree */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDiffServAssignQueueNextFreeValue,
                    sizeof (objDiffServAssignQueueNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServGeneral_DiffServMirrorNextFree
*
* @purpose Get 'DiffServMirrorNextFree'
*
* @description [DiffServMirrorNextFree]: This object contains an unused value
*              for agentDiffServMirrorId, or a zero to indicate that none
*              exist. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServGeneral_DiffServMirrorNextFree (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServMirrorNextFreeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMirrorIndexNext (L7_UNIT_CURRENT,
                                  &objDiffServMirrorNextFreeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(objDiffServMirrorNextFreeValue == 0 )
  {
    owa.rc = XLIBRC_COMMON_TABLE_FULL;    /* ADD TABLE FULL ERROR CODE */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objDiffServMirrorNextFreeValue,
                     sizeof (objDiffServMirrorNextFreeValue));

  /* return the object value: DiffServMirrorNextFree */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDiffServMirrorNextFreeValue,
                           sizeof (objDiffServMirrorNextFreeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
