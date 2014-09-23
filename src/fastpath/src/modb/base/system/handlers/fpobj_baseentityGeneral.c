/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseentityGeneral.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to system-object.xml
*
* @create  11 March 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseentityGeneral_obj.h"
#include "usmdb_edb_api.h"

/*******************************************************************************
* @function fpObjGet_baseentityGeneral_entLastChangeTime
*
* @purpose Get 'entLastChangeTime'
*
* @description [entLastChangeTime]: The value of sysUpTime at the time a conceptual
*              row is created, modified, or deleted in any of these
*              tables: - entPhysicalTable - entLogicalTable - entLPMappingTable
*              - 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseentityGeneral_entLastChangeTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objentLastChangeTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbEdbLastChangeTimeGet ( &objentLastChangeTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objentLastChangeTimeValue,
                     sizeof (objentLastChangeTimeValue));

  /* return the object value: entLastChangeTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objentLastChangeTimeValue,
                           sizeof (objentLastChangeTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
