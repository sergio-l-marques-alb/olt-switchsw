
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
#include "usmdb_util_api.h"

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
  FPOBJ_TRACE_VALUE (bufp, &objFreeMemoryValue, sizeof (objFreeMemoryValue));

  /* return the object value: AvailableMemory */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFreeMemoryValue,
                           sizeof (objFreeMemoryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/******************************************************************************
* @function fpObjGet_baseMemoryInfo_RisingThreshold
*
* @purpose Get 'RisingThreshold'
* @description [RisingThreshold] CPU Utilization Rising Threshold value
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjGet_baseMemoryInfo_RisingThreshold(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;

  FPOBJ_TRACE_ENTER(bufp);

  owa.l7rc = usmdbCpuUtilMonitorParamGet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, sizeof(val));

  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&val, sizeof(val));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/******************************************************************************
* @function fpObjSet_baseMemoryInfo_RisingThreshold
*
* @purpose Set 'RisingThreshold'
* @description [RisingThreshold] CPU Utilization Rising Threshold value
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjSet_baseMemoryInfo_RisingThreshold(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;

  FPOBJ_TRACE_ENTER(bufp);

  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)&val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, owa.len);

  if (val == L7_NULL)
  {
    if ((usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM, val) != L7_SUCCESS) ||
        (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM, val) != L7_SUCCESS) ||
        (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, val) != L7_SUCCESS) ||
        (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM, val) != L7_SUCCESS))
    {
      owa.rc = XLIBRC_FAILURE;
    }
  }
  else
  {
    if (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, val) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
    }
  }

  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/******************************************************************************
* @function fpObjGet_baseMemoryInfo_RisingThresholdIntvl
*
* @purpose Get 'RisingThresholdIntvl'
* @description [RisingThresholdIntvl] CPU Utilization Rising Threshold interval
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjGet_baseMemoryInfo_RisingThresholdIntvl(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;

  FPOBJ_TRACE_ENTER(bufp);

  owa.l7rc = usmdbCpuUtilMonitorParamGet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM, &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, sizeof(val));

  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&val, sizeof(val));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/******************************************************************************
* @function fpObjSet_baseMemoryInfo_RisingThresholdIntvl
*
* @purpose Set 'RisingThresholdIntvl'
* @description [RisingThresholdIntvl] CPU Utilization Rising Threshold interval
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjSet_baseMemoryInfo_RisingThresholdIntvl(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;
  L7_uint32 tmpVal2;

  FPOBJ_TRACE_ENTER(bufp);

  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)&val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  if ((val % FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD) != 0)
  {
    owa.rc = XLIBRC_INVALID_RISING_INTERVAL;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, owa.len);

  if (usmdbCpuUtilMonitorParamGet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, &tmpVal2) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
  }
  else
  {
    if ((val == L7_NULL) || (tmpVal2 == L7_NULL))
    {
      if ((usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM, L7_NULL) != L7_SUCCESS) ||
          (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM, L7_NULL) != L7_SUCCESS) ||
          (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, L7_NULL) != L7_SUCCESS) ||
          (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM, L7_NULL) != L7_SUCCESS))
      {
        owa.rc = XLIBRC_FAILURE;
      }
    }
    else
    {
      if (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM, val) != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;
      }
    }
  }

  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/******************************************************************************
* @function fpObjGet_baseMemoryInfo_FallingThreshold
*
* @purpose Get 'FallingThreshold'
* @description [FallingThreshold] CPU Utilization Falling Threshold value
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjGet_baseMemoryInfo_FallingThreshold(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;

  FPOBJ_TRACE_ENTER(bufp);

  owa.l7rc = usmdbCpuUtilMonitorParamGet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM, &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, sizeof(val));

  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&val, sizeof(val));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/******************************************************************************
* @function fpObjSet_baseMemoryInfo_FallingThreshold
*
* @purpose Set 'FallingThreshold'
* @description [FallingThreshold] CPU Utilization Falling Threshold value
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjSet_baseMemoryInfo_FallingThreshold(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;
  L7_uint32 tmpVal;

  FPOBJ_TRACE_ENTER(bufp);

  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)&val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, owa.len);

  if (usmdbCpuUtilMonitorParamGet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_THRESHOLD_PARAM, &tmpVal) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
  }
  else
  {
    if (val == L7_NULL)
    {
      if (tmpVal > L7_NULL)
      {
        owa.rc = XLIBRC_FALLING_THRESHOLD_CANNOT_BE_ZERO_IF_RISING_IS_NONZERO;
      }
    }
    else if ((val > tmpVal) && (tmpVal != L7_NULL))
    {
      owa.rc = XLIBRC_FALLING_GREATER_THAN_RISING_THRESHOLD;
    }
    else if (val <= tmpVal)
    {
      if (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_FALLING_THRESHOLD_PARAM, val) != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;
      }
    }
  }

  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/******************************************************************************
* @function fpObjGet_baseMemoryInfo_FallingThresholdIntvl
*
* @purpose Get 'FallingThresholdIntvl'
* @description [FallingThresholdIntvl] CPU Utilization Falling Threshold interval
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjGet_baseMemoryInfo_FallingThresholdIntvl(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;

  FPOBJ_TRACE_ENTER(bufp);

  owa.l7rc = usmdbCpuUtilMonitorParamGet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM, &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, sizeof(val));

  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&val, sizeof(val));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/******************************************************************************
* @function fpObjSet_baseMemoryInfo_FallingThresholdIntvl
*
* @purpose Set 'FallingThresholdIntvl'
* @description [FallingThresholdIntvl] CPU Utilization Falling Threshold interval
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjSet_baseMemoryInfo_FallingThresholdIntvl(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;
  L7_uint32 tmpVal;

  FPOBJ_TRACE_ENTER(bufp);

  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)&val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  if ((val % FD_SIM_DEFAULT_CPU_UTIL_MIN_PERIOD) != 0)
  {
    owa.rc = XLIBRC_INVALID_FALLING_INTERVAL;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, owa.len);

  if (usmdbCpuUtilMonitorParamGet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_RISING_PERIOD_PARAM, &tmpVal) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
  }
  else
  {
    if (val == L7_NULL)
    {
      if (tmpVal > L7_NULL)
      {
        owa.rc = XLIBRC_FALLING_INTERVAL_CANNOT_BE_ZERO_IF_RISING_IS_NONZERO;
      }
    }
    else
    {
      if (usmdbCpuUtilMonitorParamSet(USMDB_UNIT_CURRENT, SIM_CPU_UTIL_MONITOR_FALLING_PERIOD_PARAM, val) != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;
      }
    }
  }

  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/******************************************************************************
* @function fpObjGet_baseMemoryInfo_FreeMemoryThreshold
*
* @purpose Get 'FreeMemoryThreshold'
* @description [FreeMemoryThreshold] CPU Free Memory Threshold value
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjGet_baseMemoryInfo_FreeMemoryThreshold(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;

  FPOBJ_TRACE_ENTER(bufp);

  owa.l7rc = usmdbCpuFreeMemoryThresholdGet(USMDB_UNIT_CURRENT, &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, sizeof(val));

  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&val, sizeof(val));

  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/******************************************************************************
* @function fpObjSet_baseMemoryInfo_FreeMemoryThreshold
*
* @purpose Set 'FreeMemoryThreshold'
* @description [FreeMemoryThreshold] CPU Free Memory Threshold value
* @notes 
*
* @return
******************************************************************************/
xLibRC_t fpObjSet_baseMemoryInfo_FreeMemoryThreshold(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibU32_t));
  xLibU32_t val;

  FPOBJ_TRACE_ENTER(bufp);

  owa.rc = xLibBufDataGet(bufp, (xLibU8_t *)&val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE(bufp, &val, owa.len);

  owa.l7rc = usmdbCpuFreeMemoryThresholdSet(USMDB_UNIT_CURRENT, val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    if (owa.l7rc == L7_REQUEST_DENIED)
    {
      owa.rc = XLIBRC_INVALID_FREE_MEMORY_THRESHOLD;
    }
  }

  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

