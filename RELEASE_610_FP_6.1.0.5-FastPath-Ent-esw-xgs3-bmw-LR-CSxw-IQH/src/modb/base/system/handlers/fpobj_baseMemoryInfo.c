
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseMemoryInfo.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  01 October 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseMemoryInfo_obj.h"
#include "usmdb_switch_cpu_api.h"



/*******************************************************************************
* @function fpObjGet_baseMemoryInfo_TotalMemory
*
* @purpose Get 'TotalMemory'
 *@description  [TotalMemory] This is the total Memory available on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseMemoryInfo_TotalMemory (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t objTotalMemoryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbInfoSwitchCpuProcessMemAvailable(L7_UNIT_CURRENT, &objTotalMemoryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTotalMemoryValue, sizeof (objTotalMemoryValue));

  /* return the object value: TotalMemory */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTotalMemoryValue, sizeof (objTotalMemoryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseMemoryInfo_AllocMemory
*
* @purpose Get 'AllocMemory'
 *@description  [AllocMemory] This is the Allocated Memory for the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseMemoryInfo_AllocMemory (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t objAllocMemoryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbInfoSwitchCpuProcessMemAllocated(L7_UNIT_CURRENT, &objAllocMemoryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_VALUE (bufp, &objAllocMemoryValue, sizeof (objAllocMemoryValue));

  /* return the object value: TotalMemory */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAllocMemoryValue, sizeof (objAllocMemoryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseMemoryInfo_FreeMemory
*
* @purpose Get 'FreeMemory'
 *@description  [FreeMemory] This value shows the free memory on the system   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseMemoryInfo_FreeMemory (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t objFreeMemoryValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbInfoSwitchCpuProcessMemFree(L7_UNIT_CURRENT, &objFreeMemoryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objFreeMemoryValue *=1024;
  FPOBJ_TRACE_VALUE (bufp, &objFreeMemoryValue, sizeof (objFreeMemoryValue));

  /* return the object value: AvailableMemory */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFreeMemoryValue,
                           sizeof (objFreeMemoryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

