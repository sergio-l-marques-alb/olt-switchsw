/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingVoiceVlanConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to VoiceVlan-object.xml
*
* @create  5 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingVoiceVlanConfig_obj.h"
#include "usmdb_voice_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanConfig_GlobalAdminMode
*
* @purpose Get 'GlobalAdminMode'
*
* @description [GlobalAdminMode]: This enables or disables Voice VLAN on the
*              system 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanConfig_GlobalAdminMode (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanAdminModeGet(L7_UNIT_CURRENT, &objGlobalAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalAdminModeValue,
                     sizeof (objGlobalAdminModeValue));

  /* return the object value: GlobalAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objGlobalAdminModeValue,
                           sizeof (objGlobalAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanConfig_GlobalAdminMode
*
* @purpose Set 'GlobalAdminMode'
*
* @description [GlobalAdminMode]: This enables or disables Voice VLAN on the
*              system 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanConfig_GlobalAdminMode (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objGlobalAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: GlobalAdminMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objGlobalAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objGlobalAdminModeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbVoiceVlanAdminModeSet(L7_UNIT_CURRENT, objGlobalAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
