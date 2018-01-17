/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosdiffServDscpMarkActDscp.c
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
#include "_xe_qosdiffServDscpMarkActDscp_obj.h"
#include "usmdb_mib_diffserv_api.h"

/*******************************************************************************
* @function fpObjGet_qosdiffServDscpMarkActDscp_diffServDscpMarkActDscp
*
* @purpose Get 'diffServDscpMarkActDscp'
*
* @description [diffServDscpMarkActDscp]: The DSCP that this Action will store
*              into the DSCP field of the subject. It is quite possible
*              that the only packets subject to this Action are already
*              marked with this DSCP. Note also that Differentiated Services
*              processing may result in 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServDscpMarkActDscp_diffServDscpMarkActDscp (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServDscpMarkActDscpValue;
  xLibU32_t nextObjdiffServDscpMarkActDscpValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServDscpMarkActDscp */
  owa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServDscpMarkActDscp_diffServDscpMarkActDscp,
                   (xLibU8_t *) & objdiffServDscpMarkActDscpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
	
    objdiffServDscpMarkActDscpValue = 0;
    nextObjdiffServDscpMarkActDscpValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdiffServDscpMarkActDscpValue, owa.len);
  owa.l7rc =
      usmDbDiffServDscpMarkActGetNext (L7_UNIT_CURRENT,
                                       objdiffServDscpMarkActDscpValue,
                                       &nextObjdiffServDscpMarkActDscpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdiffServDscpMarkActDscpValue, owa.len);

  /* return the object value: diffServDscpMarkActDscp */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdiffServDscpMarkActDscpValue,
                    sizeof (objdiffServDscpMarkActDscpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
