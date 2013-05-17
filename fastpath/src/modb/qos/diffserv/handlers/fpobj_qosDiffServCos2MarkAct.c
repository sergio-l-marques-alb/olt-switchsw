/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServCos2MarkAct.c
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
#include "_xe_qosDiffServCos2MarkAct_obj.h"
#include "usmdb_mib_diffserv_api.h"

/*******************************************************************************
* @function fpObjGet_qosDiffServCos2MarkAct_DiffServCos2MarkActCos
*
* @purpose Get 'DiffServCos2MarkActCos'
*
* @description [DiffServCos2MarkActCos]: The Secondary Class of Service value
*              that this Action will store into the priority bits of the
*              second/inner 802.1p tag header. It is quite possible that
*              the only packets subject to this Action are already marked
*              with this COS value. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServCos2MarkAct_DiffServCos2MarkActCos (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServCos2MarkActCosValue;
  xLibU32_t nextObjDiffServCos2MarkActCosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServCos2MarkActCos */
  owa.rc =
    xLibFilterGet (wap, XOBJ_qosDiffServCos2MarkAct_DiffServCos2MarkActCos,
                   (xLibU8_t *) & objDiffServCos2MarkActCosValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objDiffServCos2MarkActCosValue = 0;
    nextObjDiffServCos2MarkActCosValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDiffServCos2MarkActCosValue, owa.len);
  owa.l7rc =
      usmDbDiffServCos2MarkActGetNext (L7_UNIT_CURRENT,
                                       objDiffServCos2MarkActCosValue,
                                       &nextObjDiffServCos2MarkActCosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDiffServCos2MarkActCosValue, owa.len);

  /* return the object value: DiffServCos2MarkActCos */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDiffServCos2MarkActCosValue,
                    sizeof (objDiffServCos2MarkActCosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
