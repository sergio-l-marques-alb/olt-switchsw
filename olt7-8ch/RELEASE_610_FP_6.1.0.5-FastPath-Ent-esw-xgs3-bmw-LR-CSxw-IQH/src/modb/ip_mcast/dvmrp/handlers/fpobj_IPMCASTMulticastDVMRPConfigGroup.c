
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTMulticastDVMRPConfigGroup.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to IPMCAST-object.xml
*
* @create  01 May 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_IPMCASTMulticastDVMRPConfigGroup_obj.h"
#include "usmdb_mib_dvmrp_api.h"


/*******************************************************************************
* @function fpObjGet_IPMCASTMulticastDVMRPConfigGroup_dvmrpGlobalAdminMode
*
* @purpose Get 'dvmrpGlobalAdminMode'
*
* @description [dvmrpGlobalAdminMode] enables or disables DVMRP on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTMulticastDVMRPConfigGroup_dvmrpGlobalAdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpGlobalAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpAdminModeGet(L7_UNIT_CURRENT, &objdvmrpGlobalAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdvmrpGlobalAdminModeValue, sizeof (objdvmrpGlobalAdminModeValue));

  /* return the object value: dvmrpGlobalAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpGlobalAdminModeValue,
                           sizeof (objdvmrpGlobalAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTMulticastDVMRPConfigGroup_dvmrpGlobalAdminMode
*
* @purpose Set 'dvmrpGlobalAdminMode'
*
* @description [dvmrpGlobalAdminMode] enables or disables DVMRP on the system
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTMulticastDVMRPConfigGroup_dvmrpGlobalAdminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpGlobalAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dvmrpGlobalAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objdvmrpGlobalAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdvmrpGlobalAdminModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbDvmrpAdminModeSet(L7_UNIT_CURRENT, objdvmrpGlobalAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
