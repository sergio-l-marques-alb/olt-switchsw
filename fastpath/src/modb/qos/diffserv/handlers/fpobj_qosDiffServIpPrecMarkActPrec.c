/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServIpPrecMarkActPrec.c
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
#include "_xe_qosDiffServIpPrecMarkActPrec_obj.h"
#include "usmdb_mib_diffserv_api.h"


/*******************************************************************************
* @function fpObjGet_qosDiffServIpPrecMarkActPrec_DiffServIpPrecMarkActPrecedence
*
* @purpose Get 'DiffServIpPrecMarkActPrecedence'
*
* @description [DiffServIpPrecMarkActPrecedence]: The IP Precedence that this
*              Action will store into the Precedence field of the IP packet.
*              It is quite possible that the only packets subject to
*              this Action are already marked with this IP Precedence. Note
*              also that Differentiated Servi 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_qosDiffServIpPrecMarkActPrec_DiffServIpPrecMarkActPrecedence (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServIpPrecMarkActPrecedenceValue;
  xLibU32_t nextObjDiffServIpPrecMarkActPrecedenceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServIpPrecMarkActPrecedence */
  owa.rc =
    xLibFilterGet (wap,
                   XOBJ_qosDiffServIpPrecMarkActPrec_DiffServIpPrecMarkActPrecedence,
                   (xLibU8_t *) & objDiffServIpPrecMarkActPrecedenceValue,
                   &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objDiffServIpPrecMarkActPrecedenceValue = 0;
    nextObjDiffServIpPrecMarkActPrecedenceValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDiffServIpPrecMarkActPrecedenceValue,
                             owa.len);
  owa.l7rc =
      usmDbDiffServIpPrecMarkActGetNext (L7_UNIT_CURRENT,
                                         objDiffServIpPrecMarkActPrecedenceValue,
                                         &nextObjDiffServIpPrecMarkActPrecedenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDiffServIpPrecMarkActPrecedenceValue,
                       owa.len);

  /* return the object value: DiffServIpPrecMarkActPrecedence */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & nextObjDiffServIpPrecMarkActPrecedenceValue,
                    sizeof (objDiffServIpPrecMarkActPrecedenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
