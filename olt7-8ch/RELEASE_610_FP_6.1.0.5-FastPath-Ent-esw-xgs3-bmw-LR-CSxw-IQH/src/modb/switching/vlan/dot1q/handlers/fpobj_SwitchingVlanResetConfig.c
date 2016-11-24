/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingVlanPortConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to VLan-object.xml
*
* @create  11 September 2008
*
* @author  Rajesh G
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingVlanResetConfig_obj.h"
#include "usmdb_dot1q_api.h"

/*******************************************************************************
* @function fpObjSet_SwitchingVlanResetConfig_ResetVlan
*
* @purpose Resets VLAN configuration to default values
*
* @description Resets VLAN configuration to factory set default values
*              parameters.
*
* 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVlanResetConfig_ResetVlan (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSwitchingVlanResetConfigValue=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SwitchingVlanResetConfig*/
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objSwitchingVlanResetConfigValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSwitchingVlanResetConfigValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbVlanResetToDefaultSet (L7_UNIT_CURRENT, objSwitchingVlanResetConfigValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
