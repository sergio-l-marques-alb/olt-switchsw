
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseSysMiscObjects.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  15 April 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseSysMiscObjects_obj.h"
#include "usmdb_sim_api.h"


/*******************************************************************************
* @function fpObjGet_baseSysMiscObjects_SaveDeviceConfiguration
*
* @purpose Get 'SaveDeviceConfiguration'
*
* @description [SaveDeviceConfiguration] Save the configuration of the device. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseSysMiscObjects_SaveDeviceConfiguration (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSaveDeviceConfigurationValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlSaveConfigurationGet (L7_UNIT_CURRENT, &objSaveDeviceConfigurationValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objSaveDeviceConfigurationValue,
                     sizeof (objSaveDeviceConfigurationValue));

  /* return the object value: SaveDeviceConfiguration */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSaveDeviceConfigurationValue,
                           sizeof (objSaveDeviceConfigurationValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseSysMiscObjects_SaveDeviceConfiguration
*
* @purpose Set 'SaveDeviceConfiguration'
*
* @description [SaveDeviceConfiguration] Save the configuration of the device. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseSysMiscObjects_SaveDeviceConfiguration (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSaveDeviceConfigurationValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: SaveDeviceConfiguration */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSaveDeviceConfigurationValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSaveDeviceConfigurationValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlSaveConfigurationSet (L7_UNIT_CURRENT, L7_TRUE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  else
  {
    owa.rc = XLIBRC_COMMON_SAVECONFIG_MESSAGE;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


