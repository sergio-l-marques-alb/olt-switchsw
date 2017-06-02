
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimStorage.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimStorage_obj.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimStorage_pimDeviceConfigStorageType
*
* @purpose Get 'pimDeviceConfigStorageType'
*
* @description [pimDeviceConfigStorageType] The storage type used for the global PIM configuration of this device.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimStorage_pimDeviceConfigStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimDeviceConfigStorageTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, &objpimDeviceConfigStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objpimDeviceConfigStorageTypeValue,
                     sizeof (objpimDeviceConfigStorageTypeValue));

  /* return the object value: pimDeviceConfigStorageType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objpimDeviceConfigStorageTypeValue,
                           sizeof (objpimDeviceConfigStorageTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjSet_ipmcastpimStorage_pimDeviceConfigStorageType
*
* @purpose Set 'pimDeviceConfigStorageType'
*
* @description [pimDeviceConfigStorageType] The storage type used for the global PIM configuration of this device.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_ipmcastpimStorage_pimDeviceConfigStorageType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimDeviceConfigStorageTypeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: pimDeviceConfigStorageType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objpimDeviceConfigStorageTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objpimDeviceConfigStorageTypeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, objpimDeviceConfigStorageTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
