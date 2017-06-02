/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPXMedConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to LLDP-object.xml
*
* @create  5 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingLLDPXMedConfig_obj.h"
#include "usmdb_lldp_api.h"
#include "lldp_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMedConfig_LocDeviceClass
*
* @purpose Get 'LocDeviceClass'
*
* @description [LocDeviceClass]: Local Device Class 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMedConfig_LocDeviceClass (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t  objLocDeviceClassValue;
  L7_uint32 count=0;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedLocDeviceClassGet (&count); 
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiSnprintf(objLocDeviceClassValue, sizeof(objLocDeviceClassValue), "%s", 
                usmDbLldpXMedDeviceClassString(count));
  FPOBJ_TRACE_VALUE (bufp, objLocDeviceClassValue,
                     strlen (objLocDeviceClassValue));

  /* return the object value: LocDeviceClass */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLocDeviceClassValue,
                           strlen (objLocDeviceClassValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMedConfig_FastStartRepeatCount
*
* @purpose Get 'FastStartRepeatCount'
*
* @description [FastStartRepeatCount]: The number of times the fast start
*              LLDPDU are being sent during the activation of the fast start
*              mechanism defined by LLDP-MED. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMedConfig_FastStartRepeatCount (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFastStartRepeatCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedFastStartRepeatCountGet ( &objFastStartRepeatCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFastStartRepeatCountValue,
                     sizeof (objFastStartRepeatCountValue));

  /* return the object value: FastStartRepeatCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFastStartRepeatCountValue,
                           sizeof (objFastStartRepeatCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPXMedConfig_FastStartRepeatCount
*
* @purpose Set 'FastStartRepeatCount'
*
* @description [FastStartRepeatCount]: The number of times the fast start
*              LLDPDU are being sent during the activation of the fast start
*              mechanism defined by LLDP-MED. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPXMedConfig_FastStartRepeatCount (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFastStartRepeatCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FastStartRepeatCount */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objFastStartRepeatCountValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFastStartRepeatCountValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbLldpXMedFastStartRepeatCountSet (objFastStartRepeatCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


