/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServCosMarkAct.c
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
#include "_xe_qosDiffServCosMarkAct_obj.h"
#include "usmdb_mib_diffserv_api.h"


/*******************************************************************************
* @function fpObjGet_qosDiffServCosMarkAct_DiffServCosMarkActCos
*
* @purpose Get 'DiffServCosMarkActCos'
*
* @description [DiffServCosMarkActCos]: The Class of Service value that this
*              Action will store into the priority bits of the 802.1p header.
*              It is quite possible that the only packets subject to
*              this Action are already marked with this COS value. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServCosMarkAct_DiffServCosMarkActCos (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServCosMarkActCosValue;
  xLibU32_t nextObjDiffServCosMarkActCosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServCosMarkActCos */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServCosMarkAct_DiffServCosMarkActCos,
                          (xLibU8_t *) & objDiffServCosMarkActCosValue,
                          &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objDiffServCosMarkActCosValue = 0;
    nextObjDiffServCosMarkActCosValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDiffServCosMarkActCosValue, owa.len);
  owa.l7rc =
      usmDbDiffServCosMarkActGetNext (L7_UNIT_CURRENT,
                                      objDiffServCosMarkActCosValue,
                                      &nextObjDiffServCosMarkActCosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDiffServCosMarkActCosValue, owa.len);

  /* return the object value: DiffServCosMarkActCos */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDiffServCosMarkActCosValue,
                    sizeof (objDiffServCosMarkActCosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
